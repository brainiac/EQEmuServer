/*
	EQEmu:  Everquest Server Emulator

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

#ifndef UTILSERVER_H__
#define UTILSERVER_H__

#include <string>

class Database;
class WorldServer;
class EQEmuConfig;

class UtilServer
{
public:
	UtilServer();
	~UtilServer();

	int Initialize();
	void Shutdown();

	int Run();

private:
	WorldServer* m_worldServer;
	Database* m_database;
	const EQEmuConfig* m_config;

	volatile bool m_runLoop;
	std::string m_shortName;
};

#endif // !UTILSERVER_H__
