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

#include "opcode_manager.h"

#include "common/emu_opcodes.h"
#include "common/eqemu_logsys.h"

#include <cstdio>
#include <unordered_map>
#include <string>

OpcodeManager::OpcodeManager(const char* name, const OpcodeValueList* opcodeValues)
	: m_name(name)
	, m_opcodeValues(opcodeValues)
{
}

OpcodeManager::~OpcodeManager()
{
}

bool OpcodeManager::LoadOpcodesFile(const char* filename, bool report_errors)
{
	FILE* opf = fopen(filename, "r");
	if (opf == nullptr)
	{
		LogError("Unable to open opcodes file [{}]", filename);
		return false;
	}

	std::unordered_map<std::string, uint16> opcodes;

	//load the opcode file into eq, could swap in a nice XML parser here
	char line[2048];
	int lineno = 0;
	uint16 curop;
	while (!feof(opf))
	{
		lineno++;
		line[0] = '\0'; // for blank line at end of file
		if (fgets(line, sizeof(line), opf) == nullptr)
			break;

		// ignore any line that dosent start with OP_
		if (line[0] != 'O' || line[1] != 'P' || line[2] != '_')
			continue;

		char* num = line + 3;	//skip OP_
		// look for the = sign
		while (*num != '=' && *num != '\0')
			num++;

		// make sure we found =
		if (*num != '=')
		{
			if (report_errors)
				fprintf(stderr, "Malformed opcode line at %s:%d\n", filename, lineno);
			continue;
		}
		*num = '\0';    // null terminate the name
		num++;          // num should point to the opcode

		// read the opcode
		if (sscanf(num, "0x%hx", &curop) != 1) {
			if (report_errors)
				fprintf(stderr, "Malformed opcode at %s:%d\n", filename, lineno);
			continue;
		}

		// we have a name and our opcode... stick it in the map
		opcodes[line] = curop;
	}
	fclose(opf);

	// do the mapping and store them in the array
	for (EmuOpcode emu_op = OP_Unknown; emu_op < MaxEmuOpcode; emu_op = static_cast<EmuOpcode>(emu_op + 1))
	{
		// get the name of this emu opcode
		const char* op_name = OpcodeNames[emu_op];
		if (op_name[0] == '\0')
			break;

		// find the opcode in the file
		auto res = opcodes.find(op_name);
		if (res == opcodes.end())
		{
			if (report_errors)
				fprintf(stderr, "Opcode %s is missing from %s\n", op_name, filename);

			continue; // continue to give them a list of all missing opcodes
		}

		Set(emu_op, res->second);
	}

	return true;
}

bool OpcodeManager::Set(EmuOpcode emu_op, uint16 eq_op)
{
	if (static_cast<size_t>(emu_op) >= m_emuToEQ.size())
		return false;
	if (static_cast<size_t>(eq_op) >= m_eqToEmu.size())
		return false;

	m_emuToEQ[emu_op] = eq_op;
	m_eqToEmu[eq_op] = emu_op;
	return true;
}

const char* OpcodeManager::EmuToName(const EmuOpcode emu_op)
{
	return OpcodeNames[emu_op];
}

const char* OpcodeManager::EQToName(const uint16 eq_op)
{
	// first must resolve the eq op to an emu op
	EmuOpcode emu_op = EQToEmu(eq_op);

	return OpcodeNames[emu_op];
}

EmuOpcode OpcodeManager::NameSearch(const char* name)
{
	for (EmuOpcode emu_op = OP_Unknown; emu_op < MaxEmuOpcode; emu_op = static_cast<EmuOpcode>(emu_op + 1))
	{
		// get the name of this emu opcode
		const char* op_name = OpcodeNames[emu_op];

		if (!strcasecmp(op_name, name))
		{
			return emu_op;
		}
	}

	return OP_Unknown;
}

bool OpcodeManager::LoadOpcodes(const std::string& filename, bool report_errors)
{
	std::scoped_lock lock(m_mutex);

	if (m_emuToEQ.empty())
	{
		m_emuToEQ = std::vector<uint16_t>(MaxEmuOpcode, 0);
		m_eqToEmu = std::vector<uint16_t>(MAX_EQ_OPCODE, 0);
	}
	else
	{
		memset(m_emuToEQ.data(), 0, m_emuToEQ.size() * sizeof(uint16_t));
		memset(m_eqToEmu.data(), 0, m_eqToEmu.size() * sizeof(uint16_t));
	}

	bool success = false;

	if (m_opcodeValues != nullptr)
	{
		for (const auto& [emuOpcode, opcodeValue] : *m_opcodeValues)
		{
			if (!Set(emuOpcode, opcodeValue))
			{
				if (report_errors)
					fprintf(stderr, "Built-in Opcode %s has invalid value 0x%x for %s\n", OpcodeNames[emuOpcode], opcodeValue, m_name);
			}
		}

		success = !m_opcodeValues->empty();
	}
	
	if (filename.empty())
	{
		return success;
	}

	return LoadOpcodesFile(filename.c_str(), report_errors);
}

uint16 OpcodeManager::EmuToEQ(const EmuOpcode emu_op)
{
	// opcode is checked for validity in GetEQOpcode
	uint16 res;
	{
		std::scoped_lock lock(m_mutex);
		res = m_emuToEQ[emu_op];
	}

	LogNetcodeDetail("[Opcode Manager] Translate emu [{}] ({:#06x}) eq [{:#06x}]",
		OpcodeNames[emu_op], emu_op, res);
#ifdef DEBUG_TRANSLATE
	fprintf(stderr, "M Translate Emu %s (%d) to EQ 0x%.4x\n", OpcodeNames[emu_op], emu_op, res);
#endif

	return res;
}

EmuOpcode OpcodeManager::EQToEmu(const uint16 eq_op)
{
	EmuOpcode res;
	{
		std::scoped_lock lock(m_mutex);
		res = static_cast<EmuOpcode>(m_eqToEmu[eq_op]);
	}

#ifdef DEBUG_TRANSLATE
	fprintf(stderr, "M Translate EQ 0x%.4x to Emu %s (%d)\n", eq_op, OpcodeNames[res], res);
#endif

	return res;
}
