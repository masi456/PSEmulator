#pragma once

#include "cpustate.hpp"
#include "opcode.hpp"

namespace OpcodeImplementationCpu {
void lui(Opcode opcode, CpuState *cpuState, IOpcodeCpuCallbacks *cpuCallbacks);
void ori(Opcode opcode, CpuState *cpuState, IOpcodeCpuCallbacks *cpuCallbacks);
void sll(Opcode opcode, CpuState *cpuState, IOpcodeCpuCallbacks *cpuCallbacks);
void addu(Opcode opcode, CpuState *cpuState, IOpcodeCpuCallbacks *cpuCallbacks);
void addi(Opcode opcode, CpuState *cpuState, IOpcodeCpuCallbacks *cpuCallbacks);
void addiu(Opcode opcode, CpuState *cpuState, IOpcodeCpuCallbacks *cpuCallbacks);
void or_(Opcode opcode, CpuState *cpuState, IOpcodeCpuCallbacks *cpuCallbacks);
void sltu(Opcode opcode, CpuState *cpuState, IOpcodeCpuCallbacks *cpuCallbacks);

void lw(Opcode opcode, CpuState *cpuState, Memory *memory, IOpcodeCpuCallbacks *cpuCallbacks);
void sw(Opcode opcode, CpuState *cpuState, Memory *memory);
void sh(Opcode opcode, CpuState *cpuState, Memory *memory);

void j(Opcode opcode, CpuState *cpuState);
void bne(Opcode opcode, CpuState *cpuState);
}; // namespace OpcodeImplementationCpu
