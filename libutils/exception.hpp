#pragma once

#include <exception>
#include <stdexcept>
#include <spdlog/spdlog.h>

// Ugh. Change once source_location becomes available
#define THROW(e, msg) (throw e(fmt::format("{}:{} [{}] {}", __FILE__, __LINE__, __func__, msg)))

class NotImplementedError : public std::logic_error {
public:
    NotImplementedError() : std::logic_error("Function not yet implemented.") {}
};

class OpcodeNotImplementedError : public std::runtime_error {
public:
    OpcodeNotImplementedError() : std::runtime_error("Opcode not implemented.") {}
};