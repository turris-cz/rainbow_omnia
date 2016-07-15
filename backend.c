/*
 * Rainbow is a tool for changing color and status of the LEDs of the Turris router
 *
 * Copyright (C) 2016 CZ.NIC, z.s.p.o. (http://www.nic.cz/)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "configuration.h"
#include "arg_parser.h"
#include "backend.h"
#include "util.c"

#define SYS_PATH "/sys/devices/platform/soc/soc:internal-regs/f1011000.i2c/i2c-0/i2c-1/1-002b"
#define LED_PREFIX "leds/omnia-led"

static char *led_map[] = {
	[CMD_PWR] = "power",
	[CMD_LAN0] = "lan0",
	[CMD_LAN1] = "lan1",
	[CMD_LAN2] = "lan2",
	[CMD_LAN3] = "lan3",
	[CMD_LAN4] = "lan4",
	[CMD_WAN] = "wan",
	[CMD_PCI1] = "pci1",
	[CMD_PCI2] = "pci2",
	[CMD_PCI3] = "pci3",
	[CMD_USR1] = "user1",
	[CMD_USR2] = "user2",
	[CMD_ALL] = "all"
};

static void backend_write(const char *path, const char *value)
{
	int fd = open(path, O_WRONLY);
	if (fd == -1) {
		fprintf(stderr, "Failed to open file: %s\n", strerror(errno));
		exit(3);
	}
	size_t len = strlen(value);
	while (len > 0) {
		int ret = write(fd, value, len);
		if (ret == -1) {
			if (errno == EINTR) {
				continue;
			} else {
				fprintf(stderr, "Write error: %s\n", strerror(errno));
				exit(3);
			}
		}

		value += ret;
		len -= ret;
	}

	close(fd);
}
static void backend_read(const char *path, char *buff, size_t len)
{
	int fd = open(path, O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "Failed to open file: %s\n", strerror(errno));
		exit(3);
	}
	while (len > 0) {
		int ret = read(fd, buff, len);
		if (ret == 0) {
			//EOF
			close(fd);
			return;

		} else if (ret == -1) {
			if (errno == EINTR) {
				continue;
			} else {
				fprintf(stderr, "Read error: %s\n", strerror(errno));
				exit(3);
			}
		}

		buff += ret;
		len -= ret;
	}

	close(fd);
}

static void get_rgb_parts(unsigned int color, unsigned char *r, unsigned char *g, unsigned char *b) {
	*r = ((color & 0xFF0000) >> 2*8);
	*g = ((color & 0x00FF00) >> 8);
	*b = (color & 0x0000FF);
}

void set_intensity(unsigned int level)
{
	const char *path = aprintf("%s/%s", SYS_PATH, "global_brightness");
	const char *value = aprintf("%u", level);
	backend_write(path, value);
}

int get_intensity()
{
	/*
	The maximum value of brightness/intensity is 100. Greater number (longer
	string) is invalid input. + 1 for '\0' at the end of the string.
	*/
	const size_t bufflen = 4;
	char buff[bufflen];
	int level;

	const char *path = aprintf("%s/%s", SYS_PATH, "global_brightness");
	backend_read(path, buff, bufflen);
	buff[bufflen - 1] = '\0'; // Just to make sure we have one

	int ret = sscanf(buff, "%d", &level);
	if (ret != 1) {
		fprintf(stderr, "Read error: %s\n", strerror(errno));
		exit(3);
	}

	return level;
}

void set_color(enum cmd cmd, unsigned int color)
{
	unsigned char r, g, b;
	get_rgb_parts(color, &r, &g, &b);

	const char *path = aprintf("%s/%s:%s/%s", SYS_PATH, LED_PREFIX, led_map[cmd], "color");
	const char *value = aprintf("%d %d %d", r, g, b);
	backend_write(path, value);
}

void set_status(enum cmd cmd, enum status status)
{
	const char *path_autonomous = aprintf("%s/%s:%s/%s", SYS_PATH, LED_PREFIX, led_map[cmd], "autonomous");
	const char *path_brightness = aprintf("%s/%s:%s/%s", SYS_PATH, LED_PREFIX, led_map[cmd], "brightness");

	if (status == ST_DISABLE) {
		backend_write(path_autonomous, "0");
		backend_write(path_brightness, "0");

	} else if (status == ST_ENABLE) {
		backend_write(path_autonomous, "0");
		backend_write(path_brightness, "255");

	} else if (status == ST_AUTO) {
		backend_write(path_autonomous, "1");
	}
}
