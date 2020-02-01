#include "opcode_cpu.hpp"
#include "libutils/math.hpp"
#include "loaddelayslot.hpp"

#include <spdlog/spdlog.h>

void OpcodeImplementationCpu::lui(Opcode opcode, CpuState *cpuState, IOpcodeCpuCallbacks *cpuCallbacks) {
    auto rt = opcode.rt();
    auto imm = opcode.imm16();

    spdlog::trace("[opcode] lui ${}, {:#06x}", rt, imm);

    uint32_t value = imm << 16;
    cpuState->setRegister(rt, value);
    cpuCallbacks->invalidateLoadDelaySlot(rt);
}

void OpcodeImplementationCpu::ori(Opcode opcode, CpuState *cpuState, IOpcodeCpuCallbacks *cpuCallbacks) {
    auto rt = opcode.rt();
    auto rs = opcode.rs();
    auto imm = opcode.imm16();

    spdlog::trace("[opcode] ori ${}, ${}, {:#06x}", rt, rs, imm);

    uint32_t value = cpuState->getRegister(rs) | imm;
    cpuState->setRegister(rt, value);
    cpuCallbacks->invalidateLoadDelaySlot(rt);
}

void OpcodeImplementationCpu::lw(Opcode opcode, CpuState *cpuState, Memory *memory, IOpcodeCpuCallbacks *cpuCallbacks) {
    auto rt = opcode.rt();
    auto rs = opcode.rs();
    auto imm = opcode.imm16();

    spdlog::trace("[opcode] lw ${}, {:#06x}(${})", rt, imm, rs);

    uint32_t address = cpuState->getRegister(rs) + imm;
    auto value = memory->u32(address);
    cpuCallbacks->addLoadDelaySlot(LoadDelaySlot(rt, value));
}

void OpcodeImplementationCpu::sw(Opcode opcode, CpuState *cpuState, Memory *memory) {
    auto rt = opcode.rt();
    auto rs = opcode.rs();
    auto imm = opcode.imm16();

    spdlog::trace("[opcode] sw ${}, {:#06x}(${})", rt, imm, rs);

    uint32_t address = cpuState->getRegister(rs) + imm;
    uint32_t value = cpuState->getRegister(rt);
    memory->u32Write(address, value);
}

void OpcodeImplementationCpu::sh(Opcode opcode, CpuState *cpuState, Memory *memory) {
    auto rt = opcode.rt();
    auto rs = opcode.rs();
    auto imm = opcode.imm16();

    spdlog::trace("[opcode] sh ${}, {:#06x}(${})", rt, imm, rs);

    uint32_t address = cpuState->getRegister(rs) + imm;
    uint16_t value = cpuState->getRegister(rt) & 0xFFFF;
    memory->u16Write(address, value);
}

void OpcodeImplementationCpu::sll(Opcode opcode, CpuState *cpuState, IOpcodeCpuCallbacks *cpuCallbacks) {
    auto rt = opcode.rt();
    auto rd = opcode.rd();
    auto imm = opcode.imm5();

    spdlog::trace("[opcode] sll ${}, ${}, {:#06x}", rd, rt, imm);

    uint32_t value = cpuState->getRegister(rt) << imm;
    cpuState->setRegister(rd, value);
    cpuCallbacks->invalidateLoadDelaySlot(rd);
}

void OpcodeImplementationCpu::addu(Opcode opcode, CpuState *cpuState, IOpcodeCpuCallbacks *cpuCallbacks) {
    auto rt = opcode.rt();
    auto rs = opcode.rs();
    auto rd = opcode.rd();

    spdlog::trace("[opcode] addu ${}, ${}, ${}", rd, rs, rt);

    auto value = cpuState->getRegister(rs) + cpuState->getRegister(rt);
    cpuState->setRegister(rd, value);
    cpuCallbacks->invalidateLoadDelaySlot(rd);
}

void OpcodeImplementationCpu::addi(Opcode opcode, CpuState *cpuState, IOpcodeCpuCallbacks *cpuCallbacks) {
    auto rt = opcode.rt();
    auto rs = opcode.rs();
    auto imm = opcode.imm16();

    spdlog::trace("[opcode] addi ${}, ${}, {:#07x}", rt, rs, imm);

    auto checkedAdd = checked_add(cpuState->getRegister(rs), imm);
    if (checkedAdd) {
        cpuState->setRegister(rt, *checkedAdd);
        cpuCallbacks->invalidateLoadDelaySlot(rt);
    } else {
        throw std::runtime_error("addi overflow");
    }
}

void OpcodeImplementationCpu::addiu(Opcode opcode, CpuState *cpuState, IOpcodeCpuCallbacks *cpuCallbacks) {
    auto rt = opcode.rt();
    auto rs = opcode.rs();
    auto imm = static_cast<int16_t>(opcode.imm16());

    spdlog::trace("[opcode] addiu ${}, ${}, {:#07x}", rt, rs, imm);

    uint32_t value = cpuState->getRegister(rs) + imm;
    cpuState->setRegister(rt, value);
    cpuCallbacks->invalidateLoadDelaySlot(rt);
}

void OpcodeImplementationCpu::j(Opcode opcode, CpuState *cpuState) {
    auto imm = opcode.imm26();

    uint32_t address = (cpuState->getProgramCounter() & 0xF0000000) + (imm << 2);
    spdlog::trace("[opcode] j {:#010x}", address);
    cpuState->setProgramCounter(address);
}

void OpcodeImplementationCpu::or_(Opcode opcode, CpuState *cpuState, IOpcodeCpuCallbacks *cpuCallbacks) {
    auto rt = opcode.rt();
    auto rs = opcode.rs();
    auto rd = opcode.rd();

    spdlog::trace("[opcode] sll ${}, ${}, ${}", rd, rs, rt);

    auto value = cpuState->getRegister(rs) | cpuState->getRegister(rt);
    cpuState->setRegister(rd, value);
    cpuCallbacks->invalidateLoadDelaySlot(rd);
}

void OpcodeImplementationCpu::sltu(Opcode opcode, CpuState *cpuState, IOpcodeCpuCallbacks *cpuCallbacks) {
    auto rt = opcode.rt();
    auto rs = opcode.rs();
    auto rd = opcode.rd();

    spdlog::trace("[opcode] sltu ${}, ${}, ${}", rd, rs, rt);

    auto value = cpuState->getRegister(rs) < cpuState->getRegister(rt) ? 1 : 0;
    cpuState->setRegister(rd, value);
    cpuCallbacks->invalidateLoadDelaySlot(rd);
}

void OpcodeImplementationCpu::bne(Opcode opcode, CpuState *cpuState) {
    auto rt = opcode.rt();
    auto rs = opcode.rs();

    auto imm = static_cast<int32_t>(opcode.imm16signed());
    imm <<= 2;

    spdlog::trace("[opcode] bne ${}, ${}, {:#06x}", rs, rt, imm);

    if (cpuState->getRegister(rs) != cpuState->getRegister(rt)) {
        auto pc = cpuState->getProgramCounter();
        pc += imm;

        // Compensate the hardcoded add to pc in CPU::step()
        pc -= 4;

        cpuState->setProgramCounter(pc);
    }
}
