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

typedef struct app_state {
	app_actions_t action;
	struct window* main_window;
	struct window* install_window;
	struct window* erase_window;
} app_state_t;

app_state_t* app;

////////////////////////////////////////
//                                    //
////////////////////////////////////////

window_t* tbgui_main_window_init(void) // {{{
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
		"Power off the device"
	);

	finalize_window(window);

	lv_cont_set_layout(window->main_container, LV_LAYOUT_OFF);
	lv_obj_align(description_label, NULL, LV_ALIGN_CENTER, 0, 0);

	app->main_window = window;

	return window;
} // }}}

void tbgui_install_window_on_present(window_t* window);

window_t* tbgui_install_window_init(void) // {{{
{
	window_t* window = create_window("Installation");

	lv_obj_t* bar = lv_bar_create(window->main_container, NULL);
	lv_obj_set_width(bar, lv_obj_get_width_fit(window->main_container));
	lv_bar_set_value(bar, 0, LV_ANIM_OFF);

	// Buttons

	add_button(
		window->actions_container,
		APP_ACTION_DO_INSTALL,
		"Start installation"
	);

	add_button(
		window->actions_container,
		APP_ACTION_BACK,
		"Go back to the menu"
	);

	finalize_window(window);

	lv_cont_set_layout(window->main_container, LV_LAYOUT_OFF);

	app->install_window = window;
	window->back_location = app->main_window;
	window->on_present = tbgui_install_window_on_present;

	return window;
} // }}}

void tbgui_erase_window_on_present(window_t* window);

window_t* tbgui_erase_window_init(void) // {{{
{
	window_t* window = create_window("Erase");

	lv_obj_t* bar = lv_bar_create(window->main_container, NULL);
	lv_obj_set_width(bar, lv_obj_get_width_fit(window->main_container));
	lv_bar_set_value(bar, 0, LV_ANIM_OFF);

	// Buttons

	add_button(
		window->actions_container,
		APP_ACTION_DO_ERASE,
		"Erase " INSTALL_LOCATION
	);

	add_button(
		window->actions_container,
		APP_ACTION_BACK,
		"Go back to the menu"
	);

	finalize_window(window);

	lv_cont_set_layout(window->main_container, LV_LAYOUT_OFF);

	app->erase_window = window;
	window->back_location = app->main_window;
	window->on_present = tbgui_erase_window_on_present;

	return window;
} // }}}

void tbgui_erase_window_on_present(window_t* window)
{
	// XXX assumes bar is first child
	lv_obj_t* bar = lv_obj_get_child(window->main_container, NULL);
	lv_bar_set_value(bar, 0, LV_ANIM_OFF);
	enable_disable_actions(window, true);
}

void handle_erase(window_t* window)
{
	enable_disable_actions(window, false);

	// XXX assumes bar is first child
	lv_obj_t* bar = lv_obj_get_child(window->main_container, NULL);

	// TODO: number of chunks we write.
	lv_bar_set_range(bar, 0, 100);

	int i = 1;
	while (i <= 100) {
		lv_bar_set_value(bar, i, LV_ANIM_OFF);
		lv_task_handler();

		// DO WORK HERE...
		usleep(SECOND_AS_MICROSECONDS * 0.05);

		i++;
	}

	// Assume back button is the last option.
	lv_obj_t* back_button = lv_obj_get_child(window->actions_container, NULL);
	btn_enable_state(back_button, true);
}

void tbgui_install_window_on_present(window_t* window)
{
	// XXX assumes bar is first child
	lv_obj_t* bar = lv_obj_get_child(window->main_container, NULL);
	lv_bar_set_value(bar, 0, LV_ANIM_OFF);
	enable_disable_actions(window, true);
}

void handle_install(window_t* window)
{
	enable_disable_actions(window, false);

	// XXX assumes bar is first child
	lv_obj_t* bar = lv_obj_get_child(window->main_container, NULL);

	// TODO: number of chunks we write.
	lv_bar_set_range(bar, 0, 100);

	int i = 1;
	while (i <= 100) {
		lv_bar_set_value(bar, i, LV_ANIM_OFF);
		lv_task_handler();

		// DO WORK HERE...
		usleep(SECOND_AS_MICROSECONDS * 0.05);

		i++;
	}

	// Assume back button is the last option.
	lv_obj_t* back_button = lv_obj_get_child(window->actions_container, NULL);
	btn_enable_state(back_button, true);
}

void btn_event_cb(lv_obj_t * btn, lv_event_t event)
{
    if (event == LV_EVENT_RELEASED) {
		app_actions_t action = (app_actions_t)lv_obj_get_user_data(btn);
		app->action = action;
    }
}

////////////////////////////////////////
// Initialization {{{

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

void tbgui_app_init()
{
#if USE_MONITOR
#	if 0
#	elif 1
	monitor_width = 360;
	monitor_height = 640;
#	elif 0
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
}

// }}}
////////////////////////////////////////

int main()
{
	tbgui_app_init();

	app = calloc(1, sizeof(app_state_t));
	tbgui_main_window_init();
	tbgui_install_window_init();
	tbgui_erase_window_init();

	present_window(app->main_window);

	while (true) {
		lv_task_handler();
		usleep(SECOND_AS_MICROSECONDS * FRAME_RATE);
		if (app->action != APP_ACTION_NONE) {
			switch (app->action) {
				case APP_ACTION_NONE:
					break;
				case APP_ACTION_BACK:
					present_window(current_window->back_location);
					break;
				case APP_ACTION_INSTALL:
					present_window(app->install_window);
					break;
				case APP_ACTION_ERASE:
					present_window(app->erase_window);
					break;
				case APP_ACTION_POWEROFF:
#ifdef LVGL_ENV_SIMULATOR
					exit(0);
#else
					int res = 0;
					res = system("poweroff");
					(void)res;
#endif
					break;
				case APP_ACTION_DO_ERASE:
					handle_erase(app->erase_window);
					break;
				case APP_ACTION_DO_INSTALL:
					handle_install(app->install_window);
					break;
				case _APP_ACTIONS_COUNT:
				default:
					printf("Unhandled action: [%d]\n", app->action);
			}
			app->action = APP_ACTION_NONE;
		}
	}
}
