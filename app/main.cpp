#include "libps/playstation.hpp"

#include <exception>
#include <iostream>
#include <spdlog/spdlog.h>

int main(int, char **)
{
    spdlog::set_level(spdlog::level::trace);
    spdlog::debug("Hello, Playstation!");

    auto ps = Playstation();
    ps.initialize();
    ps.intializeBios("D:/Programmierung/playground/cpp/PSEmulator/files/SCPH-1001.bin");
    ps.run();
}