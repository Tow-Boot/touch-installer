#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/limits.h>

#include "conf.h"
#include "tbgui_parts.h"
#include "app_state.h"
#include "window_erase.h"
#include "utils.h"

#define LOG_BUF_SIZE (1024 * 1024)
#define BAR_SEGMENTS 1024

void tbgui_erase_window_on_present(window_t* window);

#ifdef LVGL_ENV_SIMULATOR
static int fake_failure = 0;
#endif

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

static void write_data_callback(void* window, int bytes_written, int total_bytes)
{
	erase_window_priv_t* private = ((window_t*)window)->private;
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
		"Bytes erased:\n%d/%d",
		bytes_written,
		total_bytes
	);
	lv_task_handler();
}

void handle_erase(window_t* window)
{
	erase_window_priv_t* private = window->private;

	enable_disable_actions(window, false);

	lv_label_set_text(
		private->progress_label,
		"Preparing for erasing..."
	);
	usleep(SECOND_AS_MICROSECONDS * FRAME_RATE);
	lv_task_handler();

	int ret = 0;
#ifdef LVGL_ENV_SIMULATOR
	if (fake_failure == 1) {
		ret = system("echo 'Fake installer checks fail for simulator' > " TBGUI_CHECK_LOG_LOCATION "; sleep 1; false");
		fake_failure = 0;
	}
	else {
		ret = system("echo 'Fake success for simulator' > " TBGUI_CHECK_LOG_LOCATION "; sleep 1; true");
		fake_failure = 1;
	}
#else
	ret = system("tow-boot-installer--erase-checks");
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

#ifdef LVGL_ENV_SIMULATOR
	uint64_t length = 16 * 1024 * 1024;
#else
	uint64_t length = get_block_device_size(TARGET_BLOCK_DEVICE);
#endif

	write_to_device(window, write_data_callback, "/dev/zero", TARGET_BLOCK_DEVICE, length);

	lv_label_set_text(
		private->progress_label,
		"The " INSTALL_LOCATION " was erased successfully.\n"
		"\nYou can now proceed with the installation on your " DEVICE_NAME "\n"
		"\nAlternatively you can safely power off the " DEVICE_NAME " and remove the installation media."
	);

	tbgui_theme_success();
	btn_enable_state(private->back_button, true);
}
