#pragma once

#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

// Note: add new modules here before using
#include <random>

#include <spdlog/sinks/basic_file_sink.h>
#include <pugixml.hpp>

namespace WinAPI = SKSE::WinAPI;
namespace logger = SKSE::log;
namespace fs = std::filesystem;

using namespace std::literals;

#define DLLEXPORT __declspec(dllexport)
