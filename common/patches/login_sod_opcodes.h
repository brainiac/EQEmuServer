/*	EQEmu: EQEmulator

	Copyright (C) 2001-2026 EQEmu Development Team

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

// A default define makes intellisense not always complain
#ifndef OPCODE
#define OPCODE(name, value)
#endif

// EQEmu Public Login Server OPCodes
OPCODE(OP_SessionReady,                 0x0001)
OPCODE(OP_Login,                        0x0002)
OPCODE(OP_ServerListRequest,            0x0004)
OPCODE(OP_PlayEverquestRequest,         0x000d)
OPCODE(OP_PlayEverquestResponse,        0x0022)
OPCODE(OP_ChatMessage,                  0x0017)
OPCODE(OP_LoginAccepted,                0x0018)
OPCODE(OP_ServerListResponse,           0x0019)
OPCODE(OP_Poll,                         0x0029)
OPCODE(OP_LoginExpansionPacketData,     0x0031)
OPCODE(OP_EnterChat,                    0x000f)
OPCODE(OP_PollResponse,                 0x0011)
