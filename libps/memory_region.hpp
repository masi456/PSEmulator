#pragma once

#include "libutils/exception.hpp"

#include <cstdint>

class MemoryRegion {
public:
    virtual uint32_t size() const = 0;

    virtual uint8_t u8(uint32_t offset) const = 0;
    virtual uint16_t u16(uint32_t offset) const = 0;
    virtual uint32_t u32(uint32_t offset) const = 0;

    virtual void u8Write(uint32_t offset, uint8_t value) = 0;
    virtual void u16Write(uint32_t offset, uint16_t value) = 0;
    virtual void u32Write(uint32_t offset, uint32_t value) = 0;

    template <typename T>
    bool checkAccess(uint32_t offset) const {
        if (offset + sizeof(T) < size()) {
            return true;
        }
        return false;
    }

    template <typename T>
    void throwOnInvalidAccess(uint32_t offset) const {
        if (!checkAccess<T>(offset)) {
            throw std::runtime_error("Trying to access past size of memory region.");
        }
    }
};