#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include "conf.h"
#include "utils.h"

int write_to_device(void* userdata, write_callback_function_t* cb, char* from_path, char* to_path, int length)
{
	static char buf[TBGUI_WRITE_BLOCK_SIZE] __attribute__ ((__aligned__ (TBGUI_WRITE_BLOCK_SIZE)));

	int from = open(from_path, O_RDONLY);
	if (from == -1) {
		fprintf(stderr, "ERROR: Opening “%s” failed, (%m)\n", from_path);
		return errno;
	}

	int to = open(to_path, O_SYNC | O_WRONLY);
	if (to == -1) {
		fprintf(stderr, "ERROR: Opening “%s” failed, (%m)\n", to_path);
		return errno;
	}

	if (length == 0) {
		length = lseek(from, 0, SEEK_END);
	}

	lseek(from, 0, SEEK_SET);

	int i = 0;
	int count = 0;
	int ret = 0;

	while (i < length) {
		count = read(from, buf, TBGUI_WRITE_BLOCK_SIZE);

		if (count < 0) {
			fprintf(stderr, "ERROR: reading from “%s” failed, (%m)\n", from_path);
			return errno;
		}

		ret = write(to, buf, count);

		if (ret < 0) {
			fprintf(stderr, "ERROR: writing to “%s” failed, (%m)\n", to_path);
			return errno;
		}

		i += count;

		cb(userdata, i, length);
	}

	return 0;
}
