#pragma once

namespace Hazel
{
	enum class FileSystemAction
	{
		Added, Rename, Modified, Delete	
	};
	
	class FileSystem
	{
	public:
		static bool CreateFolder(const std::string& filepath);
		static bool Exists(const std::string& filepath);
		
	};
}
