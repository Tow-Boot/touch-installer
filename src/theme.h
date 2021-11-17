/**
 * @file lv_theme_tb.h
 *
 */

#ifndef LV_THEME_TB_H
#define LV_THEME_TB_H

#define LV_USE_THEME_TB 1


#define THEME_HUE_RED     4
#define THEME_HUE_GREEN 120
#define THEME_HUE_BLUE  212

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#else
#include "../lvgui/lv_conf.h"
#endif

#if LV_USE_THEME_TB

lv_theme_t * lv_theme_tb_init(int hue, lv_font_t * font, lv_font_t * button_font);
lv_theme_t * lv_theme_get_tb(void);

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_THEME_TB_H*/
