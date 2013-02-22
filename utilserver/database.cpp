/*
	EQEMu:  Everquest Server Emulator

	Copyright (C) 2001-2008 EQEMu Development Team (http://eqemulator.net)

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; version 2 of the License.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY except by those people which sell it, which
	are required to give you total support for your newly bought product;
	without even the implied warranty of MERCHANTABILITY or FITNESS FOR
	A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/


#include "../common/debug.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errmsg.h>
#include <mysqld_error.h>
#include <limits.h>
#include <ctype.h>
#include <assert.h>
#include <map>

// Disgrace: for windows compile
#ifdef _WINDOWS
#include <windows.h>
#define snprintf	_snprintf
#define strncasecmp	_strnicmp
#define strcasecmp	_stricmp
#else
#include "../common/unix.h"
#include <netinet/in.h>
#endif

#include "database.h"
#include "../common/eq_packet_structs.h"
#include "../common/MiscFunctions.h"
#include "../common/servertalk.h"

Database::Database()
{
	DBInitVars();
}

Database::~Database()
{	
}

// Establish a connection to a mysql database with the supplied parameters
Database::Database(const char* host, const char* user, const char* password, const char* database, uint32 port)
{
	DBInitVars();
	Connect(host, user, password, database, port);
}

bool Database::Connect(const char* host, const char* user, const char* password, const char* database, uint32 port)
{
	uint32 errnum = 0;
	char errbuf[MYSQL_ERRMSG_SIZE];

	if (!Open(host, user, password, database, port, &errnum, errbuf))
	{
		LogFile->write(EQEMuLog::Error, "Failed to connect to database: Error: %s", errbuf);
		HandleMysqlError(errnum);

		return false;
	}
	else
	{
		LogFile->write(EQEMuLog::Status, "Using database '%s' at %s:%d", database, host, port);
		return true;
	}
}

void Database::DBInitVars()
{
}

void Database::HandleMysqlError(uint32 errnum)
{
}

