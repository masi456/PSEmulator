#include "libps/memory.hpp"
#include "libps/cpu.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using testing::_;
using testing::Return;

class MockMemory : public Memory {
public:
    MOCK_METHOD(uint32_t, u32, (uint32_t address), (override));
    MOCK_METHOD(void, u32Write, (uint32_t address, uint32_t value), (override));
};

TEST(LoadDelaySlot, testLoadDelaySlotNextRead) {
    auto memory = MockMemory();

    // lw $1, 0($0)
    uint32_t loadInstruction = 
        0b100011 << 26 | 
        0 << 21 | 
        1 << 16 | 
        0;

    // addi $2, $1, 0
    uint32_t addInstruction1 =
        0b001000 << 26 |
        1 << 21 |
        2 << 16 |
        0;

    // addi $3, $1, 0
    uint32_t addInstruction2 =
        0b001000 << 26 |
        1 << 21 |
        3 << 16 |
        0;

    ON_CALL(memory, u32(_))
        .WillByDefault(Return(0xDEADBEEF));

    ON_CALL(memory, u32(0xBFC00000))
        .WillByDefault(Return(loadInstruction));
    ON_CALL(memory, u32(0xBFC00004))
        .WillByDefault(Return(addInstruction1));
    ON_CALL(memory, u32(0xBFC00008))
        .WillByDefault(Return(addInstruction2));

    ON_CALL(memory, u32(0))
        .WillByDefault(Return(12));

    
    auto cpu = CPU();
    cpu.initializeState();

    cpu.setMemory(&memory);
    for (int i = 0; i < 3; i++) {
        cpu.step();
    }

    auto cpuState = cpu.getCpuState();
    ASSERT_EQ(cpuState->getRegister(RegisterIndex(1)), 12);
    ASSERT_EQ(cpuState->getRegister(RegisterIndex(2)), 0);
    ASSERT_EQ(cpuState->getRegister(RegisterIndex(3)), 12);
}

TEST(LoadDelaySlot, testLoadDelaySlotNextWrite) {
    auto memory = MockMemory();

    // lw $1, 0($0)
    uint32_t loadInstruction = 
        0b100011 << 26 | 
        0 << 21 | 
        1 << 16 | 
        0;

    // addi $1, $0, 42
    uint32_t addInstruction1 =
        0b001000 << 26 |
        0 << 21 |
        1 << 16 |
        42;

    ON_CALL(memory, u32(_))
        .WillByDefault(Return(0xDEADBEEF));

    ON_CALL(memory, u32(0xBFC00000))
        .WillByDefault(Return(loadInstruction));
    ON_CALL(memory, u32(0xBFC00004))
        .WillByDefault(Return(addInstruction1));

    ON_CALL(memory, u32(0))
        .WillByDefault(Return(12));

    auto cpu = CPU();
    cpu.initializeState();

    cpu.setMemory(&memory);
    for (int i = 0; i < 3; i++) {
        cpu.step();
    }

    auto cpuState = cpu.getCpuState();
    ASSERT_EQ(cpuState->getRegister(RegisterIndex(1)), 42);
}
