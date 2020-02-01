#include "ram.hpp"

constexpr uint32_t RAM_SIZE = 2048 * 1024;

Ram::Ram()
    : _data(RAM_SIZE) {
    spdlog::trace("Initialized RAM with size {}", size());
}

uint32_t Ram::size() const {
    return static_cast<uint32_t>(_data.size());
}

uint8_t Ram::u8(uint32_t offset) const {
    return read<uint8_t>(offset);
}
uint16_t Ram::u16(uint32_t offset) const {
    return read<uint16_t>(offset);
}
uint32_t Ram::u32(uint32_t offset) const {
    return read<uint32_t>(offset);
}

void Ram::u8Write(uint32_t offset, uint8_t value) {
    return write<uint8_t>(offset, value);
}
void Ram::u16Write(uint32_t offset, uint16_t value) {
    return write<uint16_t>(offset, value);
}
void Ram::u32Write(uint32_t offset, uint32_t value) {
    return write<uint32_t>(offset, value);
}

