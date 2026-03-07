#pragma once

#include <source_location>

#include "../types.h"

#define ANGLER_LOGGER_PREFIX \
    "[\033[1;34mANGLER\033[0m\t"
#define ANGLER_LOGGER_SUFFIX \
    "]"

#define ANGLER_LOGGER_CURRENT_LOC \
    std::source_location _location = std::source_location::current()

namespace angler::logger {
    enum class Severity : byte {
        MESSAGE,
        WARNING,
        ERROR
    };
    
    void log(const std::string& _message, Severity _severity, ANGLER_LOGGER_CURRENT_LOC);
    void message(const std::string& _message, ANGLER_LOGGER_CURRENT_LOC);
    void warning(const std::string& _message, ANGLER_LOGGER_CURRENT_LOC);
    void error(const std::string& _message, ANGLER_LOGGER_CURRENT_LOC);
}