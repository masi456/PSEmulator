#include "cpustate.hpp"

#include <algorithm>
#include <spdlog/spdlog.h>

std::ostream &operator<<(std::ostream &os, const RegisterIndex &ri) {
    return os << std::to_string(ri.index());
}

void CpuState::initialize() {
    // Start at BIOS segment
    _pc = 0xBFC00000;

    std::fill(std::begin(_registers) + 1, std::end(_registers), 0xDEADBEEF);
    _registers[0] = 0;

    std::fill(std::begin(_registersCop0) + 1, std::end(_registersCop0), 0xDEADBEEF);
    setRegisterCop0(Cop0Registers::SR, 0);
}

void CpuState::incrementProgramCounter() {
    _pc += sizeof(uint32_t);
}

void CpuState::setProgramCounter(uint32_t value) {
    _pc = value;
}

uint32_t CpuState::getProgramCounter() const {
    return _pc;
}

void CpuState::setRegister(RegisterIndex index, uint32_t value) {
    if (index.index() == 0) {
        spdlog::warn("[reg] ignore set $0");
        return;
    }

    spdlog::trace("[reg] write ${} = {:#010x}", index, value);
    _registers[index.index()] = value;
}

uint32_t CpuState::getRegister(RegisterIndex index) const {
    auto value = _registers[index.index()];
    spdlog::trace("[reg] read ${} = {:#010x}", index, value);
    return value;
}

void CpuState::setRegisterCop0(RegisterIndex index, uint32_t value) {
    spdlog::trace("[reg] write cop0_${} = {:#010x}", index, value);
    _registersCop0[index.index()] = value;
}

uint32_t CpuState::getRegisterCop0(RegisterIndex index) const {
    auto value = _registersCop0[index.index()];
    spdlog::trace("[reg] read cop0_${} = {:#010x}", index, value);
    return value;
}
