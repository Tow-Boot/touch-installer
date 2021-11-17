#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "conf.h"
#include "app_actions.h"
#include "app_state.h"
#include "tbgui_parts.h"

#include "windows.h"

void handle_app_actions()
{
	if (app->action != APP_ACTION_NONE) {
		switch (app->action) {
			case APP_ACTION_NONE:
				break;
			case APP_ACTION_BACK:
				present_window(current_window->back_location);
				break;
			case APP_ACTION_INSTALL:
				present_window(app->install_window);
				break;
			case APP_ACTION_ERASE:
				present_window(app->erase_window);
				break;
			case APP_ACTION_POWEROFF:
#ifdef LVGL_ENV_SIMULATOR
				exit(0);
#else
				int res = 0;
				res = system("poweroff");
				(void)res;
#endif
				break;
			case APP_ACTION_DO_ERASE:
				handle_erase(app->erase_window);
				break;
			case APP_ACTION_DO_INSTALL:
				handle_install(app->install_window);
				break;
			case _APP_ACTIONS_COUNT:
			default:
				printf("Unhandled action: [%d]\n", app->action);
		}
		app->action = APP_ACTION_NONE;
	}
}
