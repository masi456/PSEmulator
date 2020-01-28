#include "bios.hpp"
#include "playstation.hpp"
#include "libutils/file.hpp"

#include <memory>
#include <spdlog/spdlog.h>

void Playstation::initialize()
{
    _cpu.setMemory(&_memory);
    _cpu.initializeState();
}

void Playstation::intializeBios(const std::string &path)
{
    auto biosFile = File(path);
    if (!biosFile.exists()) {
        throw std::runtime_error(fmt::format("BIOS file {} does not exist", path));
    }

    auto biosData = biosFile.readAll();
    spdlog::debug("Loaded BIOS file has {} bytes", biosData.size());

    auto bios = std::make_unique<BIOS>(biosData);
    _memory.setBios(std::move(bios));
}

void Playstation::run()
{
    auto ri = RegisterIndex(0x0);
    spdlog::debug("{}", ri);

    while (true) {
        _cpu.step();
    }
}
