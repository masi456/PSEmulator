#pragma once

#include "cpustate.hpp"
#include "opcode.hpp"
#include "loaddelayslot.hpp"

namespace OpcodeImplementationCpu {
void lui(Opcode opcode, CpuState *cpuState);
void ori(Opcode opcode, CpuState *cpuState);
void sw(Opcode opcode, CpuState *cpuState, Memory *memory);
void sll(Opcode opcode, CpuState *cpuState);
void addi(Opcode opcode, CpuState *cpuState);
void addiu(Opcode opcode, CpuState *cpuState);
void j(Opcode opcode, CpuState *cpuState);
void or_(Opcode opcode, CpuState *cpuState);
void bne(Opcode opcode, CpuState *cpuState);

LoadDelaySlot lw(Opcode opcode, CpuState *cpuState, Memory *memory);
}; // namespace OpcodeImplementationCpu
