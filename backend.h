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

#ifndef BACKEND_H
#define BACKEND_H

#include <stdbool.h>

#include "arg_parser.h"

void set_intensity(int fd, unsigned int level);
void set_color(int fd, enum cmd cmd, unsigned int color);
void set_status(int fd, enum cmd cmd, enum status status);
void get_intensity(int fd, int *level);

#endif //BACKEND_H
