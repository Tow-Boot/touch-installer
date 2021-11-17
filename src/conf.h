#ifndef TBGUI_CONF_H
#define TBGUI_CONF_H

#include <lvgl/lvgl.h>
#include <lv_drv_conf.h>

#ifndef TBGUI_ASSETS_PATH
#	define TBGUI_ASSETS_PATH "./"
#endif

#ifndef TBGUI_INSTALL_TO_SPI
#	define TBGUI_INSTALL_TO_SPI 0
#endif
#ifndef TBGUI_INSTALL_TO_EMMC
#	define TBGUI_INSTALL_TO_EMMC 0
#endif

#ifdef LVGL_ENV_SIMULATOR
#define DEVICE_NAME "Simulator"
#endif

#ifndef DEVICE_NAME
#error DEVICE_NAME must be defined
#endif

#ifndef LVGL_ENV_SIMULATOR
#	if TBGUI_INSTALL_TO_SPI
#	define INSTALL_LOCATION "SPI Flash"
#	elif TBGUI_INSTALL_TO_EMMC
#	define INSTALL_LOCATION "eMMC"
#	else
#	error TBGUI_INSTALL_TO_XXX not given
#	endif
#else
#	define INSTALL_LOCATION "simulated storage"
#endif

#define FRAME_RATE 1 / 60
#define SECOND_AS_MICROSECONDS 1000000


#endif
