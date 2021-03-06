#include "file.hpp"
#include "exception.hpp"

#include <filesystem>
#include <fstream>
#include <spdlog/spdlog.h>

namespace fs = std::filesystem;

File::File(std::string path)
    : _path(path) {}

bool File::exists()
{
    return fs::exists(fs::path(_path));
}

std::vector<uint8_t> File::readAll()
{
    spdlog::trace("Reading file {}", _path);

    auto istream = std::ifstream(_path.c_str(), std::ios::binary | std::ios::ate);
    auto size = istream.tellg();
    istream.seekg(0, std::ios::beg);

    auto buffer = std::vector<uint8_t>(size);
    if (istream.read((char *)buffer.data(), size)) {
        return buffer;
    }
    else {
        throw std::runtime_error(fmt::format("Error reading file {}", _path));
    }
}