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

#include "utilserver.h"
#include "database.h"
#include "worldserver.h"

#include "../common/debug.h"
#include "../common/opcodemgr.h"
#include "../common/EQEmuConfig.h"
#include "../common/EQStreamFactory.h"
#include "../common/rulesys.h"
#include "../common/servertalk.h"
#include "../common/platform.h"
#include "../common/crash.h"

// TODO: This shouldn't be declared here, but the code expects it.
// currently every server project needs to define this. Seems silly...
TimeoutManager timeout_manager;

UtilServer::UtilServer()
  : m_worldServer(0),
	m_database(0),
	m_config(0),
	m_runLoop(true)
{
}

UtilServer::~UtilServer()
{
	delete m_worldServer;
	m_worldServer = 0;

	delete m_database;
	m_database = 0;
}

int UtilServer::Initialize()
{
	_log(UTILSERVER__INIT, "Starting EQEmu UtilityServer");

	if (!EQEmuConfig::LoadConfig())
	{
		_log(UTILSERVER__INIT, "Loading server configuration failed.");
		return 1;
	}

	m_config = EQEmuConfig::get();

	if (!load_log_settings(m_config->LogSettingsFile.c_str()))
		_log(UTILSERVER__INIT, "Warning: Unable to read %s", m_config->LogSettingsFile.c_str());
	else
		_log(UTILSERVER__INIT, "Log settings loaded from %s", m_config->LogSettingsFile.c_str());

	m_shortName = m_config->ShortName;

	_log(UTILSERVER__INIT, "Connecting to MySQL...");

	m_database = new Database();
	if (!m_database->Connect(
		m_config->DatabaseHost.c_str(),
		m_config->DatabaseUsername.c_str(),
		m_config->DatabasePassword.c_str(),
		m_config->DatabaseDB.c_str(),
		m_config->DatabasePort))
	{
		_log(UTILSERVER__ERROR, "Cannot continue without a database connection.");
		return 1;
	}

	m_worldServer = new WorldServer(m_config);
	m_worldServer->Connect();

	return 0;
}

void UtilServer::Shutdown()
{
	m_runLoop = false;

	if (m_worldServer)
	{
		m_worldServer->Disconnect();
	}
}

int UtilServer::Run()
{
	Timer interserverTimer(INTERSERVER_TIMER); // does auto-reconnect

	while (m_runLoop)
	{
		Timer::SetCurrentTime();

		if (interserverTimer.Check())
		{
			if (m_worldServer->TryReconnect() && !m_worldServer->Connected())
			{
				m_worldServer->AsyncConnect();
			}
		}

		m_worldServer->Process();

		timeout_manager.CheckTimeouts();
		Sleep(100);
	}
}
