#include "pch.h"
#include "logger.h"

#include "types.h"

void angler::logger::log(const std::string& _message, Severity _severity, std::source_location _location) {
    cstr type = _severity == Severity::MESSAGE ? "Message" : (_severity == Severity::WARNING ? "Warning" : "Error");
    
    std::cout << ANGLER_LOGGER_PREFIX << type << ANGLER_LOGGER_SUFFIX << "\n"
              << "From : " << _location.file_name() << "\n"
              << "Line : " << _location.line() << "\n"
              << "Func : " << _location.function_name() << "\n"
              << "Content : " << _message << '\n';
}

void angler::logger::message(const std::string& _message, std::source_location _location) {
    log(_message, Severity::MESSAGE, _location);
}

void angler::logger::warning(const std::string& _message, std::source_location _location) {
    log(_message, Severity::WARNING, _location);
}

void angler::logger::error(const std::string& _message, std::source_location _location) {
    log(_message, Severity::ERROR, _location);
}