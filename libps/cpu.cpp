#include "cpu.hpp"
#include "libutils/memory_utils.hpp"
#include "libutils/platform.hpp"
#include "opcode.hpp"
#include "opcode_cop0.hpp"
#include "opcode_cpu.hpp"

#include <algorithm>
#include <cstring>
#include <fmt/format.h>

void CPU::setMemory(Memory *memory) {
    _memory = memory;
}

void CPU::initializeState() {
    _cpuState.initialize();
    _nextOpcode = Opcode::NOP();
}

const CpuState *CPU::getCpuState() const {
    return &_cpuState;
}

/*
Primary opcode field (Bit 26..31)

  00h=SPECIAL 08h=ADDI  10h=COP0 18h=N/A   20h=LB   28h=SB   30h=LWC0 38h=SWC0
  01h=BcondZ  09h=ADDIU 11h=COP1 19h=N/A   21h=LH   29h=SH   31h=LWC1 39h=SWC1
  02h=J       0Ah=SLTI  12h=COP2 1Ah=N/A   22h=LWL  2Ah=SWL  32h=LWC2 3Ah=SWC2
  03h=JAL     0Bh=SLTIU 13h=COP3 1Bh=N/A   23h=LW   2Bh=SW   33h=LWC3 3Bh=SWC3
  04h=BEQ     0Ch=ANDI  14h=N/A  1Ch=N/A   24h=LBU  2Ch=N/A  34h=N/A  3Ch=N/A
  05h=BNE     0Dh=ORI   15h=N/A  1Dh=N/A   25h=LHU  2Dh=N/A  35h=N/A  3Dh=N/A
  06h=BLEZ    0Eh=XORI  16h=N/A  1Eh=N/A   26h=LWR  2Eh=SWR  36h=N/A  3Eh=N/A
  07h=BGTZ    0Fh=LUI   17h=N/A  1Fh=N/A   27h=N/A  2Fh=N/A  37h=N/A  3Fh=N/A


Secondary opcode field (Bit 0..5) (when Primary opcode = 00h)

  00h=SLL   08h=JR      10h=MFHI 18h=MULT  20h=ADD  28h=N/A  30h=N/A  38h=N/A
  01h=N/A   09h=JALR    11h=MTHI 19h=MULTU 21h=ADDU 29h=N/A  31h=N/A  39h=N/A
  02h=SRL   0Ah=N/A     12h=MFLO 1Ah=DIV   22h=SUB  2Ah=SLT  32h=N/A  3Ah=N/A
  03h=SRA   0Bh=N/A     13h=MTLO 1Bh=DIVU  23h=SUBU 2Bh=SLTU 33h=N/A  3Bh=N/A
  04h=SLLV  0Ch=SYSCALL 14h=N/A  1Ch=N/A   24h=AND  2Ch=N/A  34h=N/A  3Ch=N/A
  05h=N/A   0Dh=BREAK   15h=N/A  1Dh=N/A   25h=OR   2Dh=N/A  35h=N/A  3Dh=N/A
  06h=SRLV  0Eh=N/A     16h=N/A  1Eh=N/A   26h=XOR  2Eh=N/A  36h=N/A  3Eh=N/A
  07h=SRAV  0Fh=N/A     17h=N/A  1Fh=N/A   27h=NOR  2Fh=N/A  37h=N/A  3Fh=N/A
*/

