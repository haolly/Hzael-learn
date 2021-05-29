#pragma once

namespace Hazel::Utils
{
	class StringUtils
	{
	public:
		static std::string GetFileName(const std::string& filePath);
		static std::string GetExtension(const std::string& filename);
		static std::string RemoveExtension(const std::string& filename);
		static std::vector<std::string> SplitString(const std::string& string, const std::string& delimiters);
		static std::vector<std::string> SplitString(const std::string& string, const char delimiter);
		static std::string ToLower(const std::string& string);
	};
}
