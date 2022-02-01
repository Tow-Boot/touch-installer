#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/limits.h>

#include "conf.h"
#include <hal.h>
#include "tbgui_parts.h"
#include "app_state.h"
#include "window_install.h"
#include "utils.h"

#define LOG_BUF_SIZE (1024 * 1024)
#define BAR_SEGMENTS 1024

void tbgui_install_window_on_present(window_t* window);

#if LVGL_ENV_SIMULATOR == 1
static int fake_failure = 0;
#endif

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
	lv_bar_set_range(bar, 0, BAR_SEGMENTS);
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
	lv_label_set_text(
		private->progress_label,
		"Connect your device to a charger, just in case."
		"\n\n"
		"Use the “Start installation” button to start the installation."
	);
	enable_disable_actions(window, true);
}

static void write_data_callback(void* window, int bytes_written, int total_bytes)
{
	install_window_priv_t* private = ((window_t*)window)->private;
	uint64_t progress = bytes_written;
	progress *= BAR_SEGMENTS;
	progress /= total_bytes;

	lv_bar_set_value(
		private->progress_bar,
		progress,
		LV_ANIM_OFF
	);
	lv_label_set_text_fmt(
		private->progress_label,
		"Bytes written:\n%d/%d",
		bytes_written,
		total_bytes
	);
	lv_task_handler();
}

void handle_install(window_t* window)
{
	install_window_priv_t* private = window->private;

	enable_disable_actions(window, false);

	lv_label_set_text(
		private->progress_label,
		"Preparing for installation..."
	);
	usleep(SECOND_AS_MICROSECONDS * FRAME_RATE);
	lv_task_handler();

	int ret = 0;
#if LVGL_ENV_SIMULATOR == 1
	if (fake_failure == 1) {
		ret = system("echo 'Fake installer checks fail for simulator' > " TBGUI_CHECK_LOG_LOCATION "; sleep 1; false");
		fake_failure = 0;
	}
	else {
		ret = system("echo 'Fake success for simulator' > " TBGUI_CHECK_LOG_LOCATION "; sleep 1; true");
		fake_failure = 1;
	}
#else
	ret = system("tow-boot-installer--install-checks");
#endif

	if (ret) {
		char* log = calloc(LOG_BUF_SIZE, sizeof(char));
		int res = 0;

		FILE * f = fopen(TBGUI_CHECK_LOG_LOCATION, "r");
		// Read exactly one LOG_BUF_SIZE buffer out of the file.
		// It's not expected to be lengthy anyway.
		res = fread(log, 1, LOG_BUF_SIZE, f);
		(void)res; // unused
		fclose(f);

		lv_label_set_text_fmt(
			private->progress_label,
			"Checks failed... [ret = %d]\nNothing was done.\n\nDetails:\n%s",
			ret >> 8,
			log
		);

		free(log);

		tbgui_theme_failure();
		btn_enable_state(private->back_button, true);

		return;
	}

	ret = write_to_device(window, write_data_callback, TOW_BOOT_SOURCE_FILE, TARGET_BLOCK_DEVICE, 0);
	if (ret != 0) {
		lv_label_set_text_fmt(
			private->progress_label,
			"Unexpected failure! [ret = %d]\nSomething could have happened...\n\nDetails:\n%s",
			ret,
			strerror(ret)
		);

		tbgui_theme_failure();
		btn_enable_state(private->back_button, true);

		return;
	}

	lv_label_set_text(
		private->progress_label,
		"Installation completed successfully.\n"
		"\nYou can safely power off the " DEVICE_NAME " and remove the installation media."
	);

	tbgui_theme_success();
	btn_enable_state(private->back_button, true);
}
