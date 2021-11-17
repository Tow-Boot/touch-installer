#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/limits.h>

#include "conf.h"
#include "tbgui_parts.h"
#include "app_state.h"
#include "window_install.h"

void tbgui_install_window_on_present(window_t* window);

typedef struct install_window_priv {
	lv_obj_t* back_button;
	lv_obj_t* progress_bar;
	lv_obj_t* progress_label;
} install_window_priv_t;

window_t* tbgui_install_window_init(void)
{
	install_window_priv_t* private = calloc(1, sizeof(install_window_priv_t));
	window_t* window = create_window("Installation");

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
		APP_ACTION_DO_INSTALL,
		"Start installation"
	);

	add_button(
		window->actions_container,
		APP_ACTION_BACK,
		"Go back to the menu"
	);
	private->back_button = lv_obj_get_child(window->actions_container, NULL);

	finalize_window(window);

	lv_cont_set_layout(window->main_container, LV_LAYOUT_OFF);

	app->install_window = window;
	window->back_location = app->main_window;
	window->on_present = tbgui_install_window_on_present;
	window->private = private;

	return window;
}

void tbgui_install_window_on_present(window_t* window)
{
	install_window_priv_t* private = window->private;
	lv_bar_set_value(private->progress_bar, 0, LV_ANIM_OFF);
	lv_label_set_text(private->progress_label, "Use the “Start installation” button to start the installation.");
	enable_disable_actions(window, true);
}

void handle_install(window_t* window)
{
	install_window_priv_t* private = window->private;

	enable_disable_actions(window, false);

	lv_obj_t* bar = private->progress_bar;

	// TODO: dynamically detect number of chunks we write.
	lv_bar_set_range(bar, 0, 100);

	lv_label_set_text(
		private->progress_label,
		"Preparing for installation..."
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
			"Wrote %d out of %d bytes...",
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
		"Installation completed successfully.\n"
		"\nYou can safely power off the " DEVICE_NAME " and remove the installation media."
	);

	tbgui_theme_success();
	btn_enable_state(private->back_button, true);
}
