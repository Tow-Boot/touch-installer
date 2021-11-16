#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "conf.h"

#include <hal.h>
#include <scale.h>
#include <lv_lib_freetype/lv_freetype.h>
#include <lv_lib_nanosvg/lv_nanosvg.h>

#include "theme.h"
#include "tbgui_parts.h"

////////////////////////////////////////
// Globals                            //
////////////////////////////////////////

#if USE_MONITOR
extern int monitor_height;
extern int monitor_width;
#endif

void btn_event_cb(lv_obj_t * btn, lv_event_t event);

window_t* current_window;

////////////////////////////////////////
//                                    //
////////////////////////////////////////

window_t* tbgui_main_window_init(void)
{
	window_t* window = create_window("Tow-Boot " INSTALL_LOCATION " installer");

    lv_obj_t * description_label = lv_label_create(window->main_container, NULL);
	lv_label_set_long_mode(description_label, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(description_label, lv_obj_get_width_fit(window->main_container));
    lv_label_set_text(
		description_label,
		"\nThis application is used to manage the Tow-Boot installation on " INSTALL_LOCATION ".\n\nUse one of the following options.\n"
	);

	// Buttons

	add_button(
		window->actions_container,
		APP_ACTION_INSTALL,
		"Install Tow-Boot to " INSTALL_LOCATION
	);
#if TBGUI_INSTALL_TO_SPI
	add_button(
		window->actions_container,
		APP_ACTION_ERASE,
		"Erase " INSTALL_LOCATION " content"
	);
#endif
	add_button(
		window->actions_container,
		APP_ACTION_POWEROFF,
		"Power off the device"
	);

	finalize_window(window);

	lv_cont_set_layout(window->main_container, LV_LAYOUT_OFF);
	lv_obj_align(description_label, NULL, LV_ALIGN_CENTER, 0, 0);

	return window;
}

void btn_event_cb(lv_obj_t * btn, lv_event_t event)
{
    if (event == LV_EVENT_RELEASED) {
		int res = 0;
		app_actions_t action = (app_actions_t)lv_obj_get_user_data(btn);
		switch (action) {
			case APP_ACTION_NONE:
				break;
			case APP_ACTION_BACK:
				printf("Will go back to currentwindow->back_location...\n");
				// present_window(current_window->back_location);
				break;
			case APP_ACTION_INSTALL:
				printf("Will install...\n");
				res = system("echo tbgui-action-installer");
				break;
			case APP_ACTION_ERASE:
				printf("Will erase...\n");
				res = system("echo tbgui-action-erase");
				break;
			case APP_ACTION_POWEROFF:
#ifdef LVGL_ENV_SIMULATOR
				exit(0);
#else
				res = system("poweroff");
#endif
				break;
			case _APP_ACTIONS_COUNT:
			default:
			printf("Unhandled action: [%d]\n", action);
		}
		if (res) {
			printf("Warning: command unexpectedly failed...");
		}
    }
}


void font_init()
{
	int ret = 0;

	// Landscape mode?
	if (disp_drv.hor_res > disp_drv.ver_res) {
		// This app is tight in landscape mode since we show the portrait mode app centered.
		// Let's make the scaling work out better.
		mn_hal_default_dpi = mn_hal_default_dpi * 0.6;
	}

	static lv_font_t font;
	ret = lv_freetype_font_init(&font, hal_asset_path("fonts/RobotoCondensed-Regular.ttf"), POINTS_SCALE(32));
	if (ret == FT_Err_Ok) {
		mn_hal_default_font = (void *)&font;
	}
}

void present_window(window_t* window)
{
	current_window = window;
	lv_scr_load(window->scr);

	lv_group_t* group = lvgui_get_focus_group();
	lv_group_remove_all_objs(group);

	lv_obj_t * child;
	child = lv_obj_get_child_back(window->actions_container, NULL);
	while(child) {
		lv_group_add_obj(group, child);
		child = lv_obj_get_child_back(window->actions_container, child);
	}
}

int main()
{
#if USE_MONITOR
#	if 0
#	elif 1
	monitor_width = 360;
	monitor_height = 640;
#	elif 1
	monitor_width = 720;
	monitor_height = 1280;
#   elif 0
	monitor_width = 1080 * 9 / 16;
	monitor_height = 1080;
#   elif 1
	monitor_width = 1920;
	monitor_height = 1080;
#	else
	monitor_width = 1280;
	monitor_height = 720;
#	endif
#endif

	hal_init(TBGUI_ASSETS_PATH);
	font_init();
	lv_nanosvg_init();
	lv_anim_core_init();

	lv_theme_t * th = lv_theme_tb_init(NULL, NULL);
	lv_theme_set_current(th);

	present_window(tbgui_main_window_init());

	while (true) {
		lv_task_handler();
		usleep(SECOND_AS_MICROSECONDS * FRAME_RATE);
	}
}
