#pragma once
/*
 *   Copyright (C) 2020 by Thomas Early N7TAE
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

#include <stdio.h>
#include <sqlite3.h>
#include <string>
#include <list>

class CDashDB
{
public:
	CDashDB() : db(NULL) {}
	~CDashDB() { if (db) sqlite3_close(db); }
	bool Open(const char *name);
	bool UpdateLH(const char *callsign, uint32_t slot, const char *tg);
	bool UpdateLH(const char *callsign, const char *status);
	int Count(const char *table);
	void ClearLH();

private:
	bool Init();
	sqlite3 *db;
};
