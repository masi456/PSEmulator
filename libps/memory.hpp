#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

#include "bios.hpp"
#include "memory_region.hpp"

#include "libutils/data.hpp"

enum class MemorySegment {
    MAIN_RAM,
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
    std::unique_ptr<MemoryRegion> _bios;

public:
    Memory() = default;

    void setBios(std::unique_ptr<MemoryRegion> bios);

    // Memory access
    virtual uint32_t u32(uint32_t address);
    virtual void u32Write(uint32_t address, uint32_t value);

    std::optional<SegmentAndOffset> getSegmentForAddress(uint32_t address);
};
