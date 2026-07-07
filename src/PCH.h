#pragma once

// Precompiled header. CommonLibSSE-NG pulls in the SKSE + Skyrim runtime API,
// spdlog and the standard library used across the plugin.
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

#include <spdlog/sinks/basic_file_sink.h>

// Short alias used throughout for logging.
namespace logger = SKSE::log;

using namespace std::literals;
