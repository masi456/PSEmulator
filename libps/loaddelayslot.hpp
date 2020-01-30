#pragma once

#include "cpustate.hpp"

#include <cstdint>
#include <optional>

struct LoadDelaySlot {
public:
    RegisterIndex index;
    uint32_t value;

    LoadDelaySlot(RegisterIndex index, uint32_t value)
        : index(index), value(value) {}
};

struct InvalidateLoadDelaySlot {
public:
    RegisterIndex index;
    
    InvalidateLoadDelaySlot(RegisterIndex index)
        : index(index) {}
};