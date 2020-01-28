#pragma once

#include <cstdint>

// Must be included to make custom operator<< implementations for spdlog.
#include <spdlog/fmt/ostr.h>

struct RegisterIndex {
private:
    uint8_t _index;

public:
    RegisterIndex(uint8_t index) { _index = index; }
    uint8_t index() const { return _index; }
};
std::ostream &operator<<(std::ostream &os, const RegisterIndex &ri);

namespace Cop0Registers {
const RegisterIndex SR = RegisterIndex(12);

enum Cop0StatusRegisterFlags : uint32_t {
    IsolateCache = (1 << 16)
};
}; // namespace Cop0Registers

class CpuState {
    uint32_t _pc;
    uint32_t _registers[32];
    uint32_t _registersCop0[32];

public:
    void initialize();

    void incrementProgramCounter();
    void setProgramCounter(uint32_t value);
    uint32_t getProgramCounter() const;

    void setRegister(RegisterIndex index, uint32_t value);
    uint32_t getRegister(RegisterIndex index) const;

    void setRegisterCop0(RegisterIndex index, uint32_t value);
    uint32_t getRegisterCop0(RegisterIndex index) const ;
};