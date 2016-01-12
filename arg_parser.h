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

#ifndef ARG_PARSER_H
#define ARG_PARSER_H

#include <stdbool.h>

// Single
#define KW_PWR		"pwr"
#define KW_LAN0		"lan0"
#define KW_LAN1		"lan1"
#define KW_LAN2		"lan2"
#define KW_LAN3		"lan3"
#define KW_LAN4		"lan4"
#define KW_WAN		"wan"
#define KW_PCI1		"pci1"
#define KW_PCI2		"pci2"
#define KW_PCI3		"pci3"
#define KW_USR1		"usr1"
#define KW_USR2		"usr2"

// Groups
#define KW_ALL		"all"
#define KW_LAN		"lan"

// Commands
#define KW_INTEN	"intensity"
#define KW_BINMASK	"binmask"
#define KW_ENABLE	"enable"
#define KW_DISABLE	"disable"
#define KW_AUTO		"auto"
#define KW_GET		"get"

enum status {
	ST_DISABLE = 0,
	ST_ENABLE = 1,
	ST_AUTO = 2
};

enum cmd {
	CMD_UNDEF = -1,
// Do NOT change order below!!
	CMD_PWR = 0,
	CMD_LAN0 = 1,
	CMD_LAN1 = 2,
	CMD_LAN2 = 3,
	CMD_LAN3 = 4,
	CMD_LAN4 = 5,
	CMD_WAN = 6,
	CMD_PCI1 = 7,
	CMD_PCI2 = 8,
	CMD_PCI3 = 9,
	CMD_USR1 = 10,
	CMD_USR2 = 11,
	CMD_ALL = 12,
// Do NOT change order above!!
	CMD_LAN = 13,
	CMD_INTEN = 14,
	CMD_BINMASK = 15,
	CMD_GET = 16
};

bool parse_cmd(const char *param, enum cmd *command);
bool parse_color(const char *param, unsigned int *color);
bool parse_status(const char *param, enum status *status);
bool parse_number(const char *param, unsigned int *number);

#endif //ARG_PARSER_H
