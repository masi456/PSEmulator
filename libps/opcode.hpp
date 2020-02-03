#pragma once

#include "branchdelayslot.hpp"
#include "cpustate.hpp"
#include "loaddelayslot.hpp"
#include "memory.hpp"

#include <cstdint>

class Opcode {
private:
    uint32_t _opcode;

    // For debugging purposes
    uint32_t _address;

public:
    Opcode(uint32_t opcode);

    void setAddress(uint32_t address) { _address = address; }
    uint32_t address() { return _address; }

    uint32_t raw();

    // Get bits 31..26
    uint8_t instruction();

    // Get bits 20..16
    RegisterIndex rt();

    // Get bits 25..21
    RegisterIndex rs();

    // Get bits 15..11
    RegisterIndex rd();

    // Get bits 15..0
    uint16_t imm16();
    int16_t imm16signed();

    // Get bits 10..6
    uint8_t imm5();

    // Get bits 25..0
    uint32_t imm26();

    // Gets bits 5..0
    uint8_t subfunction();

    // Get bits 25..21
    uint8_t cop_opcode();

    static Opcode NOP() { return Opcode(0); }
};

class IOpcodeCpuCallbacks {
public:
    virtual void invalidateLoadDelaySlot(RegisterIndex index) = 0;
    virtual void addLoadDelaySlot(LoadDelaySlot slot) = 0;
    virtual void addBranchDelaySlot(BranchDelaySlot slot) = 0;
};