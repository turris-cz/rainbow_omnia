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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <sys/ioctl.h>
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
		"  Show this help: rainbow --help or -h\n"
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
		"  VALUE is 'intensity' (no more getters are available for now)\n"
		"\n"
		"Examples:\n"
		"rainbow all blue auto - reset status of all LEDs and set their color to blue\n"
		"rainbow all blue pwr red - set color of all LEDs to blue except the Power one\n"
		"rainbow all enable wan auto - all LEDs will be shining except the LED of WAN port\n"
		"                              that will flash according to traffic\n"



	);
}

static void meta_set_color(enum cmd cmd, unsigned int color)
{
	switch (cmd) {
	case CMD_LAN:
		set_color(CMD_LAN0, color);
		set_color(CMD_LAN1, color);
		set_color(CMD_LAN2, color);
		set_color(CMD_LAN3, color);
		set_color(CMD_LAN4, color);
		break;
	case CMD_UNDEF:
	case CMD_INTEN:
	case CMD_BINMASK:
	case CMD_GET:
		assert(NULL);
		break;
	default:
		set_color(cmd, color);
	}
}

static void meta_set_status(enum cmd cmd, enum status status)
{
	switch (cmd) {
	case CMD_LAN:
		set_status(CMD_LAN0, status);
		set_status(CMD_LAN1, status);
		set_status(CMD_LAN2, status);
		set_status(CMD_LAN3, status);
		set_status(CMD_LAN4, status);
		break;
	case CMD_UNDEF:
	case CMD_INTEN:
	case CMD_BINMASK:
	case CMD_GET:
		assert(NULL);
		break;
	default:
		set_status(cmd, status);
	}
}

static void binmask_set(unsigned mask, unsigned position, enum cmd cmd)
{
	if (mask & position) {
		set_status(cmd, ST_ENABLE);
	} else {
		set_status(cmd, ST_DISABLE);
	}
}

static void binmask(unsigned int mask)
{
	binmask_set(mask, 0x800, CMD_PWR);
	binmask_set(mask, 0x400, CMD_LAN0);
	binmask_set(mask, 0x200, CMD_LAN1);
	binmask_set(mask, 0x100, CMD_LAN2);
	binmask_set(mask, 0x080, CMD_LAN3);
	binmask_set(mask, 0x040, CMD_LAN4);
	binmask_set(mask, 0x020, CMD_WAN);
	binmask_set(mask, 0x010, CMD_PCI1);
	binmask_set(mask, 0x008, CMD_PCI2);
	binmask_set(mask, 0x004, CMD_PCI3);
	binmask_set(mask, 0x002, CMD_USR1);
	binmask_set(mask, 0x001, CMD_USR2);
}

struct cleanup_data {
	int fd;
	struct tokenizer *tokenizer;
};

static struct cleanup_data cleanup = {
	.tokenizer = NULL
};

static void cleanup_atexit()
{
	if (cleanup.tokenizer) {
		tokenizer_destroy(cleanup.tokenizer);
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

	struct tokenizer *tokenizer = tokenizer_init(argv, optind);
	if (!tokenizer) {
		fprintf(stderr, "Memory allocation error\n");
		exit(2);
	}

	// Now I have FD - prepare cleanup
	cleanup.tokenizer = tokenizer;
	atexit(cleanup_atexit);

	enum cmd current_cmd = CMD_UNDEF;
	bool eof = false;

	while (!eof) {
		struct token token = next_token(tokenizer);

		switch (token.type) {
		case TOK_UNDEF:
			fprintf(stderr, "Undefined sequence: %s is some garbage\n", token.raw);
			return 1;
		case TOK_CMD:
			switch (token.data.cmd) {
			case CMD_GET:
				token = next_token(tokenizer);
				if (token.type != TOK_CMD) {
					fprintf(stderr, "Specify item for get command\n");
					return 1;
				}
				if (token.data.cmd == CMD_INTEN) {
					printf("%d\n", get_intensity());
				} else {
					fprintf(stderr, "Unknown getter\n");
					return 1;
				}
				break;
			case CMD_INTEN:
				token = next_token(tokenizer);
				if (token.type != TOK_NUMBER) {
					fprintf(stderr, "Specify intensity level\n");
					return 1;
				}
				if (token.data.number <= MAX_INTENSITY_LEVEL) {
					set_intensity(token.data.number);
				} else {
					fprintf(stderr, "Intensity is out of range [0-100]\n");
					return 1;
				}
				break;
			case CMD_BINMASK:
				token = next_token(tokenizer);
				if (token.type != TOK_NUMBER) {
					fprintf(stderr, "Specify binary mask\n");
					return 1;
				}
				if (token.data.number <= MAX_BINMASK_VALUE) {
					binmask(token.data.number);
				} else {
					fprintf(stderr, "Number is out of range [0-0xFFF]\n");
					return 1;
				}
				break;
			case CMD_UNDEF:
				fprintf(stderr, "Undefined command\n");
				return 1;
			default: // The rest of command is some real device
				current_cmd = token.data.cmd;
				break;
			}
			break;
		case TOK_NUMBER:
			fprintf(stderr, "Unexpected value: %s\n", token.raw);
			return 1;
		case TOK_COLOR:
			if (current_cmd == CMD_UNDEF) {
				fprintf(stderr, "Trying to configure undefined device\n");
				return 1;
			}
			meta_set_color(current_cmd, token.data.color);
			break;

		case TOK_STATUS:
			if (current_cmd == CMD_UNDEF) {
				fprintf(stderr, "Trying to configure undefined device\n");
				return 1;
			}
			meta_set_status(current_cmd, token.data.status);
			break;

		case TOK_EOF:
			eof = true;
			break;
		}

	}

	return 0;
}
