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

#include "eq_packet.h"

#include "common/misc.h"
#include "common/opcode_manager.h"
#include "common/packet_dump.h"
#include "common/platform.h"

EQPacket::EQPacket(EmuOpcode op, const unsigned char* buf, uint32 len)
	: BasePacket(buf, len)
	, emu_opcode(op)
{
}

void EQPacket::build_raw_header_dump(char* buffer, uint16 seq) const
{
	BasePacket::build_raw_header_dump(buffer, seq);
	buffer += strlen(buffer);

	buffer += sprintf(buffer, "[EmuOpCode 0x%04x Size=%u]\n", emu_opcode, size);
}

void EQPacket::DumpRawHeader(uint16 seq, FILE* to) const
{
	char buff[196];
	build_raw_header_dump(buff, seq);
	fprintf(to, "%s", buff);
}

void EQPacket::build_header_dump(char* buffer) const
{
	sprintf(buffer, "[EmuOpCode 0x%04x Size=%u]", emu_opcode, size);
}

void EQPacket::DumpRawHeaderNoTime(uint16 seq, FILE* to) const
{
	if (src_ip)
	{
		std::string sIP = long2ip(src_ip);
		std::string dIP = long2ip(dst_ip);
		fprintf(to, "[%s:%d->%s:%d] ", sIP.c_str(), src_port, dIP.c_str(), dst_port);
	}
	if (seq != 0xffff)
		fprintf(to, "[Seq=%u] ", seq);

	fprintf(to, "[EmuOpCode 0x%04x Size=%lu]\n", emu_opcode, static_cast<unsigned long>(size));
}

void EQApplicationPacket::build_raw_header_dump(char* buffer, uint16 seq) const
{
	BasePacket::build_raw_header_dump(buffer, seq);
	buffer += strlen(buffer);
	buffer += sprintf(buffer, "[OpCode %s Size=%u]\n", OpcodeManager::EmuToName(emu_opcode), size);
}

void EQApplicationPacket::DumpRawHeader(uint16 seq, FILE* to) const
{
	char buff[196];
	build_raw_header_dump(buff, seq);
	fprintf(to, "%s", buff);
}

void EQApplicationPacket::build_header_dump(char* buffer) const
{
	sprintf(buffer, "[OpCode %s(0x%04x) Size=%u]", OpcodeManager::EmuToName(emu_opcode), GetProtocolOpcode(), size);
}

void EQApplicationPacket::DumpRawHeaderNoTime(uint16 seq, FILE* to) const
{
	if (src_ip)
	{
		std::string sIP = long2ip(src_ip);
		std::string dIP = long2ip(dst_ip);
		fprintf(to, "[%s:%d->%s:%d] ", sIP.c_str(), src_port, dIP.c_str(), dst_port);
	}
	if (seq != 0xffff)
		fprintf(to, "[Seq=%u] ", seq);

	fprintf(to, "[OpCode %s Size=%lu]\n", OpcodeManager::EmuToName(emu_opcode), (unsigned long)size);
}

void EQRawApplicationPacket::build_raw_header_dump(char* buffer, uint16 seq) const
{
	BasePacket::build_raw_header_dump(buffer, seq);
	buffer += strlen(buffer);
	buffer += sprintf(buffer, "[OpCode %s (0x%04x) Size=%u]\n", OpcodeManager::EmuToName(emu_opcode),
		opcode, size);
}

void EQRawApplicationPacket::DumpRawHeader(uint16 seq, FILE* to) const
{
	char buff[196];
	build_raw_header_dump(buff, seq);
	fprintf(to, "%s", buff);
}

void EQRawApplicationPacket::build_header_dump(char* buffer) const
{
	sprintf(buffer, "[OpCode %s (0x%04x) Size=%u]", OpcodeManager::EmuToName(emu_opcode), opcode, size);
}

void EQRawApplicationPacket::DumpRawHeaderNoTime(uint16 seq, FILE* to) const
{
	if (src_ip)
	{
		std::string sIP = long2ip(src_ip);
		std::string dIP = long2ip(dst_ip);
		fprintf(to, "[%s:%d->%s:%d] ", sIP.c_str(), src_port, dIP.c_str(), dst_port);
	}
	if (seq != 0xffff)
		fprintf(to, "[Seq=%u] ", seq);

	fprintf(to, "[OpCode %s (0x%04x) Size=%lu]\n", OpcodeManager::EmuToName(emu_opcode), opcode,
		static_cast<unsigned long>(size));
}

uint32 EQApplicationPacket::serialize(uint16 opcode, unsigned char* dest) const
{
	uint8 OpCodeBytes = app_opcode_size;

	if (app_opcode_size == 1)
		*dest = static_cast<uint8_t>(opcode);
	else
	{
		// Application opcodes with a low order byte of 0x00 require an extra 0x00 byte inserting prior to the opcode.
		if ((opcode & 0x00ff) == 0)
		{
			*dest = 0;
			*reinterpret_cast<uint16_t*>(dest + 1) = opcode;
			++OpCodeBytes;
		}
		else
		{
			*reinterpret_cast<uint16_t*>(dest) = opcode;
		}
	}
	memcpy(dest + OpCodeBytes, pBuffer, size);

	return size + OpCodeBytes;
}

EQApplicationPacket *EQApplicationPacket::Copy() const {
	return(new EQApplicationPacket(*this));
}

EQRawApplicationPacket::EQRawApplicationPacket(uint16 opcode, const unsigned char *buf, const uint32 len)
:	EQApplicationPacket(OP_Unknown, buf, len),
	opcode(opcode)
{
}
EQRawApplicationPacket::EQRawApplicationPacket(const unsigned char *buf, const uint32 len)
: EQApplicationPacket(OP_Unknown, buf+sizeof(uint16), len-sizeof(uint16))
{
	if(GetExecutablePlatform() != ExePlatformUCS) {
		opcode = *((const uint16 *) buf);
		if(opcode == 0x0000)
		{
			if(len >= 3)
			{
				opcode = *((const uint16 *) (buf + 1));
				const unsigned char *packet_start = (buf + 3);
				const int32 packet_length = len - 3;
				safe_delete_array(pBuffer);
				if(packet_length >= 0)
				{
					size = packet_length;
					pBuffer = new unsigned char[size];
					memcpy(pBuffer, packet_start, size);
				}
				else
				{
					size = 0;
				}
			}
			else
			{
				safe_delete_array(pBuffer);
				size = 0;
			}
		}
	} else {
		opcode = *((const uint8 *) buf);
	}
}

void DumpPacket(const EQApplicationPacket* app, bool iShowInfo)
{
	if (iShowInfo)
	{
		fmt::print("Dumping Applayer: 0x{:04x} size:{}\n", app->GetOpcode(), app->size);
	}

	DumpPacketHex(app->pBuffer, app->size);
}

std::string DumpPacketToString(const EQApplicationPacket* app)
{
	return DumpPacketHexToString(app->pBuffer, app->size);
}
