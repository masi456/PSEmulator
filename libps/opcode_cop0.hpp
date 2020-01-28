#pragma once

#include "opcode.hpp"
#include "cpustate.hpp"

namespace OpcodeImplementationCop0 {
void mtc0(Opcode opcode, CpuState *cpuState);
};