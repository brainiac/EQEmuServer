/*  EQEMu:  Everquest Server Emulator
	Copyright (C) 2001-2002  EQEMu Development Team (http://eqemu.org)

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

#include "worldserver.h"
#include "database.h"

#include "../common/debug.h"
#include "../common/servertalk.h"
#include "../common/packet_functions.h"
#include "../common/md5.h"
#include "../common/packet_dump.h"
#include "../common/EQEmuConfig.h"

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <iomanip>
#include <time.h>
#include <stdlib.h>
#include <stdarg.h>

WorldServer::WorldServer(const EQEmuConfig* config_)
  : WorldConnection(EmuTCPConnection::packetModeUtilServer, config_->SharedKey.c_str()),
	m_config(config_)
{
	pTryReconnect = true;
}

WorldServer::~WorldServer()
{
}

void WorldServer::OnConnected()
{
	_log(UTILSERVER__INIT, "Connected to World.");
	WorldConnection::OnConnected();
}

void WorldServer::Process()
{
	WorldConnection::Process();
	
	if (!Connected())
		return;

	ServerPacket *pack = 0;

	while ((pack = tcpc.PopPacket()))
	{
		_log(UTILSERVER__TRACE, "Received Opcode: %4X", pack->opcode);

		switch(pack->opcode)
		{
			case 0:
			{
				break;
			}

			case ServerOP_KeepAlive:
			{
				break;
			}

			case ServerOP_UtilServerGeneric:
			{
				_log(UTILSERVER__ERROR, "Received unhandled ServerOP_UtilServerGeneric");
				break;
			}
		}
	}

	safe_delete(pack);
	return;
}
