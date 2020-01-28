#pragma once

#include <functional>
#include <optional>
#include <vector>

#include "cpustate.hpp"
#include "loaddelayslot.hpp"
#include "memory.hpp"
#include "opcode.hpp"

class CPU {
private:
    CpuState _cpuState = {};
    Memory *_memory = nullptr;
    Opcode _nextOpcode = Opcode::NOP();

    std::optional<LoadDelaySlot> _loadDelaySlot[2];

public:
    CPU() = default;

    void setMemory(Memory *memory);

    void initializeState();
    const CpuState *getCpuState() const;

    void decodeAndExecute(Opcode opcode);
    void decodeAndExecuteCop0(Opcode opcode);
    void step();

    void moveAndApplyLoadDelaySlots();
    void invalidateLoadDelaySlot();
};