#pragma once

#include <functional>
#include <optional>
#include <vector>

#include "cpustate.hpp"
#include "loaddelayslot.hpp"
#include "branchdelayslot.hpp"
#include "memory.hpp"
#include "opcode.hpp"

class CPU
    : public IOpcodeCpuCallbacks {
private:
    CpuState _cpuState = {};
    Memory *_memory = nullptr;

    std::optional<LoadDelaySlot> _loadDelaySlots[2];
    std::optional<BranchDelaySlot> _branchDelaySlots[2];

    void moveAndApplyLoadDelaySlots();
    void moveAndApplyBranchDelaySlots();

public:
    CPU() = default;

    void setMemory(Memory *memory);

    void initializeState();
    const CpuState *getCpuState() const;

    void decodeAndExecute(Opcode opcode);
    void decodeAndExecuteCop0(Opcode opcode);
    void step();

    virtual void invalidateLoadDelaySlot(RegisterIndex index) override;
    virtual void addLoadDelaySlot(LoadDelaySlot slot) override;
    virtual void addBranchDelaySlot(BranchDelaySlot slot) override;
};