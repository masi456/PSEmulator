#pragma once

#include "libutils/exception.hpp"

#include <cstdint>

class MemoryRegion {
public:
    virtual uint32_t size() = 0;

    virtual uint32_t u32(uint32_t offset) = 0;
    virtual uint32_t u32Write(uint32_t offset, uint32_t value) = 0;

    template <typename T>
    bool checkRead(uint32_t offset)
    {
        if (offset + sizeof(T) < size()) {
            return true;            
        }
        return false;
    }

    template <typename T>
    void throwOnInvalidRead(uint32_t offset) {
        if (!checkRead<T>(offset)) {
            THROW(std::runtime_error, "Trying to read past size of memory region.");
        }
    }
};