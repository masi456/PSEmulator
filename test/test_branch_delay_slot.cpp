#include "libps/memory.hpp"
#include "libps/cpu.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <spdlog/spdlog.h>

using testing::_;
using testing::Return;

class MockMemory : public Memory {
public:
    MOCK_METHOD(uint32_t, u32, (uint32_t address), (override));
    MOCK_METHOD(void, u32Write, (uint32_t address, uint32_t value), (override));
};

TEST(BranchDelaySlot, testBranchDelaySlot) {
    spdlog::set_level(spdlog::level::trace);
    auto memory = MockMemory();

    // lw $1, 0($0)
    uint32_t jumpInstruction = static_cast<uint32_t>(
        0x02 << 26 | 
        0xFC0000C >> 2);

    // addi $1, $0, 12
    uint32_t addInstruction1 = static_cast<uint32_t>(
        0x08 << 26 |
        0 << 21 |
        1 << 16 |
        12);

    // addi $2, $0, 13
    uint32_t addInstruction2 =
        0x08 << 26 |
        0 << 21 |
        2 << 16 |
        13;

    // addi $3, $0, 14
    uint32_t addInstruction3 =
        0x08 << 26 |
        0 << 21 |
        3 << 16 |
        14;

    ON_CALL(memory, u32(_))
        .WillByDefault(Return(0xDEADBEEF));

    ON_CALL(memory, u32(0xBFC00000))
        .WillByDefault(Return(jumpInstruction));
    
    // Will be jumped over, but executed
    ON_CALL(memory, u32(0xBFC00004))
        .WillByDefault(Return(addInstruction1));
    // Will be jumped over and not executed
    ON_CALL(memory, u32(0xBFC00008))
        .WillByDefault(Return(addInstruction2));
    // Will be executed again
    ON_CALL(memory, u32(0xBFC0000C))
        .WillByDefault(Return(addInstruction3));

    auto cpu = CPU();
    cpu.initializeState();
    cpu.setMemory(&memory);

    // Only execute 3 instructions because one is jumped over
    for (int i = 0; i < 3; i++) {
        cpu.step();
    }

    auto cpuState = cpu.getCpuState();
    EXPECT_EQ(cpuState->getRegister(RegisterIndex(1)), 12);
    EXPECT_EQ(cpuState->getRegister(RegisterIndex(2)), 0xDEADBEEF);
    EXPECT_EQ(cpuState->getRegister(RegisterIndex(3)), 14);
}