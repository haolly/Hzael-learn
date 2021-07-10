#include "hazelPCH.h"
#include "Hazel/Utils/FileSystem.h"

namespace Hazel
{
	bool FileSystem::CreateFolder(const std::string& directory)
	{
		return CreateFolder(std::filesystem::path(directory));
	}

	bool FileSystem::CreateFolder(const std::filesystem::path& directory)
	{
		return std::filesystem::create_directories(directory);
	}

	bool FileSystem::Exists(const std::string& filepath)
	{
		return std::filesystem::exists(std::filesystem::path(filepath));
	}

	bool FileSystem::Exists(const std::filesystem::path& filepath)
	{
		return std::filesystem::exists(filepath);
	}

	bool FileSystem::ShowFileInExplorer(const std::filesystem::path& path)
	{
		auto absolutePath = std::filesystem::canonical(path);
		if(!Exists(absolutePath))
			return false;
		std::string cmd = fmt::format("explorer.exe /select,\"{0}\"", absolutePath.string());
		system(cmd.c_str());
		return true;
	}

	bool FileSystem::OpenDirectoryInExplorer(const std::filesystem::path& path)
	{
		auto absolutePath = std::filesystem::canonical(path);
		if(!Exists(absolutePath))
			return false;
		ShellExecute(NULL, L"explorer", absolutePath.c_str(), NULL, NULL, SW_SHOWNORMAL);
		return true;
	}
}
