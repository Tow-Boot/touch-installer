#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/limits.h>

#include "conf.h"
#include "tbgui_parts.h"
#include "app_state.h"
#include "window_erase.h"

void tbgui_erase_window_on_present(window_t* window);

typedef struct erase_window_priv {
	lv_obj_t* back_button;
	lv_obj_t* progress_bar;
	lv_obj_t* progress_label;
} erase_window_priv_t;

window_t* tbgui_erase_window_init(void)
{
	erase_window_priv_t* private = calloc(1, sizeof(erase_window_priv_t));
	window_t* window = create_window("Erase");

	lv_obj_t* bar = lv_bar_create(window->main_container, NULL);
	lv_obj_set_width(bar, lv_obj_get_width_fit(window->main_container));
	lv_bar_set_value(bar, 0, LV_ANIM_OFF);
	private->progress_bar = bar;

	lv_obj_t * progress_label = lv_label_create(window->main_container, NULL);
	lv_label_set_long_mode(progress_label, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(progress_label, lv_obj_get_width_fit(window->main_container));
	lv_label_set_text(progress_label, "");
	private->progress_label = progress_label;

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
	private->back_button = lv_obj_get_child(window->actions_container, NULL);

	finalize_window(window);

	lv_cont_set_layout(window->main_container, LV_LAYOUT_OFF);

	app->erase_window = window;
	window->back_location = app->main_window;
	window->on_present = tbgui_erase_window_on_present;
	window->private = private;

	return window;
}

void tbgui_erase_window_on_present(window_t* window)
{
	erase_window_priv_t* private = window->private;
	lv_bar_set_value(private->progress_bar, 0, LV_ANIM_OFF);
	lv_label_set_text(private->progress_label, "Use the “Erase " INSTALL_LOCATION "” button to erase the platform firmware installation from the given media.");
	enable_disable_actions(window, true);
}

void handle_erase(window_t* window)
{
	erase_window_priv_t* private = window->private;

	enable_disable_actions(window, false);

	lv_obj_t* bar = private->progress_bar;

	// TODO: number of chunks we write.
	lv_bar_set_range(bar, 0, 100);

	lv_label_set_text(
		private->progress_label,
		"Preparing for erasing..."
	);
	usleep(SECOND_AS_MICROSECONDS * FRAME_RATE);
	lv_task_handler();

	// DO CHECKS HERE
	usleep(SECOND_AS_MICROSECONDS);

	int i = 1;
	while (i <= 100) {
		lv_bar_set_value(bar, i, LV_ANIM_OFF);
		lv_label_set_text_fmt(
			private->progress_label,
			"Erased %d out of %d bytes...",
			i,
			100
		);
		lv_task_handler();

		// DO WORK HERE...
		usleep(SECOND_AS_MICROSECONDS * 0.01);

		i++;
	}

	lv_label_set_text(
		private->progress_label,
		"The " INSTALL_LOCATION " was erased successfully.\n"
		"\nYou can now proceed with the installation on your " DEVICE_NAME "\n"
		"\nAlternatively you can safely power off the " DEVICE_NAME " and remove the installation media."
	);

	tbgui_theme_success();
	btn_enable_state(private->back_button, true);
}
