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
#include "client_manager.h"

#include "common/file.h"
#include "common/misc.h"
#include "common/opcode_manager.h"
#include "common/patches/opcodes.h"
#include "common/path_manager.h"
#include "loginserver/login_server.h"

extern LoginServer server;
extern bool        run_server;

static const OpcodeValueList* GetTitaniumLoginOpcodes()
{
	static OpcodeValueList opcodes = {
#define OPCODE(name, value) { name, value },
		#include "common/patches/login_titanium_opcodes.h"
#undef OPCODE
	};

	return &opcodes;
}

static const OpcodeValueList* GetSodLoginOpcodes()
{
	static OpcodeValueList opcodes = {
#define OPCODE(name, value) { name, value },
		#include "common/patches/login_sod_opcodes.h"
#undef OPCODE
	};

	return &opcodes;
}

static const OpcodeValueList* GetSteamLatestLoginOpcodes()
{
	static OpcodeValueList opcodes = {
#define OPCODE(name, value) { name, value },
		#include "common/patches/login_steam_latest_opcodes.h"
#undef OPCODE
	};

	return &opcodes;
}

ClientManager::ClientManager()
{
	//
	// Titanium Login Protocol Support
	//

	int titanium_port = server.config.GetVariableInt("client_configuration", "titanium_port", 5998);
	std::string titanium_opcodes_file = server.config.GetVariableString("client_configuration",
		"titanium_opcodes", "login_opcodes.conf");

	EQStreamManagerInterfaceOptions titanium_opts(titanium_port, false, false);
	m_titanium_stream = new EQ::Net::EQStreamManager(titanium_opts);

	std::string titanium_opcodes_path = PathManager::Instance()->FindFilePath(PathLocation::Opcodes, titanium_opcodes_file);
	m_titanium_ops = new OpcodeManager("login_titanium", GetTitaniumLoginOpcodes());

	if (!m_titanium_ops->LoadOpcodes(titanium_opcodes_path))
	{
		LogError("ClientManager fatal error: couldn't load opcodes for Titanium file [{}]", titanium_opcodes_file);
		run_server = false;
	}

	m_titanium_stream->OnNewConnection(
		[this](std::shared_ptr<EQ::Net::EQStream> stream) {
			LogInfo(
				"New Titanium client connection from [{}:{}]",
				long2ip(stream->GetRemoteIP()),
				stream->GetRemotePort()
			);

			stream->SetOpcodeManager(&m_titanium_ops);
			Client* c = new Client(stream, cv_titanium);
			m_clients.push_back(c);
		}
	);

	//
	// SOD Login Protocol Support
	//

	int sod_port = server.config.GetVariableInt("client_configuration", "sod_port", 5999);
	std::string sod_opcodes_file = server.config.GetVariableString("client_configuration",
		"sod_opcodes", "login_opcodes.conf");

	EQStreamManagerInterfaceOptions sod_opts(sod_port, false, false);
	m_sod_stream = new EQ::Net::EQStreamManager(sod_opts);

	std::string sod_opcodes_path = PathManager::Instance()->FindFilePath(PathLocation::Opcodes, sod_opcodes_file);
	m_sod_ops = new OpcodeManager("login_sod", GetSodLoginOpcodes());

	if (!m_sod_ops->LoadOpcodes(sod_opcodes_path))
	{
		LogError("ClientManager fatal error: couldn't load opcodes for SoD file [{}]", sod_opcodes_file);
		run_server = false;
	}

	m_sod_stream->OnNewConnection(
		[this](std::shared_ptr<EQ::Net::EQStream> stream) {
			LogInfo(
				"New SoD+ client connection from [{}:{}]",
				long2ip(stream->GetRemoteIP()),
				stream->GetRemotePort()
			);

			stream->SetOpcodeManager(&m_sod_ops);
			auto* c = new Client(stream, cv_sod);
			m_clients.push_back(c);
		}
	);

	//
	// Steam-Latest Login Protocol Support
	//

	int steam_latest_port = server.config.GetVariableInt("client_configuration", "steam_latest_port", 15900);
	std::string steam_latest_opcodes_file = server.config.GetVariableString("client_configuration",
		"steam_latest_opcodes", "login_opcodes_steam_latest.conf");

	EQStreamManagerInterfaceOptions steam_latest_opts(steam_latest_port, false, false);
	m_steam_latest_stream = new EQ::Net::EQStreamManager(steam_latest_opts);

	std::string steam_latest_opcodes_path = PathManager::Instance()->FindFilePath(PathLocation::Opcodes, steam_latest_opcodes_file);
	m_steam_latest_ops = new OpcodeManager("log_steam_latest", GetSteamLatestLoginOpcodes());

	if (!m_steam_latest_ops->LoadOpcodes(steam_latest_opcodes_path))
	{
		LogError("ClientManager fatal error: couldn't load opcodes for Steam Latest file [{}]", steam_latest_opcodes_file);
		run_server = false;
	}

	m_steam_latest_stream->OnNewConnection(
		[this](std::shared_ptr<EQ::Net::EQStream> stream) {
			LogInfo(
				"New Steam Latest client connection from [{}:{}]",
				long2ip(stream->GetRemoteIP()),
				stream->GetRemotePort()
			);

			stream->SetOpcodeManager(&m_steam_latest_ops);
			Client* c = new Client(stream, cv_steam_latest);
			m_clients.push_back(c);
		}
	);
}

ClientManager::~ClientManager()
{
	delete m_titanium_stream;
	delete m_titanium_ops;
	delete m_sod_stream;
	delete m_sod_ops;
}

void ClientManager::Process()
{
	ProcessDisconnect();

	for (auto it = m_clients.begin(); it != m_clients.end();) {
		Client *c = *it;
		if (!c->Process()) {
			LogWarning("Client had a fatal error and had to be removed from the login");
			delete c;
			it = m_clients.erase(it);
		}
		else {
			++it;
		}
	}
}

void ClientManager::ProcessDisconnect()
{
	auto it = m_clients.begin();
	while (it != m_clients.end()) {
		Client *c = *it;
		if (c->GetConnection()->CheckState(CLOSED)) {
			LogInfo("Client disconnected from the server, removing client");
			delete c;
			it = m_clients.erase(it);
		}
		else {
			++it;
		}
	}
}

void ClientManager::RemoveExistingClient(unsigned int account_id, const std::string &loginserver)
{
	auto it = m_clients.begin();
	while (it != m_clients.end()) {
		Client *c = *it;
		if (c->GetAccountID() == account_id && c->GetLoginServerName() == loginserver) {
			LogInfo("Client attempting to log in existing client already logged in, removing existing client");
			delete c;
			it = m_clients.erase(it);
		}
		else {
			++it;
		}
	}
}

Client *ClientManager::GetClient(unsigned int account_id, const std::string &loginserver)
{
	auto iter = std::find_if(
		m_clients.begin(), m_clients.end(),
		[&](Client *c) {
			return c->GetAccountID() == account_id && c->GetLoginServerName() == loginserver;
		}
	);

	return (iter != m_clients.end()) ? *iter : nullptr;
}
