#pragma once
#include "../stdafx.h"
#include "../Util/defines.h"
#include <typeindex>
#include "../Util/SmartPointer.h"
#include <rapidjson/document.h>

namespace Game {

	class ResourceManager {
	private:
		rapidjson::Document doc;
		std::unordered_map<std::string, String> str_values;
		std::unordered_map<std::string, int> int_values;
		std::unordered_map<std::string, float> float_values;
		std::unordered_map<std::string, bool> bool_values;
	public:
		void LoadJson(String path);
		String GetString(std::string resourceName);
		int GetInt(std::string resourceName);
		float GetFloat(std::string resourceName);
		bool GetBool(std::string resourceName);
	};
}
