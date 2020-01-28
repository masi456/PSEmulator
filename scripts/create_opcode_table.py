def main():

    for i in range(0xFF + 1):
        s = hex(i)
        print(
f"""void opcode_{s}(CpuState *state)
{{
    spdlog::error("Invalid opcode {s}");
}}\n""")

    print(     
"""std::vector<std::function<void(CpuState *)>> initializeOpcodeTable()
{
    auto opcodeTable = std::vector<std::function<void(CpuState *)>>(0xFF);""")

    for i in range(0xFF + 1):
        s = hex(i)
        print(f"    opcodeTable[{s}] = opcode_{s};")

    print(
"""    return opcodeTable;
}""")

main()