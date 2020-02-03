#include "opcode_cop0.hpp"

void OpcodeImplementationCop0::mfc0(Opcode opcode, CpuState *cpuState, IOpcodeCpuCallbacks *cpuCallbacks) {
    auto rt = opcode.rt();
    auto rd = opcode.rd();

    spdlog::trace("[opcode] mfc0 ${}, cop0_${}", rt, rd);

    auto value = cpuState->getRegisterCop0(rd);
    cpuCallbacks->addLoadDelaySlot(LoadDelaySlot(rt, value));
}

void OpcodeImplementationCop0::mtc0(Opcode opcode, CpuState *cpuState, IOpcodeCpuCallbacks *) {
    auto rt = opcode.rt();
    auto rd = opcode.rd();

    spdlog::trace("[opcode] mtc0 ${}, cop0_${}", rt, rd);

    auto value = cpuState->getRegister(rt);
    cpuState->setRegisterCop0(rd, value);
    // TODO: Are there load delay slots on the cop0?
    // cpuCallbacks->invalidateLoadDelaySlot(rd);
}
