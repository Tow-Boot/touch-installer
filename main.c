#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <lvgl/lvgl.h>
#include <lv_drv_conf.h>

#include <hal.h>
#include <lv_lib_freetype/lv_freetype.h>
#include <lv_lib_nanosvg/lv_nanosvg.h>

#define FRAME_RATE 1 / 60
#define SECOND_AS_MICROSECONDS 1000000

#if USE_MONITOR
extern int monitor_height;
extern int monitor_width;
#endif

static void btn_event_cb(lv_obj_t * btn, lv_event_t event);

enum app_actions {
	APP_ACTION_NONE,
	APP_ACTION_BACK,
	APP_ACTION_INSTALL,
	APP_ACTION_ERASE,
	APP_ACTION_POWEROFF,
	_APP_ACTIONS_COUNT,
};
typedef enum app_actions app_actions_t;

typedef struct window {
	lv_obj_t* container;
	lv_obj_t* scr;
	lv_obj_t* main_container;
	lv_obj_t* actions_container;
	lv_obj_t* top_container;
} window_t;

lv_obj_t* add_button(lv_obj_t* parent, app_actions_t action, char* label_text)
{
    lv_obj_t * btn = lv_btn_create(parent, NULL);
    lv_obj_set_event_cb(btn, btn_event_cb);
	lv_btn_set_fit2(btn, LV_FIT_FLOOD, LV_FIT_TIGHT);
	lv_obj_set_user_data(btn, (void*)action);
    lv_obj_t * label = lv_label_create(btn, NULL);
    lv_label_set_text(label, label_text);

	return btn;
}

lv_obj_t* add_container(lv_obj_t* parent, bool transp)
{
	lv_obj_t * container = lv_cont_create(parent, NULL);
	lv_cont_set_layout(container, LV_LAYOUT_COL_M);
	lv_cont_set_fit2(container, LV_FIT_NONE, LV_FIT_TIGHT);
	if (transp) {
		lv_cont_set_style(container, LV_CONT_STYLE_MAIN, &lv_style_transp_fit);
	}
	int width = lv_obj_get_width_fit(parent);
	if (width > lv_obj_get_height_fit(parent)) {
		width = lv_obj_get_height_fit(parent);
		//width = lv_obj_get_height_fit(parent) * 9 / 16;
	}
	lv_obj_set_width(container, width);

	return container;
}

/**
 * Provides a window structure to be filled.
 */
window_t* create_window(char* title_text)
{
    lv_obj_t * scr = lv_page_create(NULL, NULL);
    lv_disp_load_scr(scr);
	
	lv_obj_t * container = lv_cont_create(scr, NULL);
	lv_cont_set_layout(container, LV_LAYOUT_COL_M);
	lv_cont_set_fit2(container, LV_FIT_FLOOD, LV_FIT_FLOOD);
	lv_cont_set_style(container, LV_CONT_STYLE_MAIN, &lv_style_transp_fit);
	
	lv_obj_t * top_container = add_container(container, false);
	lv_obj_t * main_container = add_container(container, true);
	lv_cont_set_fit2(main_container, LV_FIT_NONE, LV_FIT_NONE);
	lv_obj_t * actions_container = add_container(container, true);

	//lv_obj_t * top_container = lv_cont_create(container, NULL);
	//lv_cont_set_layout(top_container, LV_LAYOUT_COL_M);
	//lv_cont_set_fit2(top_container, LV_FIT_NONE, LV_FIT_TIGHT);
	////lv_cont_set_style(top_container, LV_CONT_STYLE_MAIN, &lv_style_transp_fit);
	//
	//lv_obj_t * main_container = lv_cont_create(container, NULL);
	//lv_cont_set_layout(main_container, LV_LAYOUT_COL_M);
	//lv_cont_set_fit2(main_container, LV_FIT_NONE, LV_FIT_NONE);
	//lv_cont_set_style(main_container, LV_CONT_STYLE_MAIN, &lv_style_transp_fit);
	//
	//lv_obj_t * actions_container = lv_cont_create(container, NULL);
	//lv_cont_set_layout(actions_container, LV_LAYOUT_COL_M);
	//lv_cont_set_fit2(actions_container, LV_FIT_NONE, LV_FIT_TIGHT);
	//lv_cont_set_style(actions_container, LV_CONT_STYLE_MAIN, &lv_style_transp_fit);

    lv_obj_t * title = lv_label_create(top_container, NULL);
    lv_label_set_text(title, title_text);
	lv_label_set_align(title, LV_LABEL_ALIGN_CENTER);

	window_t* window = calloc(1, sizeof(window_t));
	window->container = container;
	window->scr = scr;
	window->main_container = main_container;
	window->actions_container = actions_container;
	window->top_container = top_container;

	return window;
}

/**
 * Does house-keeping with the window after it's been filled.
 */
void finalize_window(window_t* window)
{
	// Resizes the main element.
	// LVGL at the current release segfaults trying to LV_FIT_FILL in this situation.
	// XXX: get actual window->container padding
	int padding = 12;
	lv_obj_set_height(
		window->main_container,
		(
		lv_obj_get_height_fit(window->scr)
		- lv_obj_get_height(window->actions_container)
		- lv_obj_get_height(window->top_container)
		- 5 * padding
		)
	);
}

void tbgui_main_window_init(void)
{
	window_t* window = create_window("Tow-Boot SPI installer");

    lv_obj_t * description_label = lv_label_create(window->main_container, NULL);
	lv_label_set_long_mode(description_label, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(description_label, lv_obj_get_width_fit(window->main_container));
	// TODO: read text from configuration
    lv_label_set_text(
		description_label,
		"\nThis application is used to manage the Tow-Boot installation on SPI.\n\nUse one of the following options.\n"
	);

	// Buttons

	add_button(
		window->actions_container,
		APP_ACTION_INSTALL,
		"Install Tow-Boot to SPI"
	);
	add_button(
		window->actions_container,
		APP_ACTION_ERASE,
		"Erase SPI Flash content"
	);
	add_button(
		window->actions_container,
		APP_ACTION_POWEROFF,
		"Power-Off the device"
	);

	finalize_window(window);

	lv_cont_set_layout(window->main_container, LV_LAYOUT_OFF);
	lv_obj_align(description_label, NULL, LV_ALIGN_CENTER, 0, 0);
}

static void btn_event_cb(lv_obj_t * btn, lv_event_t event)
{
    if (event == LV_EVENT_RELEASED) {
		int res = 0;
		app_actions_t action = (app_actions_t)lv_obj_get_user_data(btn);
		switch (action) {
			case APP_ACTION_NONE:
				break;
			case APP_ACTION_BACK:
				printf("Will go back to main screen...\n");
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



int main()
{
#if USE_MONITOR
#	if 1
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

	hal_init("./");
	mn_hal_default_dpi = 180;
	lv_nanosvg_init();
	lv_anim_core_init();

	lv_theme_t * th = lv_theme_night_init(225, NULL);
	lv_theme_set_current(th);

	tbgui_main_window_init();

	while (true) {
		lv_task_handler();
		usleep(SECOND_AS_MICROSECONDS * FRAME_RATE);
	}
}
