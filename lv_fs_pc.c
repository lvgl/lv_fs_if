/**
 * @file lv_port_disp_templ.c
 *
 */


/*********************
 *      INCLUDES
 *********************/
#include "lv_fs_if.h"
#ifdef LV_FS_IF_PC

#include <stdio.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/* Create a type to store the required data about your file. */
typedef  FILE * file_t;

/*Similarly to `file_t` create a type for directory reading too */
#ifndef  WIN32
typedef  DIR * dir_t;
#else
typedef HANDLE dir_t;
#endif


/**********************
 *  STATIC PROTOTYPES
 **********************/
static void fs_init(void);

static lv_fs_res_t fs_open (void * file_p, const char * path, lv_fs_mode_t mode);
static lv_fs_res_t fs_close (void * file_p);
static lv_fs_res_t fs_read (void * file_p, void * buf, uint32_t btr, uint32_t * br);
static lv_fs_res_t fs_write(void * file_p, const void * buf, uint32_t btw, uint32_t * bw);
static lv_fs_res_t fs_seek (void * file_p, uint32_t pos);
static lv_fs_res_t fs_size (void * file_p, uint32_t * size_p);
static lv_fs_res_t fs_tell (void * file_p, uint32_t * pos_p);
static lv_fs_res_t fs_remove (const char *path);
static lv_fs_res_t fs_trunc (void * file_p);
static lv_fs_res_t fs_rename (const char * oldname, const char * newname);
static lv_fs_res_t fs_free (uint32_t * total_p, uint32_t * free_p);
static lv_fs_res_t fs_dir_open (void * rddir_p, const char *path);
static lv_fs_res_t fs_dir_read (void * rddir_p, char *fn);
static lv_fs_res_t fs_dir_close (void * rddir_p);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_fs_if_init(void)
{
    /*---------------------------------------------------
     * Register the file system interface  in LittlevGL
     *--------------------------------------------------*/

    /* Add a simple drive to open images */
    lv_fs_drv_t fs_drv;                         /*A driver descriptor*/
    memset(&fs_drv, 0, sizeof(lv_fs_drv_t));    /*Initialization*/

    /*Set up fields...*/
    fs_drv.file_size = sizeof(file_t);
    fs_drv.letter = 'P';
    fs_drv.open = fs_open;
    fs_drv.close = fs_close;
    fs_drv.read = fs_read;
    fs_drv.write = fs_write;
    fs_drv.seek = fs_seek;
    fs_drv.tell = fs_tell;
    fs_drv.free = fs_free;
    fs_drv.size = fs_size;
    fs_drv.remove = fs_remove;
    fs_drv.rename = fs_rename;
    fs_drv.trunc = fs_trunc;

    fs_drv.rddir_size = sizeof(dir_t);
    fs_drv.dir_close = fs_dir_close;
    fs_drv.dir_open = fs_dir_open;
    fs_drv.dir_read = fs_dir_read;

    lv_fs_add_drv(&fs_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Open a file
 * @param file_p pointer to a file_t variable
 * @param path path to the file beginning with the driver letter (e.g. S:/folder/file.txt)
 * @param mode read: FS_MODE_RD, write: FS_MODE_WR, both: FS_MODE_RD | FS_MODE_WR
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_open (void * file_p, const char * path, lv_fs_mode_t mode)
{
    errno = 0;

    const char * flags = "";

    if(mode == LV_FS_MODE_WR) flags = "wb";
    else if(mode == LV_FS_MODE_RD) flags = "rb";
    else if(mode == (LV_FS_MODE_WR | LV_FS_MODE_RD)) flags = "rb+";

    /*Make the path relative to the current directory (the projects root folder)*/

#ifndef WIN32
    char buf[256];
    sprintf(buf, "./" LV_FS_PC_PATH "%s", fn); 
#else
    char buf[256];
    sprintf(buf, ".\\" LV_FS_PC_PATH "\\%s", fn);
#endif

    file_t f = fopen(buf, flags);
    if((long int)f <= 0) return LV_FS_RES_UNKNOWN;
    
    /*Be sure we are the beginning of the file*/
    fseek(f, 0, SEEK_SET);

    /* 'file_p' is pointer to a file descriptor and
     * we need to store our file descriptor here*/
    file_t * fp = file_p;        /*Just avoid the confusing casings*/
    *fp = f;
   
#endif
    return LV_FS_RES_OK;
}


/**
 * Close an opened file
 * @param file_p pointer to a file_t variable. (opened with lv_ufs_open)
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_close (void * file_p)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    pc_file_t * fp = file_p;        /*Just avoid the confusing casings*/
    fclose(*fp);
    return LV_FS_RES_OK;
}

/**
 * Read data from an opened file
 * @param file_p pointer to a file_t variable.
 * @param buf pointer to a memory block where to store the read data
 * @param btr number of Bytes To Read
 * @param br the real number of read bytes (Byte Read)
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_read (void * file_p, void * buf, uint32_t btr, uint32_t * br)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    pc_file_t * fp = file_p;        /*Just avoid the confusing casings*/
    *br = fread(buf, 1, btr, *fp);
    return LV_FS_RES_OK;
}

