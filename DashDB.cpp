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

#include <string>
#include <thread>

#include "DashDB.h"

bool CDashDB::Open(const char *name)
{
	if (sqlite3_open_v2(name, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX, NULL))
	{
		fprintf(stderr, "CDashDB::Open: can't open %s\n", name);
		return true;
	}
	auto rval = sqlite3_busy_timeout(db, 1000);
	if (SQLITE_OK != rval)
	{
		fprintf(stderr, "sqlite3_busy_timeout returned %d\n", rval);
	}

	return Init();
}

bool CDashDB::Init()
{
	char *eMsg;

	std::string sql("CREATE TABLE IF NOT EXISTS LHEARD("
					"callsign	TEXT PRIMARY KEY, "
					"slot       INT  DEFAULT 0, "
					"talkgroup  TEXT DEFAULT '0', "
					"status     TEXT, "
					"lasttime	INT NOT NULL"
					") WITHOUT ROWID;");

	if (SQLITE_OK != sqlite3_exec(db, sql.c_str(), NULL, 0, &eMsg))
	{
		fprintf(stderr, "CDashDB::Init [%s] error: %s\n", sql.c_str(), eMsg);
		sqlite3_free(eMsg);
		return true;
	}
	return false;
}

static int countcallback(void *count, int /*argc*/, char **argv, char ** /*azColName*/)
{
	auto c = (int *)count;
	*c = atoi(argv[0]);
	return 0;
}

bool CDashDB::UpdateLH(const char *callsign, uint32_t slot, const char *tg)
{
	if (NULL == db)
		return false;
	std::string sql("SELECT COUNT(*) FROM LHEARD WHERE callsign='");
	sql.append(callsign);
	sql.append("';");

	int count = 0;

	char *eMsg;
	auto rval = sqlite3_exec(db, sql.c_str(), countcallback, &count, &eMsg);
	if (SQLITE_OK == rval)
	{
		if (count)
		{
			sql.assign("UPDATE LHEARD SET (slot,talkgroup,status,lasttime) = (");
			sql.append(std::to_string(slot));
			sql.append(",'");
			sql.append(tg);
			sql.append("','Receiving...',");
			sql.append("strftime('%s','now')) WHERE callsign='");
			sql.append(callsign);
			sql.append("';");
		}
		else
		{
			sql.assign("INSERT INTO LHEARD (callsign,slot,talkgroup,status,lasttime) VALUES ('");
			sql.append(callsign);
			sql.append("',");
			sql.append(std::to_string(slot));
			sql.append(",'");
			sql.append(tg);
			sql.append("','Receiving...',");
			sql.append("strftime('%s','now'));");
		}
		rval = sqlite3_exec(db, sql.c_str(), NULL, 0, &eMsg);
	}

	if (SQLITE_OK != rval) {
		fprintf(stderr, "CDashDB::UpdateLH [%s] error: %s\n", sql.c_str(), eMsg);
		sqlite3_free(eMsg);
		return true;
	}

	return false;
}

bool CDashDB::UpdateLH(const char *callsign, const char *status)
{
	if (NULL == db)
		return false;
	std::string sql("SELECT COUNT(*) FROM LHEARD WHERE callsign='");
	sql.append(callsign);
	sql.append("';");

	int count = 0;

	char *eMsg;
	auto rval = sqlite3_exec(db, sql.c_str(), countcallback, &count, &eMsg);
	if (SQLITE_OK == rval)
	{
		if (count)
		{
			sql.assign("UPDATE LHEARD SET (status,lasttime) = ('");
			sql.append(status);
			sql.append("',");
			sql.append("strftime('%s','now')) WHERE callsign='");
			sql.append(callsign);
			sql.append("';");
			rval = sqlite3_exec(db, sql.c_str(), NULL, 0, &eMsg);
		}
	}

	if (SQLITE_OK != rval) {
		fprintf(stderr, "CDashDB::UpdateLH [%s] error: %s\n", sql.c_str(), eMsg);
		sqlite3_free(eMsg);
		return true;
	}

	return false;
}

void CDashDB::ClearLH()
{
	if (NULL == db)
		return;

	char *eMsg;

	if (SQLITE_OK != sqlite3_exec(db, "DELETE FROM LHEARD;", NULL, 0, &eMsg))
	{
		fprintf(stderr, "CDashDB::ClearLH error: %s\n", eMsg);
		sqlite3_free(eMsg);
	}
}

int CDashDB::Count(const char *table)
{
	if (NULL == db)
		return 0;

	std::string sql("SELECT COUNT(*) FROM ");
	sql.append(table);
	sql.append(";");

	int count = 0;

	char *eMsg;
	if (SQLITE_OK != sqlite3_exec(db, sql.c_str(), countcallback, &count, &eMsg))
	{
		fprintf(stderr, "CDashDB::Count error: %s\n", eMsg);
		sqlite3_free(eMsg);
	}

	return count;
}
