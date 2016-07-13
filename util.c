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

#include "util.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>

size_t vprintf_len(const char *msg, va_list args) {
	va_list cp;
	va_copy(cp, args); // Make a copy so we don't destroy/move the provided parameters
	size_t result = vsnprintf(NULL, 0, msg, cp);
	va_end(cp);
	return result + 1;
}

char *vprintf_into(char *dst, const char *msg, va_list args) {
	va_list cp;
	va_copy(cp, args); // Make a copy so we don't destroy/move the provided parameters
	vsprintf(dst, msg, cp);
	va_end(cp);
	return dst;
}

size_t printf_len(const char *msg, ...) {
	va_list args;
	va_start(args, msg);
	size_t result = vsnprintf(NULL, 0, msg, args);
	va_end(args);
	return result + 1;
}

char *printf_into(char *dst, const char *msg, ...) {
	va_list args;
	va_start(args, msg);
	vsprintf(dst, msg, args);
	va_end(args);
	return dst;
}