/**
 * Write into a file
 * @param file_p pointer to a file_t variable
 * @param buf pointer to a buffer with the bytes to write
 * @param btr Bytes To Write
 * @param br the number of real written bytes (Bytes Written). NULL if unused.
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_write(void * file_p, const void * buf, uint32_t btw, uint32_t * bw)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;
    
    pc_file_t * fp = file_p;        /*Just avoid the confusing casings*/
    *bw = fwrite(buf, 1, btw, *fp);
    return LV_FS_RES_OK;
}

/**
 * Set the read write pointer. Also expand the file size if necessary.
 * @param file_p pointer to a file_t variable. (opened with lv_ufs_open )
 * @param pos the new position of read write pointer
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_seek (void * file_p, uint32_t pos)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;
    
    pc_file_t * fp = file_p;        /*Just avoid the confusing casings*/
    fseek(*fp, pos, SEEK_SET);
    return LV_FS_RES_OK;
}

/**
 * Give the size of a file bytes
 * @param file_p pointer to a file_t variable
 * @param size pointer to a variable to store the size
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_size (void * file_p, uint32_t * size_p)
{
    pc_file_t * fp = file_p;        /*Just avoid the confusing casings*/

    fseek(*fp, 0L, SEEK_END);
    *sz = ftell(*fp);
    fseek(*fp, 0L, SEEK_SET);

    return LV_FS_RES_OK;
}
/**
 * Give the position of the read write pointer
 * @param file_p pointer to a file_t variable.
 * @param pos_p pointer to to store the result
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_tell (void * file_p, uint32_t * pos_p)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    pc_file_t * fp = file_p;        /*Just avoid the confusing casings*/
    *pos_p = ftell(*fp);
    return LV_FS_RES_OK;
}

/**
 * Delete a file
 * @param path path of the file to delete
 * @return  LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_remove (const char *path)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    /* Add your code here*/

    return res;
}

/**
 * Truncate the file size to the current position of the read write pointer
 * @param file_p pointer to an 'ufs_file_t' variable. (opened with lv_fs_open )
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_trunc (void * file_p)
{
    pc_file_t * fp = file_p;        /*Just avoid the confusing casings*/

    fflush(*fp);                    /*If not syncronized fclose can write the truncated part*/
    uint32_t p  = ftell(*fp);
    ftruncate(fileno(*fp), p);
    return LV_FS_RES_OK;
}

/**
 * Rename a file
 * @param oldname path to the file
 * @param newname path with the new name
 * @return LV_FS_RES_OK or any error from 'fs_res_t'
 */
static lv_fs_res_t fs_rename (const char * oldname, const char * newname)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;
    
    char new[512];
    char old[512];

    sprintf(old, LV_FS_PATH "/%s", oldname);
    sprintf(new, LV_FS_PATH "/%s", newname);

    int r = rename(old, new);

    if(r == 0) return LV_FS_RES_OK;
    else return LV_FS_RES_UNKNOWN;
}

