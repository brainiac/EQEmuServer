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

#include "../common/platform.h"
#include "../common/crash.h"
#include "../common/debug.h"
#include <signal.h>

void CatchSingal(int sig_num);

UtilServer g_utilServer;

int main(int argc, char* argv[])
{
	RegisterExecutablePlatform(ExePlatformUtility);
	set_exception_handler();

	if (signal(SIGINT, CatchSignal) == SIG_ERR
		|| signal(SIGTERM, CatchSignal) == SIG_ERR)
	{
		_log(UTILSERVER__ERROR, "Could not set signal handler");
		return 1;
	}

	int initResult = g_utilServer.Initialize();
	if (initResult == 0)
	{
		initResult = g_utilServer.Run();
	}

	return initResult;
}

void CatchSignal(int signum)
{
	g_utilServer.Shutdown();
}
