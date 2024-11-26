#include "path_manager.h"
#include "file.h"
#include "eqemu_logsys.h"
#include "eqemu_config.h"
#include "strings.h"

#include <filesystem>
namespace fs = std::filesystem;

void PathManager::LoadPaths()
{
	m_server_path = File::FindEqemuConfigPath();

	if (!m_server_path.empty()) {
		std::filesystem::current_path(m_server_path);
	}

	if (m_server_path.empty()) {
		LogInfo("Failed to load server path");
		return;
	}

	LogInfo("server [{}]", m_server_path);

	if (!EQEmuConfig::LoadConfig()) {
		LogError("Failed to load eqemu config");
		return;
	}

	const auto c = EQEmuConfig::get();
	fs::path server_path = fs::path{ m_server_path };
	std::error_code ec;

	// maps
	if (fs::path maps_path = server_path / c->MapDir; fs::is_directory(maps_path, ec)) {
		m_maps_path = fs::relative(maps_path).lexically_normal().string();
	} else if (maps_path = server_path / "maps"; fs::is_directory(maps_path, ec)) {
		m_maps_path = "maps";
	} else {
		m_maps_path = "Maps";
	}

	// quests
	if (fs::path quests_path = server_path / c->QuestDir; fs::is_directory(quests_path, ec)) {
		m_quests_path = fs::relative(quests_path).lexically_normal().string();
	} else {
		m_quests_path = "quests";
	}

	// plugins
	if (fs::path plugins_path = server_path / c->PluginDir; fs::is_directory(plugins_path, ec)) {
		m_plugins_path = fs::relative(plugins_path).lexically_normal().string();
	} else {
		m_plugins_path = "plugins";
	}

	// lua_modules
	if (fs::path lua_modules_path = server_path / c->LuaModuleDir; fs::is_directory(lua_modules_path, ec)) {
		m_lua_modules_path = fs::relative(lua_modules_path).lexically_normal().string();
	} else {
		m_lua_modules_path = "lua_modules";
	}

	// lua mods
	m_lua_mods_path = "mods";

	// patches
	if (fs::path patch_path = server_path / c->PatchDir; fs::is_directory(patch_path, ec)) {
		m_patch_path = fs::relative(patch_path).lexically_normal().string();
	} else {
		m_patch_path = ".";
	}

	// patches
	if (fs::path opcode_path = server_path / c->OpcodeDir; fs::is_directory(opcode_path, ec)) {
		m_opcode_path = fs::relative(opcode_path).lexically_normal().string();
	} else {
		m_opcode_path = ".";
	}

	// shared_memory_path
	if (fs::path shared_memory_path = server_path / c->SharedMemDir; fs::is_directory(shared_memory_path, ec)) {
		m_shared_memory_path = fs::relative(shared_memory_path).lexically_normal().string();
	} else {
		m_shared_memory_path = "shared";
	}

	// logging path
	if (fs::path log_path = server_path / c->LogDir; fs::is_directory(log_path, ec)) {
		m_log_path = fs::relative(log_path).lexically_normal().string();
	} else {
		m_log_path = "logs";
	}

	LogInfo("logs path [{}]", m_log_path);
	LogInfo("lua mods path [{}]", m_lua_mods_path);
	LogInfo("lua_modules path [{}]", m_lua_modules_path);
	LogInfo("maps path [{}]", m_maps_path);
	LogInfo("patches path [{}]", m_patch_path);
	LogInfo("opcode path [{}]", m_opcode_path);
	LogInfo("plugins path [{}]", m_plugins_path);
	LogInfo("quests path [{}]", m_quests_path);
	LogInfo("shared_memory path [{}]", m_shared_memory_path);
}

const std::string &PathManager::GetServerPath() const
{
	return m_server_path;
}

const std::string &PathManager::GetMapsPath() const
{
	return m_maps_path;
}

const std::string &PathManager::GetQuestsPath() const
{
	return m_quests_path;
}

const std::string &PathManager::GetPluginsPath() const
{
	return m_plugins_path;
}

const std::string &PathManager::GetSharedMemoryPath() const
{
	return m_shared_memory_path;
}

const std::string &PathManager::GetLogPath() const
{
	return m_log_path;
}

const std::string &PathManager::GetPatchPath() const
{
	return m_patch_path;
}

const std::string &PathManager::GetOpcodePath() const
{
	return m_opcode_path;
}

const std::string &PathManager::GetLuaModulesPath() const
{
	return m_lua_modules_path;
}

const std::string &PathManager::GetLuaModsPath() const
{
	return m_lua_mods_path;
}
