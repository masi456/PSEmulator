#pragma once

#include <optional>
#include <limits>

std::optional<uint32_t> checked_add(uint32_t a, uint32_t b) {
    if (a > std::numeric_limits<uint32_t>::max() - b) {
        return {};
    }
    return a + b;
}
