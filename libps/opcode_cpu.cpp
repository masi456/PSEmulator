#include "opcode_cpu.hpp"
#include "branchdelayslot.hpp"
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

void OpcodeImplementationCpu::andi(Opcode opcode, CpuState *cpuState, IOpcodeCpuCallbacks *cpuCallbacks) {
    auto rt = opcode.rt();
    auto rs = opcode.rs();
    auto imm = opcode.imm16();

    spdlog::trace("[opcode] ori ${}, ${}, {:#06x}", rt, rs, imm);

    uint32_t value = cpuState->getRegister(rs) & imm;
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

void OpcodeImplementationCpu::lbu(Opcode opcode, CpuState *cpuState, Memory *memory, IOpcodeCpuCallbacks *cpuCallbacks) {
    auto rt = opcode.rt();
    auto rs = opcode.rs();
    auto imm = opcode.imm16();

    spdlog::trace("[opcode] lbu ${}, {:#06x}(${})", rt, imm, rs);

    uint32_t address = cpuState->getRegister(rs) + imm;
    auto value = static_cast<uint32_t>(memory->u8(address));
    cpuCallbacks->addLoadDelaySlot(LoadDelaySlot(rt, value));
}

void OpcodeImplementationCpu::lb(Opcode opcode, CpuState *cpuState, Memory *memory, IOpcodeCpuCallbacks *cpuCallbacks) {
    auto rt = opcode.rt();
    auto rs = opcode.rs();
    auto imm = opcode.imm16();

    spdlog::trace("[opcode] lb ${}, {:#06x}(${})", rt, imm, rs);

    uint32_t address = cpuState->getRegister(rs) + imm;
    // Sign extend
    auto value = static_cast<uint32_t>(
        static_cast<int8_t>(memory->u8(address)));
    cpuCallbacks->addLoadDelaySlot(LoadDelaySlot(rt, value));
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
    auto imm = static_cast<int16_t>(opcode.imm16());

    spdlog::trace("[opcode] sw ${}, {:#06x}(${})", rt, imm, rs);

    uint32_t address = cpuState->getRegister(rs) + imm;
    uint32_t value = cpuState->getRegister(rt);
    memory->u32Write(address, value);
}

void OpcodeImplementationCpu::sh(Opcode opcode, CpuState *cpuState, Memory *memory) {
    auto rt = opcode.rt();
    auto rs = opcode.rs();
    auto imm = static_cast<int16_t>(opcode.imm16());

    spdlog::trace("[opcode] sh ${}, {:#06x}(${})", rt, imm, rs);

    uint32_t address = cpuState->getRegister(rs) + imm;
    uint16_t value = cpuState->getRegister(rt) & 0xFFFF;
    memory->u16Write(address, value);
}

void OpcodeImplementationCpu::sb(Opcode opcode, CpuState *cpuState, Memory *memory) {
    auto rt = opcode.rt();
    auto rs = opcode.rs();
    auto imm = static_cast<int16_t>(opcode.imm16());

    spdlog::trace("[opcode] sb ${}, {:#06x}(${})", rt, imm, rs);

    uint32_t address = cpuState->getRegister(rs) + imm;
    uint8_t value = cpuState->getRegister(rt) & 0xFF;
    memory->u8Write(address, value);
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

void OpcodeImplementationCpu::add(Opcode opcode, CpuState *cpuState, IOpcodeCpuCallbacks *cpuCallbacks) {
    auto rt = opcode.rt();
    auto rs = opcode.rs();
    auto rd = opcode.rd();

    spdlog::trace("[opcode] addu ${}, ${}, ${}", rd, rs, rt);

    auto value = checked_add_unsigned(cpuState->getRegister(rs), cpuState->getRegister(rt));
    if (value) {
        cpuState->setRegister(rd, *value);
        cpuCallbacks->invalidateLoadDelaySlot(rd);
    } else {
        spdlog::error("add overflow at {:#010x}", opcode.address());
        throw OpcodeError();
    }
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
    auto imm = static_cast<int16_t>(opcode.imm16());

    spdlog::trace("[opcode] addi ${}, ${}, {:#07x}", rt, rs, imm);

    auto registerValue = static_cast<int32_t>(cpuState->getRegister(rs));
    auto value = checked_add_signed(registerValue, imm);
    if (value) {
        cpuState->setRegister(rt, static_cast<uint32_t>(*value));
        cpuCallbacks->invalidateLoadDelaySlot(rt);
    } else {
        spdlog::error("addi overflow at {:#010x}", opcode.address());
        throw OpcodeError();
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

void OpcodeImplementationCpu::j(Opcode opcode, CpuState *cpuState, IOpcodeCpuCallbacks *cpuCallbacks) {
    auto imm = opcode.imm26();

    uint32_t address = (cpuState->getProgramCounter() & 0xF0000000) + (imm << 2);
    spdlog::trace("[opcode] j {:#010x}", address);
    cpuCallbacks->addBranchDelaySlot(BranchDelaySlot(address));
}

void OpcodeImplementationCpu::jr(Opcode opcode, CpuState *cpuState, IOpcodeCpuCallbacks *cpuCallbacks) {
    auto rs = opcode.rs();

    uint32_t address = cpuState->getRegister(rs);
    spdlog::trace("[opcode] jr ${}", rs);
    cpuCallbacks->addBranchDelaySlot(BranchDelaySlot(address));
}

void OpcodeImplementationCpu::jalr(Opcode opcode, CpuState *cpuState, IOpcodeCpuCallbacks *cpuCallbacks) {
    auto rs = opcode.rs();
    auto rd = opcode.rd();

    // Store return address in $31
    auto returnAddress = opcode.address() + 8;
    cpuState->setRegister(rd, returnAddress);

    uint32_t address = cpuState->getRegister(rs);
    spdlog::trace("[opcode] jr ${}", rs);
    cpuCallbacks->addBranchDelaySlot(BranchDelaySlot(address));
}

void OpcodeImplementationCpu::jal(Opcode opcode, CpuState *cpuState, IOpcodeCpuCallbacks *cpuCallbacks) {
    auto imm = opcode.imm26();

    // Store return address in $31
    auto returnAddress = opcode.address() + 8;
    cpuState->setRegister(RegisterIndex(31), returnAddress);

    uint32_t address = (cpuState->getProgramCounter() & 0xF0000000) + (imm << 2);
    spdlog::trace("[opcode] jal {:#010x}", address);
    cpuCallbacks->addBranchDelaySlot(BranchDelaySlot(address));
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

void OpcodeImplementationCpu::and(Opcode opcode, CpuState *cpuState, IOpcodeCpuCallbacks *cpuCallbacks) {
    auto rt = opcode.rt();
    auto rs = opcode.rs();
    auto rd = opcode.rd();

    spdlog::trace("[opcode] and ${}, ${}, ${}", rd, rs, rt);

    auto value = cpuState->getRegister(rs) & cpuState->getRegister(rt);
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

namespace {
void branch(int16_t imm, const Opcode &opcode, IOpcodeCpuCallbacks *cpuCallbacks) {
    auto imm32 = static_cast<uint32_t>(imm) << 2;
    auto address = opcode.address() + 4 + imm32;
    cpuCallbacks->addBranchDelaySlot(BranchDelaySlot(address));
}
}; // namespace

void OpcodeImplementationCpu::bne(Opcode opcode, CpuState *cpuState, IOpcodeCpuCallbacks *cpuCallbacks) {
    auto rt = opcode.rt();
    auto rs = opcode.rs();
    auto imm = opcode.imm16signed();

    spdlog::trace("[opcode] bne ${}, ${}, {:#06x}", rs, rt, imm);

    if (cpuState->getRegister(rs) != cpuState->getRegister(rt)) {
        branch(imm, opcode, cpuCallbacks);
    }
}

void OpcodeImplementationCpu::beq(Opcode opcode, CpuState *cpuState, IOpcodeCpuCallbacks *cpuCallbacks) {
    auto rt = opcode.rt();
    auto rs = opcode.rs();
    auto imm = opcode.imm16signed();

    spdlog::trace("[opcode] beq ${}, ${}, {:#06x}", rs, rt, imm);

    if (cpuState->getRegister(rs) == cpuState->getRegister(rt)) {
        branch(imm, opcode, cpuCallbacks);
    }
}

void OpcodeImplementationCpu::bgtz(Opcode opcode, CpuState *cpuState, IOpcodeCpuCallbacks *cpuCallbacks) {
    auto rs = opcode.rs();
    auto imm = opcode.imm16signed();

    spdlog::trace("[opcode] bgtz ${}, {:#06x}", rs, imm);

    if (cpuState->getRegister(rs) > 0) {
        branch(imm, opcode, cpuCallbacks);
    }
}

void OpcodeImplementationCpu::blez(Opcode opcode, CpuState *cpuState, IOpcodeCpuCallbacks *cpuCallbacks) {
    auto rs = opcode.rs();
    auto imm = opcode.imm16signed();

    spdlog::trace("[opcode] blez ${}, {:#06x}", rs, imm);

    if (cpuState->getRegister(rs) <= 0) {
        branch(imm, opcode, cpuCallbacks);
    }
}

void OpcodeImplementationCpu::bcond(Opcode opcode, CpuState *cpuState, IOpcodeCpuCallbacks *cpuCallbacks) {
    auto subfunction = opcode.bcond_subfunction();
    auto rs = opcode.rs();
    auto imm = opcode.imm16();

    switch (subfunction) {
    case 0x00: // bltz
        return;
    case 0x01: // bgez
        return;
    case 0x10: // bltzal
        return;
    case 0x11: // bgezal
        return;
    default:
        spdlog::error("Unimplemented bcond subfunction");
        throw OpcodeError();
    }
}