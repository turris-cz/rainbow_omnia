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
#include <stdlib.h>
#include <string.h>

#include "arg_parser.h"

struct map {
	const char *kw;
	enum cmd cmd;
};

static struct map kw_cmd_map[] = {
	{KW_PWR, CMD_PWR},
	{KW_LAN0, CMD_LAN0},
	{KW_LAN1, CMD_LAN1},
	{KW_LAN2, CMD_LAN2},
	{KW_LAN3, CMD_LAN3},
	{KW_LAN4, CMD_LAN4},
	{KW_WAN, CMD_WAN},
	{KW_PCI1, CMD_PCI1},
	{KW_PCI2, CMD_PCI2},
	{KW_PCI3, CMD_PCI3},
	{KW_USR1, CMD_USR1},
	{KW_USR2, CMD_USR2},
	{KW_ALL, CMD_ALL},
	{KW_LAN, CMD_LAN},
	{KW_INTEN, CMD_INTEN},
	{KW_BINMASK, CMD_BINMASK},
	{KW_GET, CMD_GET},
	{NULL, CMD_UNDEF}
};

struct color {
	const char *name;
	int rgb;
};

static struct color colors[] = {
	{ "red",	0xFF0000},
	{ "green",	0x00FF00},
	{ "blue",	0x0000FF},
	{ "white",	0xFFFFFF},
	{ "black",	0x000000},
	{ NULL,	0}
};

static bool get_color_from_name(const char *color_name, unsigned int *color)
{
	for (size_t i = 0; colors[i].name != NULL; i++) {
		if (strcmp(color_name, colors[i].name) == 0) {
			*color = colors[i].rgb;
			return true;
		}
	}

	return false;
}

bool parse_cmd(const char *param, enum cmd *command)
{
	if (param == NULL) {
		return false;
	}

	for (size_t i = 0; kw_cmd_map[i].kw != NULL; i++) {
		if (strcmp(param, kw_cmd_map[i].kw) == 0) {
			*command = kw_cmd_map[i].cmd;
			return true;
		}
	}

	return false;
}

bool parse_color(const char *param, unsigned int *color)
{
	if (param == NULL) {
		return false;
	}

	//Is param pre-defined color?
	if (get_color_from_name(param, color)) {
		return true;
	}

	//Is param color in format AABBCC?
	if (strlen(param) == 6) {
		char *endptr = (char *)param;
		long int tmp_number = strtol(param, &endptr, 16);

		if (param == endptr) {
			//There wheren't any number
			return false;
		}

		*color = (unsigned int) tmp_number;
		return true;
	}

	//OK, it is not color
	return false;
}

bool parse_status(const char *param, enum status *status)
{
	if (param == NULL) {
		return false;
	}

	if (strcmp(param, KW_ENABLE) == 0) {
		*status = ST_ENABLE;
		return true;

	} else if (strcmp(param, KW_DISABLE) == 0) {
		*status = ST_DISABLE;
		return true;

	} else if (strcmp(param, KW_AUTO) == 0) {
		*status = ST_AUTO;
		return true;
	}

	return false;
}

bool parse_number(const char *param, unsigned int *number)
{
	if (param == NULL) {
		return false;
	}

	char *endptr = (char *)param;
	long int tmp_number = strtol(param, &endptr, 0);

	if (param == endptr) {
		//There weren't any number
		return false;
	}

	*number = tmp_number;

	return true;
}
