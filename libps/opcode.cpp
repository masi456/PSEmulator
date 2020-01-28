#include "opcode.hpp"

Opcode::Opcode(uint32_t opcode)
    : _opcode(opcode) {
}

uint32_t Opcode::raw() {
    return _opcode;
}

uint8_t Opcode::instruction() {
    return _opcode >> 26;
}

RegisterIndex Opcode::rt() {
    return RegisterIndex(
        (_opcode >> 16) & 0x1F);
}

RegisterIndex Opcode::rs() {
    return RegisterIndex(
        (_opcode >> 21) & 0x1F);
}

RegisterIndex Opcode::rd() {
    return RegisterIndex(
        (_opcode >> 11) & 0x1F);
}

uint16_t Opcode::imm16() {
    return _opcode & 0xFFFF;
}

int16_t Opcode::imm16signed() {
    return static_cast<int16_t>(imm16());
}

uint8_t Opcode::imm5() {
    return (_opcode >> 6) & 0x1F;
}

uint32_t Opcode::imm26() {
    return _opcode & 0x03FFFFFF;
}

uint8_t Opcode::subfunction() {
    return _opcode & 0x3F;
}

uint8_t Opcode::cop_opcode() {
    return (_opcode >> 21) & 0x1F;
}

