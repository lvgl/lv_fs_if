# File System Interface

LittlevGL has a [File system](https://docs.littlevgl.com/en/html/overview/file-system.html) module to attache memories which can manipulate with files. Here you find interfaces
- FATFS
- PC (Linux and Widows)
file systems.

You still ne to proivde the drivers, thsi repo gives "only" the bridge between FATFS/PC/etc and LittlevGL.

## Usage
1. Add these lines to you `lv_cong.h`:
```c
/*File system interface*/
#define LV_USE_FS_IF	1
#if LV_USE_FS_IF
#  define LV_FS_IF_FATFS	'\0'
#  define LV_FS_IF_PC		  '\0'
#endif  /*LV_USE_FS_IF*/
```

2. Enable an interface you need by chenging `'\0'` to letter you want to use for that drive. E.g. `'S'` for SD card with FATFS.

3. Call `lv_fs_if_init()` to register the enabled interfaces.


