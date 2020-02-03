add_library (libps
    playstation.hpp
    playstation.cpp
    memory.hpp
    memory.cpp
    cpustate.hpp
    cpustate.cpp
    cpu.hpp
    cpu.cpp
    opcode.hpp
    opcode.cpp
    opcode_cpu.cpp
    opcode_cpu.hpp
    opcode_cop0.cpp
    opcode_cop0.hpp
    bios.cpp
    bios.hpp
    memory_region.hpp
    branchdelayslot.hpp
    loaddelayslot.hpp
    ram.hpp
    ram.cpp
)

target_link_libraries (libps LINK_PUBLIC libutils)

target_include_directories (libps PUBLIC ../)
