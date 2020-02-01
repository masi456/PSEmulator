#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

#include "bios.hpp"
#include "memory_region.hpp"

#include "libutils/data.hpp"

enum class MemorySegment {
    RAM,
    EXPANSION_REGION,
    SCRATCHPAD,
    HW_REGISTERS,
    BIOS,
    CACHE_CONTROL
};

struct SegmentAndOffset {
    MemorySegment region;
    uint32_t offset;
};

class Memory {
private:
    std::unique_ptr<MemoryRegion> _ram;
    std::unique_ptr<MemoryRegion> _bios;

    template <typename ValueType>
    void write(uint32_t address, ValueType value, MemoryRegion *memory);
    template <typename ValueType>
    void write(uint32_t address, ValueType value);

    template <typename ValueType>
    ValueType read(uint32_t address, MemoryRegion *memory);
    template <typename ValueType>
    ValueType read(uint32_t address);

public:
    Memory() = default;

    void setBios(std::unique_ptr<MemoryRegion> bios);
    void setRam(std::unique_ptr<MemoryRegion> ram);

    // Memory access
    virtual uint16_t u16(uint32_t address);
    virtual uint32_t u32(uint32_t address);

    virtual void u16Write(uint32_t address, uint16_t value);
    virtual void u32Write(uint32_t address, uint32_t value);

    std::optional<SegmentAndOffset> getSegmentForAddress(uint32_t address);
};
