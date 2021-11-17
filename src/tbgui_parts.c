#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/limits.h>

#include "conf.h"
#include "tbgui_parts.h"
#include "app_state.h"
#include <hal.h>
#include <scale.h>

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
	static lv_style_t container_style;
	static bool init_done = false;
	if (!init_done) {
		lv_style_copy(&container_style, &lv_style_transp);
		container_style.body.padding.inner = PIXEL_SCALE(24);
		init_done = true;
	}

	lv_obj_t * container = lv_cont_create(parent, NULL);
	lv_cont_set_layout(container, LV_LAYOUT_COL_M);
	lv_cont_set_fit2(container, LV_FIT_NONE, LV_FIT_TIGHT);
	if (transp) {
		lv_cont_set_style(container, LV_CONT_STYLE_MAIN, &container_style);
	}
	int width = lv_obj_get_width_fit(parent);
	if (width > lv_obj_get_height_fit(parent)) {
		//width = lv_obj_get_height_fit(parent);
		width = lv_obj_get_height_fit(parent) * 9 / 16;
	}
	lv_obj_set_width(container, width);

	return container;
}

/**
 * Provides a window structure to be filled.
 */
window_t* create_window(char* title_text)
{
	lv_obj_t * scr = lv_cont_create(NULL, NULL);
	lv_disp_load_scr(scr);
	
	lv_obj_t * container = lv_cont_create(scr, NULL);
	lv_cont_set_layout(container, LV_LAYOUT_COL_M);
	lv_cont_set_fit2(container, LV_FIT_FLOOD, LV_FIT_FLOOD);
	lv_cont_set_style(container, LV_CONT_STYLE_MAIN, &lv_style_transp);
	
	lv_obj_t * top_container = add_container(container, true);
	lv_obj_t * main_container = add_container(container, true);
	lv_cont_set_fit2(main_container, LV_FIT_NONE, LV_FIT_NONE);
	lv_obj_t * actions_container = add_container(container, true);

	lv_obj_t * logo = lv_img_create(top_container, NULL);
	char* logo_path = calloc(PATH_MAX, sizeof(char));
	sprintf(
		logo_path,
		"%s?width=%d",
		hal_asset_path("installer.svg"),
		lv_obj_get_width_fit(top_container)
	);
	lv_img_set_src(logo, logo_path);
	free(logo_path);

#if 0
	lv_obj_t * title = lv_label_create(top_container, NULL);
	lv_label_set_text(title, title_text);
	lv_label_set_align(title, LV_LABEL_ALIGN_CENTER);
#endif

	// Catches default focused option
	lv_obj_t* dummy = lv_obj_create(actions_container, NULL);
	lv_obj_set_width(dummy, 0);
	lv_obj_set_height(dummy, 0);
	lv_obj_set_style(dummy, &lv_style_transp_fit);

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
	lv_obj_set_height(
		window->main_container,
		(
		lv_obj_get_height_fit(window->scr)
		- lv_obj_get_height(window->actions_container)
		- lv_obj_get_height(window->top_container)
		- 3 * PIXEL_SCALE(20)
		)
	);
}

/**
 * Changes the `current_window` ref, and presents the window.
 * When presenting, it resets the focus group to the new window actions.
 */
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

	if (window->on_present != NULL) {
		window->on_present(window);
	}
}

void btn_enable_state(lv_obj_t* btn, bool enabled)
{
	lv_obj_set_click(btn, enabled);
	lv_obj_set_opa_scale_enable(btn, true);
	lv_obj_set_opa_scale(btn, (enabled ? LV_OPA_MAX : LV_OPA_50));
}

void enable_disable_actions(window_t* window, bool enabled)
{
	lv_obj_t * child;
	child = lv_obj_get_child_back(window->actions_container, NULL);
	while (child) {
		btn_enable_state(child, enabled);
		child = lv_obj_get_child_back(window->actions_container, child);
	}
}

void btn_event_cb(lv_obj_t * btn, lv_event_t event)
{
	if (!lv_obj_get_click(btn)) {
		return;
	}
	if (event == LV_EVENT_RELEASED) {
		app_actions_t action = (app_actions_t)lv_obj_get_user_data(btn);
		app->action = action;
	}
}