void CPU::decodeAndExecute(Opcode opcode) {
    auto instruction = opcode.instruction();
    spdlog::trace("[decode] instruction {0:#04x} ({0:#08b})", instruction);

    IOpcodeCpuCallbacks *opcodeCpuCallbacks = this;

    switch (instruction) {

    case 0x00: {
        auto subfunction = opcode.subfunction();
        spdlog::trace("[decode] subfunction {0:#04x} ({0:#08b})", subfunction);
        switch (subfunction) {
        case 0x00:
            OpcodeImplementationCpu::sll(opcode, &_cpuState, opcodeCpuCallbacks);
            return;

        case 0x21:
            OpcodeImplementationCpu::addu(opcode, &_cpuState, opcodeCpuCallbacks);
            return;

        case 0x25:
            OpcodeImplementationCpu::or_(opcode, &_cpuState, opcodeCpuCallbacks);
            return;

        case 0x2b:
            OpcodeImplementationCpu::sltu(opcode, &_cpuState, opcodeCpuCallbacks);
            return;

        default:
            spdlog::error("Unhandled subfunction {0:#04x} at address {1:#010x}", subfunction, opcode.address());
            throw OpcodeNotImplemented();
            return;
        }
    }

    case 0x02:
        OpcodeImplementationCpu::j(opcode, &_cpuState);
        return;

    case 0x05:
        OpcodeImplementationCpu::bne(opcode, &_cpuState);
        return;

    case 0x08:
        OpcodeImplementationCpu::addi(opcode, &_cpuState, opcodeCpuCallbacks);
        return;

    case 0x09:
        OpcodeImplementationCpu::addiu(opcode, &_cpuState, opcodeCpuCallbacks);
        return;

    case 0x0D:
        OpcodeImplementationCpu::ori(opcode, &_cpuState, opcodeCpuCallbacks);
        return;

    case 0x0F:
        OpcodeImplementationCpu::lui(opcode, &_cpuState, opcodeCpuCallbacks);
        return;

    case 0x10:
        decodeAndExecuteCop0(opcode);
        return;

    case 0x23:
        OpcodeImplementationCpu::lw(opcode, &_cpuState, _memory, opcodeCpuCallbacks);
        return;

    case 0x29: {
        auto cacheIsolation = (_cpuState.getRegisterCop0(Cop0Registers::SR) & Cop0Registers::IsolateCache) > 0;
        if (cacheIsolation) {
            spdlog::warn("Ignoring sw instruction because IsolateCache flag is set in Cop0 SR.");
            return;
        }

        OpcodeImplementationCpu::sh(opcode, &_cpuState, _memory);
        return;
    }

    case 0x2B: {
        auto cacheIsolation = (_cpuState.getRegisterCop0(Cop0Registers::SR) & Cop0Registers::IsolateCache) > 0;
        if (cacheIsolation) {
            spdlog::warn("Ignoring sw instruction because IsolateCache flag is set in Cop0 SR.");
            return;
        }

        OpcodeImplementationCpu::sw(opcode, &_cpuState, _memory);
        return;
    }

    default: {
        auto rawOpcode = opcode.raw();
        spdlog::error("Unhandled opcode {0:#010x} ({0:#034b}) at address {1:#010x}", rawOpcode, opcode.address());
        throw OpcodeNotImplemented();
    }

    } // switch (instruction)
}

void CPU::decodeAndExecuteCop0(Opcode opcode) {
    auto cop_opcode = opcode.cop_opcode();

    IOpcodeCpuCallbacks *opcodeCpuCallbacks = this;

    switch (cop_opcode) {

    case 0b00100:
        OpcodeImplementationCop0::mtc0(opcode, &_cpuState, opcodeCpuCallbacks);
        return;

    default:
        spdlog::error("Unhandled cop0 opcode {0:#04x} ({0:#07b}) at address {1:#010x}", cop_opcode, opcode.address());
        throw OpcodeNotImplemented();

    } // switch (cop_opcode)
}

std::vector<uint32_t> BREAKPOINTS = {};

void CPU::step() {
    // Always execute the last opcode we have seen to handle Branch Delay Slots
    auto opcode = _nextOpcode;

    auto pc = _cpuState.getProgramCounter();
    auto rawOpcode = _memory->u32(pc);
    _nextOpcode = Opcode(rawOpcode);
    _nextOpcode.setAddress(pc);
    _cpuState.incrementProgramCounter();

    auto breakpointHit = std::any_of(BREAKPOINTS.begin(), BREAKPOINTS.end(), [&](auto x) {
        return x == opcode.address();
    });

    if (breakpointHit) {
        platform::debuggerBreak();
    }

    spdlog::trace("[decode] raw {0:#010x} ({0:#034b})", opcode.raw());
    decodeAndExecute(opcode);

    moveAndApplyLoadDelaySlots();
}

void CPU::moveAndApplyLoadDelaySlots() {
    auto first = _loadDelaySlot[0];
    if (first) {
        spdlog::trace("Applying load delay slot value {:010x} for register {}", first->value, first->index);
        _cpuState.setRegister(first->index, first->value);
        _loadDelaySlot[0] = {};
    }

    auto second = _loadDelaySlot[1];
    if (second) {
        _loadDelaySlot[0] = second;
        _loadDelaySlot[1] = {};
    }
}

void CPU::invalidateLoadDelaySlot(RegisterIndex index) {
    if (!_loadDelaySlot[0]) {
        spdlog::trace("Wanted to invalidate load delay slot but no entry present");
        return;
    }

    if (_loadDelaySlot[0]->index != index) {
        spdlog::trace("Wanted to invalidate load delay for index {} but entry for {} is present", index, _loadDelaySlot[0]->index);
        return;
    }

    spdlog::trace("Invalidated load delay slot for register {}", index);
    _loadDelaySlot[0] = {};
}

void CPU::addLoadDelaySlot(LoadDelaySlot slot) {
    _loadDelaySlot[1] = slot;
}