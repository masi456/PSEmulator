#include "memory.hpp"

#include <cstring>
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace {
bool addressInRange(uint32_t address, uint32_t base, uint32_t size) {
    return address >= base &&
           address <= (base + size);
}
template<typename A, typename B>
constexpr bool sameTypeRemoveQualifier() {
    return std::is_same<std::remove_cv<A>::type, B>::value;
}
} // namespace

template <typename ValueType>
void Memory::write(uint32_t offset, ValueType value, MemoryRegion *memory) {
    if constexpr (sameTypeRemoveQualifier<ValueType, uint8_t>()) {
        memory->u8Write(offset, value);
    } else if constexpr (sameTypeRemoveQualifier<ValueType, uint16_t>()) {
        memory->u16Write(offset, value);
    } else if constexpr (sameTypeRemoveQualifier<ValueType, uint32_t>()) {
        memory->u32Write(offset, value);
    } else {
        throw NotImplemented();
    }
}

template <typename ValueType>
ValueType Memory::read(uint32_t offset, MemoryRegion *memory) {
    if constexpr (sameTypeRemoveQualifier<ValueType, uint8_t>()) {
        return memory->u8(offset);
    } else if constexpr (sameTypeRemoveQualifier<ValueType, uint16_t>()) {
        return memory->u16(offset);
    } else if constexpr (sameTypeRemoveQualifier<ValueType, uint32_t>()) {
        return memory->u32(offset);
    } else {
        throw NotImplemented();
    }
}

template <typename ValueType>
ValueType Memory::read(uint32_t address) {
    spdlog::trace("[mem] Reading from {:#010x}", address);

    if (address % 4 != 0) {
        spdlog::warn("Unaligned memory access.");
    }

    auto segmentAndOffset = getSegmentForAddress(address);
    if (!segmentAndOffset) {
        throw std::runtime_error(fmt::format("Trying to read from address {:#010x}. No matching memory region found.", address));
    }

    switch (segmentAndOffset->region) {
    case MemorySegment::RAM:
        return read<ValueType>(segmentAndOffset->offset, _ram.get());
    case MemorySegment::BIOS:
        return read<ValueType>(segmentAndOffset->offset, _bios.get());
    case MemorySegment::HW_REGISTERS:
        spdlog::warn("Ignoring read from memory segment hw registers.");
        return 0;
    case MemorySegment::CACHE_CONTROL:
        spdlog::warn("Ignoring read from memory segment cache control.");
        return 0;
    default:
        throw NotImplemented();
    }
}

// 0x1f801010
template <typename ValueType>
void Memory::write(uint32_t address, ValueType value) {
    spdlog::trace("[mem] Writing {:#010x} to {:#010x}", value, address);

    if (address % 4 != 0) {
        spdlog::warn("Unaligned memory access.");
    }

    auto segmentAndOffset = getSegmentForAddress(address);
    if (!segmentAndOffset) {
        throw std::runtime_error(fmt::format("Trying to read from address {:#x}. No matching memory region found."));
    }

    switch (segmentAndOffset->region) {
    case MemorySegment::RAM:
        write(segmentAndOffset->offset, value, _ram.get());
        return;
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
        throw NotImplemented();
    }
}

void Memory::setRam(std::unique_ptr<MemoryRegion> ram) {
    spdlog::debug("Setting RAM memory region ({} bytes).", ram->size());
    _ram = std::move(ram);
}

void Memory::setBios(std::unique_ptr<MemoryRegion> bios) {
    spdlog::debug("Setting BIOS memory region ({} bytes).", bios->size());
    _bios = std::move(bios);
}

uint16_t Memory::u16(uint32_t address) {
    return read<uint16_t>(address);
}
uint32_t Memory::u32(uint32_t address) {
    return read<uint32_t>(address);
}

void Memory::u16Write(uint32_t address, uint16_t value) {
    write(address, value);
}
void Memory::u32Write(uint32_t address, uint32_t value) {
    write(address, value);
}

// Memory regions
constexpr uint32_t RAM_SIZE = 2048 * 1024;
constexpr uint32_t RAM_KUSEG = 0x00000000;
constexpr uint32_t RAM_KSEG0 = 0x80000000;
constexpr uint32_t RAM_KSEG1 = 0xA0000000;

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

// TODO: Make this code nicer and without so much branching!
// Can we strip away the first few bits to immediatly get the offset?
std::optional<SegmentAndOffset> Memory::getSegmentForAddress(uint32_t address) {
    if (addressInRange(address, RAM_KUSEG, RAM_SIZE)) {
        return SegmentAndOffset{MemorySegment::RAM, address - RAM_KUSEG};
    }
    if (addressInRange(address, RAM_KSEG0, RAM_SIZE)) {
        return SegmentAndOffset{MemorySegment::RAM, address - RAM_KSEG0};
    }
    if (addressInRange(address, RAM_KSEG1, RAM_SIZE)) {
        return SegmentAndOffset{MemorySegment::RAM, address - RAM_KSEG1};
    }

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
