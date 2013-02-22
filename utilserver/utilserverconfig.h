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

#ifndef __utilserverconfig_h__
#define __utilserverconfig_h__

#include "../common/EQEmuConfig.h"

class UtilServerConfig : public EQEmuConfig
{
public:
	static const UtilServerConfig* get()
	{
		if (_util_config == NULL)
			LoadConfig();

		return _util_config;
	}

	virtual std::string GetByName(const std::string& var_name) const;

	static bool LoadConfig()
	{
		if (_util_config != NULL)
			delete _util_config;

		_util_config = new UtilServerConfig;
		_config = _util_config;

		return _config->ParseFile(EQEmuConfig::ConfigFile.c_str(), "server");
	}

private:
	static UtilServerConfig* _util_config;
};

#endif // !__utilserverconfig_h__
