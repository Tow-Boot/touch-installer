#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/limits.h>

#include "conf.h"
#include "tbgui_parts.h"
#include "app_state.h"
#include "window_main.h"

window_t* tbgui_main_window_init(void)
{
	window_t* window = create_window("Tow-Boot " INSTALL_LOCATION " installer");

	lv_obj_t * description_label = lv_label_create(window->main_container, NULL);
	lv_label_set_long_mode(description_label, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(description_label, lv_obj_get_width_fit(window->main_container));
	lv_label_set_text(
		description_label,
		"This application is used to manage the Tow-Boot installation on your " DEVICE_NAME ".\n"
		"\nTow-Boot will be installed on " INSTALL_LOCATION ".\n"
#if TBGUI_INSTALL_TO_EMMC == 1 || LVGL_ENV_SIMULATOR == 1
		"\nInstalling to eMMC will ERASE the internal storage content.\n"
#endif
		"\nUse one of the following options."
	);

	// Buttons

	add_button(
		window->actions_container,
		APP_ACTION_INSTALL,
		"Install Tow-Boot to " INSTALL_LOCATION
	);
#if TBGUI_INSTALL_TO_SPI || LVGL_ENV_SIMULATOR
	add_button(
		window->actions_container,
		APP_ACTION_ERASE,
		"Erase " INSTALL_LOCATION " content"
	);
#endif
	add_button(
		window->actions_container,
		APP_ACTION_POWEROFF,
		"Power off"
	);

	finalize_window(window);

	//lv_cont_set_layout(window->main_container, LV_LAYOUT_OFF);
	//lv_obj_align(description_label, NULL, LV_ALIGN_CENTER, 0, 0);

	app->main_window = window;

	return window;
}
