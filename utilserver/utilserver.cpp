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
#include "../common/opcodemgr.h"
#include "../common/EQStreamFactory.h"
#include "../common/rulesys.h"
#include "../common/servertalk.h"
#include "../common/platform.h"
#include "../common/crash.h"
#include "database.h"
#include "utilserverconfig.h"
#include "worldserver.h"
#include <list>
#include <signal.h>

volatile bool RunLoops = true;

Database database;
std::string WorldShortName;
TimeoutManager timeout_manager;

const UtilServerConfig* Config;
WorldServer* worldserver = 0;


void CatchSignal(int sig_num)
{
	RunLoops = false;
	if (worldserver)
		worldserver->Disconnect();
}

int main()
{
	RegisterExecutablePlatform(ExePlatformUtility);
	set_exception_handler();

	Timer InterserverTimer(INTERSERVER_TIMER); // does auto-reconnect

	_log(UTILSERVER__INIT, "Starting EQEmu UtilityServer");
	
	if (!UtilServerConfig::LoadConfig())
	{
		_log(UTILSERVER__INIT, "Loading server configuration failed.");
		return 1;
	}

	Config = UtilServerConfig::get();

	if (!load_log_settings(Config->LogSettingsFile.c_str()))
		_log(UTILSERVER__INIT, "Warning: Unable to read %s", Config->LogSettingsFile.c_str());
	else
		_log(UTILSERVER__INIT, "Log settings loaded from %s", Config->LogSettingsFile.c_str());

	WorldShortName = Config->ShortName;

	_log(UTILSERVER__INIT, "Connecting to MySQL...");
	if (!database.Connect(
		Config->DatabaseHost.c_str(),
		Config->DatabaseUsername.c_str(),
		Config->DatabasePassword.c_str(),
		Config->DatabaseDB.c_str(),
		Config->DatabasePort))
	{
		_log(UTILSERVER__ERROR, "Cannot continue without a database connection.");
		return 1;
	}

	if (signal(SIGINT, CatchSignal) == SIG_ERR
		|| signal(SIGTERM, CatchSignal) == SIG_ERR)
	{
		_log(UTILSERVER__ERROR, "Could not set signal handler");
		return 0;
	}

	worldserver = new WorldServer;
	worldserver->Connect();

	while (RunLoops)
	{
		Timer::SetCurrentTime();

		if (InterserverTimer.Check())
		{
			if (worldserver->TryReconnect() && !worldserver->Connected())
				worldserver->AsyncConnect();

		}

		worldserver->Process();

		timeout_manager.CheckTimeouts();
		Sleep(100);
	}
}



