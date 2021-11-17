#ifndef TBGUI_APP_ACTIONS_H
#define TBGUI_APP_ACTIONS_H

enum app_actions {
	APP_ACTION_NONE,
	APP_ACTION_BACK,
	APP_ACTION_INSTALL,
	APP_ACTION_DO_INSTALL,
	APP_ACTION_ERASE,
	APP_ACTION_DO_ERASE,
	APP_ACTION_POWEROFF,
	_APP_ACTIONS_COUNT,
};
typedef enum app_actions app_actions_t;

void handle_app_actions();

#endif
