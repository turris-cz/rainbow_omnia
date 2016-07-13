/*
 * Rainbow is a tool for changing color and status of the LEDs of the Turris router
 *
 * Copyright 2016, CZ.NIC z.s.p.o. (http://www.nic.cz/)
 *
 * Updater is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 * Updater is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Updater.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <alloca.h>

size_t vprintf_len(const char *msg, va_list args);
char *vprintf_into(char *dst, const char *msg, va_list args);
size_t printf_len(const char *msg, ...) __attribute__((format(printf, 1, 2)));
char *printf_into(char *dst, const char *msg, ...) __attribute__((format(printf, 2, 3)));
#define vaprintf(MSG, VA_ARGS) vprintf_into(alloca(vprintf_len((MSG), (VA_ARGS))), (MSG), (VA_ARGS))
#define aprintf(...) printf_into(alloca(printf_len(__VA_ARGS__)), __VA_ARGS__)

#endif
