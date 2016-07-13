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
	CMD_PWR,
	CMD_LAN0,
	CMD_LAN1,
	CMD_LAN2,
	CMD_LAN3,
	CMD_LAN4,
	CMD_WAN,
	CMD_PCI1,
	CMD_PCI2,
	CMD_PCI3,
	CMD_USR1,
	CMD_USR2,
	CMD_ALL,
	CMD_LAN,
	CMD_INTEN,
	CMD_BINMASK,
	CMD_GET
};

enum token_type {
	TOK_UNDEF = -1,
	TOK_CMD,
	TOK_NUMBER,
	TOK_COLOR,
	TOK_STATUS,
	TOK_EOF
};

struct token {
	enum token_type type;
	union {
		enum cmd cmd;
		unsigned int number;
		unsigned int color;
		enum status status;
	} data;
	const char *raw;
};

struct tokenizer;

struct token next_token(struct tokenizer *tokenizer);
struct tokenizer *tokenizer_init(char **argv, int from);
void tokenizer_destroy(struct tokenizer *tokenizer);

#endif //ARG_PARSER_H
