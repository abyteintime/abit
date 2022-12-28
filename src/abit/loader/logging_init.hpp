#pragma once

#include <memory>

#include "spdlog/spdlog.h"

namespace abit {

void
InitializeLogging() noexcept;

void
DeinitializeLogging() noexcept;

spdlog::sink_ptr&
GetConsoleSink();

spdlog::sink_ptr&
GetFileSink();

}
