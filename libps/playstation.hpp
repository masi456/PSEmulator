#pragma once

#include <string>

#include "memory.hpp"
#include "cpu.hpp"

class Playstation
{
private:
    Memory _memory;
    CPU _cpu;

public:
    Playstation() = default;

    void initialize();
    void intializeBios(const std::string &path);
    void run();
};