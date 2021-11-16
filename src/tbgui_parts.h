#ifndef TBGUI_PARTS_H
#define TBGUI_PARTS_H

struct window;
typedef struct window {
	lv_obj_t* container;
	lv_obj_t* scr;
	lv_obj_t* main_container;
	lv_obj_t* actions_container;
	lv_obj_t* top_container;

	struct window* back_location;
} window_t;

enum app_actions {
	APP_ACTION_NONE,
	APP_ACTION_BACK,
	APP_ACTION_INSTALL,
	APP_ACTION_ERASE,
	APP_ACTION_POWEROFF,
	_APP_ACTIONS_COUNT,
};
typedef enum app_actions app_actions_t;

lv_obj_t* add_button(lv_obj_t* parent, app_actions_t action, char* label_text);
lv_obj_t* add_container(lv_obj_t* parent, bool transp);
window_t* create_window(char* title_text);
void finalize_window(window_t* window);


#endif
