#pragma once

#include "memory_region.hpp"

#include "libutils/data.hpp"

class BIOS 
    : public MemoryRegion
{
    ByteBuffer _data;

public:
    BIOS(const ByteBuffer &buffer);

    virtual uint32_t size() override;
    virtual uint32_t u32(uint32_t offset) override;
    virtual uint32_t u32Write(uint32_t offset, uint32_t value) override;
};