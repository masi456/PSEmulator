#include "memory.hpp"

#include <cstring>
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace {
bool addressInRange(uint32_t address, uint32_t base, uint32_t size) {
    return address >= base &&
           address <= (base + size);
}
} // namespace

void Memory::setBios(std::unique_ptr<MemoryRegion> bios) {
    spdlog::debug("Setting BIOS memory region ({} bytes).", bios->size());
    _bios = std::move(bios);
}

uint32_t Memory::u32(uint32_t address) {
    spdlog::trace("[mem] Reading from {:#010x}", address);

    if (address % 4 != 0) {
        spdlog::warn("Unaligned memory access.");
    }

    auto segmentAndOffset = getSegmentForAddress(address);
    if (!segmentAndOffset) {
        THROW(std::runtime_error, fmt::format("Trying to read from address {:#x}. No matching memory region found."));
    }

    switch (segmentAndOffset->region) {
    case MemorySegment::BIOS:
        return _bios->u32(segmentAndOffset->offset);
    case MemorySegment::HW_REGISTERS:
        spdlog::warn("Ignoring read from memory segment hw registers.");
        return 0;
    case MemorySegment::CACHE_CONTROL:
        spdlog::warn("Ignoring read from memory segment cache control.");
        return 0;
    default:
        throw NotImplementedError();
    }
}

// 0x1f801010
void Memory::u32Write(uint32_t address, uint32_t value) {
    spdlog::trace("[mem] Writing {:#010x} to {:#010x}", value, address);

    if (address % 4 != 0) {
        spdlog::warn("Unaligned memory access.");
    }

    auto segmentAndOffset = getSegmentForAddress(address);
    if (!segmentAndOffset) {
        THROW(std::runtime_error, fmt::format("Trying to read from address {:#x}. No matching memory region found."));
    }

    switch (segmentAndOffset->region) {
    case MemorySegment::BIOS:
        spdlog::warn("Writes to memory segment BIOS are not allowed.");
        return;
    case MemorySegment::HW_REGISTERS:
        spdlog::warn("Ignoring write to memory segment hw registers.");
        return;
    case MemorySegment::CACHE_CONTROL:
        spdlog::warn("Ignoring write to memory segment cache control.");
        return;
    default:
        throw NotImplementedError();
    }
}

// Memory regions
constexpr uint32_t BIOS_SIZE = 512 * 1024;
constexpr uint32_t BIOS_KUSEG = 0x1fc00000;
constexpr uint32_t BIOS_KSEG0 = 0x9fc00000;
constexpr uint32_t BIOS_KSEG1 = 0xbfc00000;

constexpr uint32_t HW_REGISTERS_SIZE = 8 * 1024;
constexpr uint32_t HW_REGISTERS_KUSEG = 0x1f801000;
constexpr uint32_t HW_REGISTERS_KSEG0 = 0x9f801000;
constexpr uint32_t HW_REGISTERS_KSEG1 = 0xbf801000;

constexpr uint32_t CACHE_CONTROL_SIZE = 512;
constexpr uint32_t CACHE_CONTROL_KSEG2 = 0xfffe0000;

std::optional<SegmentAndOffset> Memory::getSegmentForAddress(uint32_t address) {
    if (addressInRange(address, BIOS_KUSEG, BIOS_SIZE)) {
        return SegmentAndOffset{MemorySegment::BIOS, address - BIOS_KUSEG};
    }
    if (addressInRange(address, BIOS_KSEG0, BIOS_SIZE)) {
        return SegmentAndOffset{MemorySegment::BIOS, address - BIOS_KSEG0};
    }
    if (addressInRange(address, BIOS_KSEG1, BIOS_SIZE)) {
        return SegmentAndOffset{MemorySegment::BIOS, address - BIOS_KSEG1};
    }

    if (addressInRange(address, HW_REGISTERS_KUSEG, HW_REGISTERS_SIZE)) {
        return SegmentAndOffset{MemorySegment::HW_REGISTERS, address - HW_REGISTERS_KUSEG};
    }
    if (addressInRange(address, HW_REGISTERS_KSEG0, HW_REGISTERS_SIZE)) {
        return SegmentAndOffset{MemorySegment::HW_REGISTERS, address - HW_REGISTERS_KSEG0};
    }
    if (addressInRange(address, HW_REGISTERS_KSEG1, HW_REGISTERS_SIZE)) {
        return SegmentAndOffset{MemorySegment::HW_REGISTERS, address - HW_REGISTERS_KSEG1};
    }

    if (addressInRange(address, CACHE_CONTROL_KSEG2, CACHE_CONTROL_SIZE)) {
        return SegmentAndOffset{MemorySegment::CACHE_CONTROL, address - CACHE_CONTROL_KSEG2};
    }

    return {};
}
