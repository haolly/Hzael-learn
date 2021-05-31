#include "hazelPCH.h"
#include "Hazel/Utils/FileSystem.h"

namespace Hazel
{
	bool FileSystem::CreateFolder(const std::string& filepath)
	{
		BOOL created = CreateDirectoryA(filepath.c_str(), nullptr);
		if(!created)
		{
			DWORD error = GetLastError();
			if(error == ERROR_ALREADY_EXISTS)
				HZ_CORE_ERROR("{0} already exists!", filepath);
			if(error == ERROR_PATH_NOT_FOUND)
				HZ_CORE_ERROR("{0}: One or more directories don't exist.", filepath);

			return false;
		}
		return true;
	}

}