/*
 *   Copyright (C) 2015,2016,2020 by Jonathan Naylor G4KLX
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "Log.h"

#include <sys/time.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cassert>
#include <cstring>

static unsigned int m_level = 2U;

static char LEVELS[] = " DMIWEF";

void LogInitialise(unsigned int level)
{
	m_level    = level;
}

void Log(unsigned int level, const char* fmt, ...)
{
	assert(fmt != NULL);

	char buffer[501U];
	::sprintf(buffer, "%c: ", LEVELS[level]);

	va_list vl;
	va_start(vl, fmt);

	::vsnprintf(buffer + 3, 497, fmt, vl);

	va_end(vl);

	if (level >= m_level && m_level != 0U)
	{
		auto f = (level > 4) ? stderr : stdout;
		::fprintf(f, "%s\n", buffer);
		::fflush(f);
	}

	if (level == 6U)  		// Fatal
	{
		exit(1);
	}
}
