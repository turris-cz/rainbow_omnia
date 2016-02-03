/*
 * Rainbow is tool for changing color and status of LEDs of the Turris router
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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <getopt.h>
#include <assert.h>

#include "configuration.h"
#include "arg_parser.h"
#include "backend.h"

static struct option long_options[] = {
	{"help", no_argument, 0, 'h'},
	{0, 0, 0, 0}
};

void help() {
	fprintf(stdout,
		"Usage:\n"
		"  Show this help: rainbow --help of -h\n"
		"  Set devices: rainbow DEV_CONFIGURATION [DEV_CONFIGURATION ...]\n"
		"\n"
		"DEV_CONFIGURATION is one of the next options:\n"
		"DEV COLOR STATUS or DEV STATUS COLOR or DEV STATUS or DEV COLOR, where:\n"
		"  DEV: 'pwr' (LED of Power signalization),\n"
		"       'lan0', 'lan1', 'lan2', 'lan3', 'lan4' (one of the LAN LED),\n"
		"       'wan' (LED of WAN port),\n"
		"       'pci1', 'pci2', 'pci3' (one of the PCI LED),\n"
		"       'usr1', 'usr2' (one of the custom USER's LED),\n"
		"       or alias 'all' for all previous devices,\n"
		"                'lan' for all LAN ports\n"
		"  COLOR: name of predefined color (red, blue, green, white, black)\n"
		"         or 3 bytes for RGB, so red is 'FF0000', green '00FF00'\n"
		"         blue '0000FF' etc.\n"
		"  STATUS: 'enable' (device is shining), 'disable' (device is off)\n"
		"          'auto' (device is operated by HW - typically flashing)\n"
		"\n"
		"'intensity' NUMBER, where:\n"
		"  NUMBER is number from 0 to 100 (percent of maximum brightness).\n"
		"\n"
		"'binmask' NUMBER:\n"
		"  Use binary representation of NUMBER as mask to set ENABLE/DISABLE\n"
		"  status of LEDs. MSB is PWR LED and LSB is USR2. Max value is 4095 or 0xFFF.\n"
		"\n"
		"'get' VALUE, where:\n"
		"  VALUE is 'intensity' (no more getters aren't available for now)\n"
		"\n"
		"Examples:\n"
		"rainbow all blue auto - reset status of all LEDs and set their color to blue\n"
		"rainbow all blue pwr red - set color of all LEDs to blue except the Power one\n"
		"rainbow all enable wan auto - all LEDs will shining except the LED of WAN port\n"
		"                              that will flashing according to traffic\n"



	);
}

static void meta_set_color(int fd, enum cmd cmd, unsigned int color)
{
	switch (cmd) {
	case CMD_LAN:
		set_color(fd, CMD_LAN0, color);
		set_color(fd, CMD_LAN1, color);
		set_color(fd, CMD_LAN2, color);
		set_color(fd, CMD_LAN3, color);
		set_color(fd, CMD_LAN4, color);
		break;
	case CMD_UNDEF:
	case CMD_INTEN:
	case CMD_BINMASK:
	case CMD_GET:
		assert(NULL);
		break;
	default:
		set_color(fd, cmd, color);
	}
}

static void meta_set_status(int fd, enum cmd cmd, enum status status)
{
	switch (cmd) {
	case CMD_LAN:
		set_status(fd, CMD_LAN0, status);
		set_status(fd, CMD_LAN1, status);
		set_status(fd, CMD_LAN2, status);
		set_status(fd, CMD_LAN3, status);
		set_status(fd, CMD_LAN4, status);
		break;
	case CMD_UNDEF:
	case CMD_INTEN:
	case CMD_BINMASK:
	case CMD_GET:
		assert(NULL);
		break;
	default:
		set_status(fd, cmd, status);
	}
}

static void binmask_set(int fd, unsigned mask, unsigned position, enum cmd cmd)
{
	if (mask & position) {
		set_status(fd, cmd, ST_ENABLE);
	} else {
		set_status(fd, cmd, ST_DISABLE);
	}
}

static void binmask(int fd, unsigned int mask)
{
	binmask_set(fd, mask, 0x800, CMD_PWR);
	binmask_set(fd, mask, 0x400, CMD_LAN0);
	binmask_set(fd, mask, 0x200, CMD_LAN1);
	binmask_set(fd, mask, 0x100, CMD_LAN2);
	binmask_set(fd, mask, 0x080, CMD_LAN3);
	binmask_set(fd, mask, 0x040, CMD_LAN4);
	binmask_set(fd, mask, 0x020, CMD_WAN);
	binmask_set(fd, mask, 0x010, CMD_PCI1);
	binmask_set(fd, mask, 0x008, CMD_PCI2);
	binmask_set(fd, mask, 0x004, CMD_PCI3);
	binmask_set(fd, mask, 0x002, CMD_USR1);
	binmask_set(fd, mask, 0x001, CMD_USR2);
}

struct cleanup_data {
	int fd;
};

static struct cleanup_data cleanup;

static void cleanup_on_exit(int status, void *arg)
{
	struct cleanup_data *data = (struct cleanup_data *) arg;

	// For better readability: in case of normal exit keep close() in main()
	if (status != 0) {
		close(data->fd);
	}
}

int main(int argc, char **argv) {
	if (argc <= 1) {
		help();
		return 1;
	}

	//Parse options
	int c; //returned char

	while ((c = getopt_long(argc, argv, "h", long_options, NULL)) != -1) {
		switch (c) {
			case 'h':
				help();
				return 0;
				break;
		}
	}

	//Open memory raw device
	int devfd = open(I2C_BUS, O_RDWR);
	if (devfd < 0) {
		fprintf(stderr, "Cannot open I2C bus %s\n", I2C_BUS);
		return 2;
	}

	if (ioctl(devfd, I2C_SLAVE, I2C_ADDR) < 0) {
		fprintf(stderr, "Failed to bind slave address to 0x%02X\n", I2C_ADDR);
		return 2;
	}

	// Now I have FD - prepare cleanup
	cleanup.fd = devfd;
	on_exit(cleanup_on_exit, &cleanup);

	int i = optind;
	enum cmd current_cmd = CMD_UNDEF;
	enum status status;
	unsigned int color = 0;
	unsigned int number = 0;

	while (argv[i] != NULL) {
		if (parse_cmd(argv[i], &current_cmd)) {
			// This is placeholder for "get intensity" feature

		} else if (parse_status(argv[i], &status)) {
			//This part has to be before color parser because "enable" is valid color in color parser
			meta_set_status(devfd, current_cmd, status);

		} else if (parse_color(argv[i], &color)) {
			meta_set_color(devfd, current_cmd, color);

		} else if (parse_number(argv[i], &number)) {
			if (current_cmd == CMD_INTEN) {
				if (number <= MAX_INTENSITY_LEVEL) {
					set_intensity(devfd, number);
				}
			} else if (current_cmd == CMD_BINMASK) {
				if (number <= MAX_BINMASK_VALUE) {
					binmask(devfd, number);
				}
			} else {
				fprintf(stderr,
					"Parse error - unspecified command for number.\n"
					"Use rainbow -h for help.\n");
				exit(1);
				break;
			}
		} else {
			fprintf(stderr,
				"Parse error.\n"
				"Use rainbow -h for help.\n");
			exit(1);
			break;
		}
		i++;
	}

	close(devfd);

	return 0;
}
