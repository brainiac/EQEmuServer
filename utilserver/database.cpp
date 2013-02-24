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

#include "database.h"

Database::Database()
{
	DBInitVars();
}

Database::~Database()
{
}

// Establish a connection to a mysql database with the supplied parameters
Database::Database(const std::string& host, const std::string& user, const std::string& password,
	const std::string& database, uint32 port)
{
	DBInitVars();
	Connect(host, user, password, database, port);
}

bool Database::Connect(const std::string& host, const std::string& user, const std::string& password,
	const std::string& database, uint32 port)
{
	uint32 errnum = 0;
	char errbuf[MYSQL_ERRMSG_SIZE];

	if (!Open(host.c_str(), user.c_str(), password.c_str(), database.c_str(), port, &errnum, errbuf))
	{
		LogFile->write(EQEMuLog::Error, "Failed to connect to database: Error: %s", errbuf);
		HandleMysqlError(errnum);

		return false;
	}
	else
	{
		LogFile->write(EQEMuLog::Status, "Using database '%s' at %s:%d", database.c_str(), host.c_str(), port);
		return true;
	}
}

void Database::DBInitVars()
{
}

void Database::HandleMysqlError(uint32 errnum)
{
}
