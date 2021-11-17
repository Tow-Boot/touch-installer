#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/limits.h>

#include "conf.h"
#include "tbgui_parts.h"
#include "app_state.h"
#include "window_erase.h"

void tbgui_erase_window_on_present(window_t* window);

window_t* tbgui_erase_window_init(void)
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
}

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

