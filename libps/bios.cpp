#include "bios.hpp"
#include "libutils/exception.hpp"

#include <exception>
#include <fmt/format.h>
#include <stdexcept>

constexpr uint32_t BIOS_SIZE = 512 * 1024;

BIOS::BIOS(const ByteBuffer &buffer)
    : _data(buffer)
{
    if (buffer.size() != BIOS_SIZE) {
        THROW(std::runtime_error, fmt::format("BIOS buffer does not have the right size ({} vs {})", buffer.size(), BIOS_SIZE));
    }
}

uint32_t BIOS::size()
{
    return _data.size();
}

uint32_t BIOS::u32(uint32_t offset)
{
    throwOnInvalidRead<uint32_t>(offset);
    return *reinterpret_cast<uint32_t *>(_data.data() + offset);
}

uint32_t BIOS::u32Write(uint32_t offset, uint32_t value)
{
    THROW(std::runtime_error, "Cannot write to BIOS memory.");
}
