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
#pragma once

#include "common/types.h"
#include "common/patches/opcodes.h"

#include <mutex>
#include <vector>

enum EmuOpcode : uint16_t;

class OpcodeManager
{
public:
	OpcodeManager(const char* name, const OpcodeValueList* opcodeValues = nullptr);
	~OpcodeManager();

	bool LoadOpcodes(const std::string& filename, bool report_errors = false);

	uint16 EmuToEQ(EmuOpcode emu_op);
	EmuOpcode EQToEmu(uint16 eq_op);

	static const char* EmuToName(EmuOpcode emu_op);
	const char* EQToName(uint16 emu_op);
	EmuOpcode NameSearch(const char* name);

private:
	bool LoadOpcodesFile(const char* filename, bool report_errors);
	bool Set(EmuOpcode emu_op, uint16 eq_op);

	std::mutex m_mutex;
	const char* m_name;
	std::vector<uint16_t> m_emuToEQ;
	std::vector<uint16_t> m_eqToEmu;

	// Compiled in opcode values
	const OpcodeValueList* m_opcodeValues;
};
