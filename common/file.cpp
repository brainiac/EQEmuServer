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
#include "file.h"
#include "fmt/format.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#if defined(_WIN32)
#include "common/platform/platform.h"
#endif

namespace fs = std::filesystem;

constexpr const char* EQEMU_CONFIG_FILENAME = "eqemu_config.json";
constexpr const char* EQEMU_LOGIN_FILENAME = "login.json";

bool File::Exists(const std::string& name)
{
	std::error_code ec;

	return fs::exists(name, ec);
}

void File::Makedir(const std::string &directory_name)
{
	try
	{
		fs::create_directory(directory_name);
		fs::permissions(directory_name, fs::perms::owner_all);
	}
	catch (const fs::filesystem_error& ex)
	{
		std::cout << "Failed to create directory: " << directory_name << "\n";
		std::cout << ex.what() << "\n";
	}
}

std::string File::GetCurrentProcessPath()
{
	std::error_code ec;

#ifdef __linux__
	fs::path result = fs::canonical("/proc/self/exe", ec).string();
	if (ec)
	{
		result = fs::current_path(ec).string();
	}
#else
	wchar_t buffer[1024];
	::GetModuleFileNameW(nullptr, buffer, 1024);

	fs::path exePath(buffer, std::filesystem::path::native_format);
	return fs::canonical(exePath.parent_path(), ec).string();
#endif
}

static fs::path FindConfigDirectory(const fs::path& basePath)
{
	std::error_code ec;

	if (fs::is_regular_file(basePath / EQEMU_CONFIG_FILENAME, ec))
	{
		return basePath;
	}

	if (fs::is_regular_file(basePath.parent_path() / EQEMU_CONFIG_FILENAME, ec))
	{
		return basePath.parent_path();
	}

	if (fs::is_regular_file(basePath / EQEMU_LOGIN_FILENAME, ec))
	{
		return basePath;
	}

	if (fs::is_regular_file(basePath.parent_path() / EQEMU_LOGIN_FILENAME, ec))
	{
		return basePath.parent_path();
	}

	return {};
}

std::string File::FindEqemuConfigPath()
{
	std::error_code ec;
	fs::path currentPath = fs::absolute(fs::current_path(ec)).lexically_normal();

	fs::path configPath = FindConfigDirectory(currentPath);

	if (configPath.empty())
	{
		configPath = FindConfigDirectory(GetCurrentProcessPath());
	}

	if (!configPath.empty())
	{
		return fs::canonical(configPath, ec).string();
	}

	return {};
}

std::string File::GetCwd()
{
	return fs::current_path().string();
}

FileContentsResult File::GetContents(const std::string& file_name)
{
	try {
		std::ifstream f(file_name, std::ios::in | std::ios::binary);
		if (!f)
		{
			return {
				.error = fmt::format("Couldn't open file [{}]", file_name)
			};
		}

		constexpr size_t CHUNK_SIZE = 4096;  // Read 4KB chunks
		std::string lines;
		std::vector<char> buffer(CHUNK_SIZE);

		while (f.read(buffer.data(), CHUNK_SIZE) || f.gcount() > 0)
		{
			lines.append(buffer.data(), f.gcount());
		}

		return FileContentsResult{
			.contents = lines
		};
	}
	catch (const std::exception& ex)
	{
		return {
			.error = fmt::format("Error reading file [{}]: {}", file_name, ex.what())
		};
	}
}
