#pragma once

#include <optional>
#include <limits>

std::optional<int32_t> checked_add_signed(int32_t a, int32_t b) {
    if (b > 0 && a > std::numeric_limits<int32_t>::max() - b) {
        return {};
    }
    return a + b;
}

std::optional<uint32_t> checked_add_unsigned(uint32_t a, uint32_t b) {
    if (a > std::numeric_limits<uint32_t>::max() - b) {
        return {};
    }
    return a + b;
}
