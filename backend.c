/*
 * Rainbow is tool for changing color and status of LEDs of the Turris router
 * Rainbow daemon provides indication of WiFi status by controlling its LED
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
#include <linux/i2c-dev.h>

#include "configuration.h"
#include "arg_parser.h"
#include "backend.h"

#define REG_MODE 3
#define REG_STATE 4
#define REG_BRIGHTNESS 7

static char led_map[] = {
	11, // PWR
	10, // LAN0
	9, // LAN1
	8, // LAN2
	7, // LAN3
	6, // LAN4
	5, // WAN
	4, // PCI1
	3, // PCI2
	2, // PCI3
	1, // USR1
	0, // USR2
	12 // ALL
};

static void get_rgb_parts(unsigned int color, unsigned char *r, unsigned char *g, unsigned char *b) {
	*r = ((color & 0xFF0000) >> 2*8);
	*g = ((color & 0x00FF00) >> 8);
	*b = (color & 0x0000FF);
}

void set_intensity(int fd, unsigned int level)
{
	i2c_smbus_write_word_data(fd, REG_BRIGHTNESS, level);
}

void set_color(int fd, enum cmd cmd, unsigned int color)
{
	unsigned char r, g, b;
	get_rgb_parts(color, &r, &g, &b);

	//TODO

}

void set_status(int fd, enum cmd cmd, enum status status)
{
	if (status == ST_DISABLE) {
		i2c_smbus_write_byte_data(fd, REG_MODE, led_map[cmd] | (1 << 4));
		i2c_smbus_write_byte_data(fd, REG_STATE, led_map[cmd] | (0 << 4));

	} else if (status == ST_ENABLE) {
		i2c_smbus_write_byte_data(fd, REG_MODE, led_map[cmd] | (1 << 4));
		i2c_smbus_write_byte_data(fd, REG_STATE, led_map[cmd] | (1 << 4));

	} else if (status == ST_AUTO) {
		i2c_smbus_write_byte_data(fd, REG_MODE, led_map[cmd] | (0 << 4));
	}
}
