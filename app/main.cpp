#include "libps/playstation.hpp"

#include <algorithm>
#include <exception>
#include <iostream>
#include <spdlog/spdlog.h>

namespace {
bool commandLineOptionPresent(int argc, char **argv, const std::string &option) {
    auto begin = argv;
    auto end = argv + argc;
    return std::find(begin, end, option) != end;
}
}; // namespace

int main(int argc, char **argv) {
    auto logLevel = commandLineOptionPresent(argc, argv, "--trace") ? spdlog::level::trace : spdlog::level::debug;
    spdlog::set_level(logLevel);
    spdlog::flush_on(spdlog::level::err);

    spdlog::debug("Hello, Playstation!");

    // try {
        auto ps = Playstation();
        ps.initialize();
        ps.intializeBios("D:/Programmierung/C++/PSEmulator/files/SCPH-1001.bin");
        ps.run();
    // } catch (std::exception &e) {
    //    spdlog::error("Unhandled exception occured: {}", e.what());
    // }
}