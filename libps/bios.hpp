#pragma once

#include "memory_region.hpp"

#include "libutils/data.hpp"

class BIOS
    : public MemoryRegion {
    ByteBuffer _data;

    template <typename T>
    T read(uint32_t offset) const {
        throwOnInvalidAccess<T>(offset);
        return *reinterpret_cast<const T*>(_data.data() + offset);
    }

public:
    BIOS(const ByteBuffer &buffer);

    virtual uint32_t size() const override;

    virtual uint8_t u8(uint32_t offset) const override;
    virtual uint16_t u16(uint32_t offset) const override;
    virtual uint32_t u32(uint32_t offset) const override;

    virtual void u8Write(uint32_t offset, uint8_t value) override;
    virtual void u16Write(uint32_t offset, uint16_t value) override;
    virtual void u32Write(uint32_t offset, uint32_t value) override;
};