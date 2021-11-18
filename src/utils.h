#ifndef TBGUI_UTILS_H
#define TBGUI_UTILS_H

#define TBGUI_WRITE_BLOCK_SIZE 8*1024 // (0x2000 bytes)

typedef void (write_callback_function_t)(void* userdata, int bytes_written, int total_bytes);
int write_to_device(void* userdata, write_callback_function_t* cb, char* from_path, char* to_path, int limit);

#endif
