#include "opcode_cop0.hpp"

void OpcodeImplementationCop0::mtc0(Opcode opcode, CpuState *cpuState) {
    auto rt = opcode.rt();
    auto rd = opcode.rd();

    spdlog::trace("[opcode] mtc0 ${}, cop0_${}", rt, rd);

    auto value = cpuState->getRegister(rt);
    cpuState->setRegisterCop0(rd, value);
}
