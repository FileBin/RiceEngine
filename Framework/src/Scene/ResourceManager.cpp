#include "pch.h"
#include <GameEngine\Scene\ResourceManager.h>
#include <rapidjson/error/error.h>

namespace Game {
	void ResourceManager::LoadJson(String filename) {
		using namespace rapidjson;
		auto fileData = Util::ReadFile(filename);
		fileData.push_back('\0');

		doc.Parse((const char*)fileData.data());
		if (doc.HasParseError()) THROW_JSON_PARSING_EXCEPTION(filename.c_str(), doc.GetErrorOffset());

		constexpr char res[] = "Resources";

		if (doc.HasMember(res)) {
			auto arr = doc[res].GetArray();
			for (const auto& val : arr) {
				auto it = val.MemberBegin();
				if (it != val.MemberEnd()) {
					if (it->value.IsString())
						str_values.insert({ it->name.GetString(), Util::Utf8ToWstring(it->value.GetString()) });
					else if (it->value.IsInt())
						int_values.insert({ it->name.GetString(), it->value.GetInt() });
					else if (it->value.IsFloat())
						float_values.insert({ it->name.GetString(), it->value.GetFloat() });
					else if (it->value.IsBool())
						bool_values.insert({ it->name.GetString(), it->value.GetBool() });
					else if (it->value.IsObject()) {
						objects.insert({ it->name.GetString(), it->value.GetObj() });
					}
				}
			}
		} else THROW_JSON_HAS_NOT_MEMBER_EXCEPTION(filename.c_str(), res);
	}

	String ResourceManager::GetString(std::string resourceName) {
		try {
			return str_values.at(resourceName);
		} catch (std::out_of_range) {
			THROW_INVALID_ARG_EXCEPTION(resourceName.c_str());
		}
	}

	int ResourceManager::GetInt(std::string resourceName) {
		try {
			return int_values.at(resourceName);
		} catch (std::out_of_range) {
			THROW_INVALID_ARG_EXCEPTION(resourceName.c_str());
		}
	}

	float ResourceManager::GetFloat(std::string resourceName) {
		try {
			return float_values.at(resourceName);
		} catch (std::out_of_range) {
			THROW_INVALID_ARG_EXCEPTION(resourceName.c_str());
		}
	}

	bool ResourceManager::GetBool(std::string resourceName) {
		try {
			return bool_values.at(resourceName);
		} catch (std::out_of_range) {
			THROW_INVALID_ARG_EXCEPTION(resourceName.c_str());
		}
	}

	std::vector<rapidjson::Value>& ResourceManager::GetArray(std::string resourceName) {
		try {
			return arrays.at(resourceName);
		} catch (std::out_of_range) {
			THROW_INVALID_ARG_EXCEPTION(resourceName.c_str());
		}
	}
}