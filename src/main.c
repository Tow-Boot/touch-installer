#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "conf.h"

#include <hal.h>
#include <scale.h>
#include <lv_lib_freetype/lv_freetype.h>
#include <lv_lib_nanosvg/lv_nanosvg.h>

#include "theme.h"
#include "app_state.h"
#include "tbgui_parts.h"
#include "windows.h"

////////////////////////////////////////
// Globals                            //
////////////////////////////////////////

#if USE_MONITOR
extern int monitor_height;
extern int monitor_width;
#endif

app_state_t* app;

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
