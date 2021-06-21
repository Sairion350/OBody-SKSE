#pragma once

// Note: add new modules here before using 

#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "pugixml.hpp"

#include <random>

#include <spdlog/sinks/basic_file_sink.h>

namespace WinAPI
{
	using namespace SKSE::WinAPI;
}

namespace fs = std::filesystem;

#include <spdlog/sinks/base_sink.h>

using namespace std::literals;

namespace logger
{
	using namespace SKSE::log;
}

#define DLLEXPORT __declspec(dllexport)