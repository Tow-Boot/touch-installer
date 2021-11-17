#ifndef TBGUI_PARTS_H
#define TBGUI_PARTS_H

#include "tbgui_actions.h"

struct window;
typedef struct window window_t;
struct window {
	lv_obj_t* container;
	lv_obj_t* scr;
	lv_obj_t* main_container;
	lv_obj_t* actions_container;
	lv_obj_t* top_container;

	struct window* back_location;

	void (*on_present)(window_t*);

	// Pointer to stash useful things for the window's own internal use.
	// Ugh... I don't like C :/
	void* private_use;
};

window_t* current_window;

lv_obj_t* add_button(lv_obj_t* parent, app_actions_t action, char* label_text);
lv_obj_t* add_container(lv_obj_t* parent, bool transp);
window_t* create_window(char* title_text);
void finalize_window(window_t* window);
void present_window(window_t* window);
void btn_enable_state(lv_obj_t* btn, bool enabled);
void enable_disable_actions(window_t* window, bool enabled);

/**
 * Declared here, defind by the app.
 */
void btn_event_cb(lv_obj_t * btn, lv_event_t event);


#endif