/**
 * Get the free and total size of a driver in kB
 * @param letter the driver letter
 * @param total_p pointer to store the total size [kB]
 * @param free_p pointer to store the free size [kB]
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_free (uint32_t * total_p, uint32_t * free_p)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    /* Add your code here*/

    return res;
}


#ifdef WIN32
static char next_fn[256];
#endif

/**
 * Initialize a 'fs_read_dir_t' variable for directory reading
 * @param rddir_p pointer to a 'fs_read_dir_t' variable
 * @param path path to a directory
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_dir_open (void * rddir_p, const char *path)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    pc_dir_t d;
#ifndef WIN32
    /*Make the path relative to the current directory (the projects root folder)*/
    char buf[256];
    sprintf(buf, LV_FS_PC_PATH "/%s", path);
    if ((d = opendir(buf)) == NULL) {
        printf("opendir() error: %s", buf);
        return LV_FS_RES_FS_ERR;
    } else {
        /* 'dir_p' is pointer to a file descriptor and
         * we need to store our file descriptor here*/
        pc_dir_t * dp = dir_p;        /*Just avoid the confusing casings*/
        *dp = d;
    }
#else

    d = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA fdata;

    /*Make the path relative to the current directory (the projects root folder)*/
    char buf[256];
    sprintf(buf, LV_FS_PC_PATH "\\%s\\*", path);

    strcpy(next_fn, "");
    d = FindFirstFile(buf, &fdata);
    do {
    	if (strcmp(fdata.cFileName, ".") == 0 || strcmp(fdata.cFileName, "..") == 0) {
    		continue;
    	} else {

    		if (fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    		{
    			sprintf(next_fn, "/%s", fdata.cFileName);
    		} else {
    			sprintf(next_fn, "%s", fdata.cFileName);
    		}
    		break;
    	}
    } while(FindNextFileA(d, &fdata));

    pc_dir_t * dp = dir_p;        /*Just avoid the confusing casings*/
   *dp = d;

#endif

    return LV_FS_RES_OK;
}

/**
 * Read the next filename form a directory.
 * The name of the directories will begin with '/'
 * @param rddir_p pointer to an initialized 'fs_read_dir_t' variable
 * @param fn pointer to a buffer to store the filename
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_dir_read (void * rddir_p, char *fn)
{
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;
 pc_dir_t * dp = dir_p;        /*Just avoid the confusing casings*/

#ifndef WIN32
    struct dirent *entry;
    do {
        entry = readdir(*dp);

        if(entry) {
            if(entry->d_type == DT_DIR) sprintf(fn, "/%s", entry->d_name);
            else strcpy(fn, entry->d_name);
        } else {
            strcpy(fn, "");
        }
    } while(strcmp(fn, "/.") == 0 || strcmp(fn, "/..") == 0);
#else
    strcpy(fn, next_fn);

    strcpy(next_fn, "");
    WIN32_FIND_DATA fdata;

    if(FindNextFile(*dp, &fdata) == false) return LV_FS_RES_OK;
    do {
    	if (strcmp(fdata.cFileName, ".") == 0 || strcmp(fdata.cFileName, "..") == 0) {
    		continue;
    	} else {

    		if (fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    		{
    			sprintf(next_fn, "/%s", fdata.cFileName);
    		} else {
    			sprintf(next_fn, "%s", fdata.cFileName);
    		}
    		break;
    	}
    } while(FindNextFile(*dp, &fdata));

#endif
    return LV_FS_RES_OK;
}

/**
 * Close the directory reading
 * @param rddir_p pointer to an initialized 'fs_read_dir_t' variable
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_dir_close (void * rddir_p)
{   pc_dir_t * dp = dir_p;
#ifndef WIN32

    closedir(*dp);
#else
    FindClose(*dp);
    *dp = INVALID_HANDLE_VALUE;
#endif
    return LV_FS_RES_OK;
}

#endif
