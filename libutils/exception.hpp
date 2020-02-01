#pragma once

#include <exception>
#include <stdexcept>
#include <spdlog/spdlog.h>

// Ugh. Change once source_location becomes available
// #define THROW(e, msg) (throw e(fmt::format("{}:{} [{}] {}", __FILE__, __LINE__, __func__, msg)))

class NotImplemented : public std::logic_error {
public:
    NotImplemented() : std::logic_error("Function not yet implemented.") {}
};

class OpcodeNotImplemented : public std::runtime_error {
public:
    OpcodeNotImplemented() : std::runtime_error("Opcode not implemented.") {}
};

class IllegalWrite : public std::runtime_error {
public:
    IllegalWrite() : std::runtime_error("Illegal write.") {}
};