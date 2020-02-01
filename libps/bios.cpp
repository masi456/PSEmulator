#include "bios.hpp"
#include "libutils/exception.hpp"

#include <exception>
#include <fmt/format.h>
#include <stdexcept>

constexpr uint32_t BIOS_SIZE = 512 * 1024;

BIOS::BIOS(const ByteBuffer &buffer)
    : _data(buffer) {
    if (buffer.size() != BIOS_SIZE) {
        throw std::runtime_error(fmt::format("BIOS buffer does not have the right size ({} vs {})", buffer.size(), BIOS_SIZE));
    }
}

uint32_t BIOS::size() const {
    return static_cast<uint32_t>(_data.size());
}

uint8_t BIOS::u8(uint32_t offset) const {
    return read<uint8_t>(offset);
}
uint16_t BIOS::u16(uint32_t offset) const {
    return read<uint16_t>(offset);
}
uint32_t BIOS::u32(uint32_t offset) const {
    return read<uint32_t>(offset);
}

static void cannotWrite() {
    spdlog::error("Tried to write to BIOS region");
    throw IllegalWrite();
}

void BIOS::u8Write(uint32_t, uint8_t) {
    cannotWrite();
}
void BIOS::u16Write(uint32_t, uint16_t) {
    cannotWrite();
}
void BIOS::u32Write(uint32_t, uint32_t) {
    cannotWrite();
}
