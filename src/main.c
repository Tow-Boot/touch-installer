#include <unistd.h>
#include "conf.h"

#include <hal.h>
#include <scale.h>
#include <lv_lib_freetype/lv_freetype.h>
#include <lv_lib_nanosvg/lv_nanosvg.h>

#include "theme.h"
#include "app_actions.h"
#include "app_state.h"
#include "tbgui_parts.h"
#include "windows.h"

#if USE_MONITOR
extern int monitor_height;
extern int monitor_width;
#endif

// Define `app` once.
app_state_t* app;

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
	tbgui_theme_default();
}

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
		handle_app_actions(app);
	}
}
