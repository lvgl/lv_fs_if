/**
 * @file lv_fs_if.h
 *
 */

#ifndef LV_FS_IF_H
#define LV_FS_IF_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lvgl.h"

/*********************
 *      DEFINES
 *********************/

#ifndef LV_FS_IF_LETTER
# define LV_FS_IF_LETTER 'F'
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Register a driver for the File system interface
 */
void lv_fs_if_init(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_FS_IF_H*/

