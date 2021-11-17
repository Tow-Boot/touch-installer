#ifndef TBGUI_APP_STATE_H
#define TBGUI_APP_STATE_H

#include "tbgui_actions.h"

typedef struct app_state {
	app_actions_t action;
	struct window* main_window;
	struct window* install_window;
	struct window* erase_window;
} app_state_t;

extern app_state_t* app;

#endif
