#pragma once

#include <cstdint>

struct BranchDelaySlot {
public:
    uint32_t address;

    BranchDelaySlot(uint32_t jumpAddress)
        : address(jumpAddress) {}
};