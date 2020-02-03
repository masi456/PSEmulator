#pragma once

#include "opcode.hpp"
#include "cpustate.hpp"

namespace OpcodeImplementationCop0 {
void mfc0(Opcode opcode, CpuState *cpuState, IOpcodeCpuCallbacks *cpuCallbacks);
void mtc0(Opcode opcode, CpuState *cpuState, IOpcodeCpuCallbacks *cpuCallbacks);
};