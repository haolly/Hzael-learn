#pragma once

#ifdef HZ_PLATFORM_WINDOWS
	#include <Windows.h>
#endif

#include <memory>
#include <iostream>
#include <functional>
#include <utility>
#include <algorithm>
#include <random>
#include <fstream>

#include <string>
#include <array>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "Hazel/Core/Base.h"
#include "Hazel/Core/Log.h"
#include "Hazel/Events/Event.h"

#include "Hazel/Debug/Instrumentor.h"
