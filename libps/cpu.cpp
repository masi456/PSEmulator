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

void CPU::decodeAndExecute(Opcode opcode) {
    auto instruction = opcode.instruction();
    spdlog::trace("[decode] instruction {0:#04x} ({0:#08b})", instruction);

    switch (instruction) {
    case 0b001111:
        OpcodeImplementationCpu::lui(opcode, &_cpuState);
        return;

    case 0b001101:
        OpcodeImplementationCpu::ori(opcode, &_cpuState);
        return;

    case 0b101011: {
        auto cacheIsolation = (_cpuState.getRegisterCop0(Cop0Registers::SR) & Cop0Registers::IsolateCache) > 0;
        if (cacheIsolation) {
            spdlog::warn("Ignoring sw instruction because IsolateCache flag is set in Cop0 SR.");
            return;
        }

        OpcodeImplementationCpu::sw(opcode, &_cpuState, _memory);
        return;
    }

    case 0b100011:
        _loadDelaySlot[1] = OpcodeImplementationCpu::lw(opcode, &_cpuState, _memory);
        return;

    case 0b001000:
        OpcodeImplementationCpu::addi(opcode, &_cpuState);
        return;

    case 0b001001:
        OpcodeImplementationCpu::addiu(opcode, &_cpuState);
        return;

    case 0b000010:
        OpcodeImplementationCpu::j(opcode, &_cpuState);
        return;

    case 0b000101:
        OpcodeImplementationCpu::bne(opcode, &_cpuState);
        return;

    case 0b000000: {
        auto subfunction = opcode.subfunction();
        spdlog::trace("[decode] subfunction {0:#04x} ({0:#08b})", subfunction);
        switch (subfunction) {
        case 0b000000:
            OpcodeImplementationCpu::sll(opcode, &_cpuState);
            return;
        case 0b100101:
            OpcodeImplementationCpu::or_(opcode, &_cpuState);
            return;
        }
    }

    case 0b010000:
        decodeAndExecuteCop0(opcode);
        return;

    default: {
        auto rawOpcode = opcode.raw();
        spdlog::error("Unhandled opcode {0:#010x} ({0:#034b}) at address {1:#010x}", rawOpcode, opcode.address());
        throw OpcodeNotImplementedError();
    }

    } // switch (instruction)
}

void CPU::decodeAndExecuteCop0(Opcode opcode) {
    auto cop_opcode = opcode.cop_opcode();
    switch (cop_opcode) {

    case 0b00100:
        OpcodeImplementationCop0::mtc0(opcode, &_cpuState);
        return;

    default:
        spdlog::error("Unhandled cop0 opcode {0:#04x} ({0:#07b}) at address {1:#010x}", cop_opcode, opcode.address());
        throw OpcodeNotImplementedError();

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
        _cpuState.setRegister(first->index, first->value);
        _loadDelaySlot[0] = {};
    }

    auto second = _loadDelaySlot[1];
    if (second) {
        first = second;
        _loadDelaySlot[1] = {};
    }
}

void CPU::invalidateLoadDelaySlot() {
    _loadDelaySlot[0] = {};
}