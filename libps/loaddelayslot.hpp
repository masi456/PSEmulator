#pragma once

#include "cpustate.hpp"

#include <cstdint>
#include <optional>

struct LoadDelaySlot {
    RegisterIndex index;
    uint32_t value;
};

struct OpcodeImplementationResult {
    std::optional<>
};
