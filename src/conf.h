#ifndef TBGUI_CONF_H
#define TBGUI_CONF_H

#include <lvgl/lvgl.h>
#include <lv_drv_conf.h>

#ifndef TBGUI_CHECK_LOG_LOCATION
#	define TBGUI_CHECK_LOG_LOCATION "/tmp/tow-boot-check.log"
#endif

#ifndef TBGUI_INSTALL_TO_SPI
#	define TBGUI_INSTALL_TO_SPI 0
#endif
#ifndef TBGUI_INSTALL_TO_EMMC
#	define TBGUI_INSTALL_TO_EMMC 0
#endif
#ifndef TBGUI_INSTALL_TO_EMMCBOOT
#	define TBGUI_INSTALL_TO_EMMCBOOT 0
#endif

#if LVGL_ENV_SIMULATOR == 1
#define TBGUI_ASSETS_PATH "./"
#define TARGET_BLOCK_DEVICE "/tmp/fake-device.tmp"
#define TOW_BOOT_SOURCE_FILE hal_asset_path("temp.bin")
#define DEVICE_NAME "Simulator"
#endif

#ifndef TBGUI_ASSETS_PATH
#error TBGUI_ASSETS_PATH must be defined
#endif

#ifndef TARGET_BLOCK_DEVICE
#error TARGET_BLOCK_DEVICE must be defined
#endif

#ifndef TOW_BOOT_SOURCE_FILE
#error TOW_BOOT_SOURCE_FILE must be defined
#endif

#ifndef DEVICE_NAME
#error DEVICE_NAME must be defined
#endif

#if LVGL_ENV_SIMULATOR == 0
#	if TBGUI_INSTALL_TO_SPI
#	define INSTALL_LOCATION "SPI Flash"
#	elif TBGUI_INSTALL_TO_EMMC
#	define INSTALL_LOCATION "eMMC"
#	elif TBGUI_INSTALL_TO_EMMCBOOT
#	define INSTALL_LOCATION "eMMC Boot"
#	else
#	error TBGUI_INSTALL_TO_XXX not given
#	endif
#else
#	define INSTALL_LOCATION "simulated storage"
#endif

#define FRAME_RATE 1 / 60
#define SECOND_AS_MICROSECONDS 1000000


#endif
