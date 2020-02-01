#pragma once

#include "libutils/data.hpp"
#include "memory_region.hpp"
#include "ram.hpp"

class Ram
    : public MemoryRegion {
private:
    ByteBuffer _data;

    template <typename T>
    T read(uint32_t offset) const {
        throwOnInvalidAccess<T>(offset);
        return *reinterpret_cast<const T *>(_data.data() + offset);
    }

    template <typename T>
    void write(uint32_t offset, T value) {
        throwOnInvalidAccess<T>(offset);
        *reinterpret_cast<T *>(_data.data() + offset) = value;
    }

public:
    Ram();

    virtual uint32_t size() const;

    virtual uint8_t u8(uint32_t offset) const override;
    virtual uint16_t u16(uint32_t offset) const override;
    virtual uint32_t u32(uint32_t offset) const override;

    virtual void u8Write(uint32_t offset, uint8_t value) override;
    virtual void u16Write(uint32_t offset, uint16_t value) override;
    virtual void u32Write(uint32_t offset, uint32_t value) override;
};