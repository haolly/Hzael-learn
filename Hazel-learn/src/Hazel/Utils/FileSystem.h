#pragma once
#include <filesystem>

namespace Hazel
{
	enum class FileSystemAction
	{
		Added, Rename, Modified, Delete	
	};
	
	class FileSystem
	{
	public:
		static bool CreateFolder(const std::string& directory);
		static bool CreateFolder(const std::filesystem::path& directory);
		static bool Exists(const std::string& filepath);
		static bool Exists(const std::filesystem::path& filepath);

		static bool ShowFileInExplorer(const std::filesystem::path& path);
		static bool OpenDirectoryInExplorer(const std::filesystem::path& path);
		
	};
}
