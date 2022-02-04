#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <linux/fs.h>

#include <unistd.h>
#include "conf.h"
#include "utils.h"

int write_to_device(void* userdata, write_callback_function_t* cb, char* from_path, char* to_path, int start, int length)
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
		close(from);
		return errno;
	}

	if (length == 0) {
		length = lseek(from, 0, SEEK_END);
	}

	lseek(from, start, SEEK_SET);
	lseek(to,   start, SEEK_SET);

	int i = start;
	int count = 0;
	int ret = 0;

	while (i < length) {
		count = read(from, buf, TBGUI_WRITE_BLOCK_SIZE);

		if (count < 0) {
			fprintf(stderr, "ERROR: reading from “%s” failed, (%m)\n", from_path);
			close(from);
			close(to);
			return errno;
		}

		ret = write(to, buf, count);

		if (ret < 0) {
			fprintf(stderr, "ERROR: writing to “%s” failed, (%m)\n", to_path);
			close(from);
			close(to);
			return errno;
		}

		i += count;

		cb(userdata, i, length);
	}

	close(from);
	close(to);
	return 0;
}

uint64_t get_block_device_size(char* path)
{
	uint64_t size;

	int f = open(path, O_RDONLY);
	if (f == -1) {
		fprintf(stderr, "ERROR: Opening “%s” failed, (%m)\n", path);
		exit(2);
	}

	if (ioctl(f, BLKGETSIZE64, &size) == -1) {
		fprintf(stderr, "ERROR: Running ioctl against “%s” failed, (%m)\n", path);
		close(f);
		exit(2);
	}

	close(f);

	return size;
}
