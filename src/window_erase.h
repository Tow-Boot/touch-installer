#ifndef TBGUI_WINDOW_ERASE_H
#define TBGUI_WINDOW_ERASE_H

window_t* tbgui_erase_window_init(void);
void tbgui_erase_window_on_present(window_t* window);
void handle_erase(window_t* window);

#endif
