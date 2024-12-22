// Logger.hpp
//
// Provides a logging utility to standardize message formatting and output with support for log levels and timestamps.
//
// @class Logger
// @description The `Logger` class offers static methods to log messages with color-coded log levels
//              (INFO, WARNING, ERROR, FINAL) and timestamps.
//
// @methods 
// - Log: Logs a message to a specified output stream with a given log level and timestamp.
//
// @enums 
// - LogLevel: Defines log levels (INFO, WARNING, ERROR, FINAL) for message categorization.
//
// @constants (Color Codes)
// - RESET: Resets text formatting.
// - RED, GREEN, YELLOW, BLUE: ANSI color codes for log levels.
//
// @date 2024-12-20
// @version 1.1
//
// @author Junhao Yu

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <iostream>
#include <sstream>
#include "TimeUtils.hpp"

// Color codes (ANSI escape sequences)
#ifdef _WIN32
    #define RESET   ""
    #define RED     ""
    #define GREEN   ""
    #define YELLOW  ""
    #define BLUE    ""
#else
    #define RESET   "\033[0m"
    #define RED     "\033[31m"
    #define GREEN   "\033[32m"
    #define YELLOW  "\033[33m"
    #define BLUE    "\033[34m"
#endif

enum class LogLevel {
    INFO,
    WARNING,
    ERROR,
    FINAL
};

class Logger {
public:
    // Log a message to the specified output stream
    static void Log(LogLevel level, const std::string& message, std::ostream& out = std::cout) {
        std::string levelStr;
        std::string color;

        // Determine log level string and color
        switch (level) {
            case LogLevel::INFO:
                levelStr = "INFO";
                color = GREEN;
                break;
            case LogLevel::WARNING:
                levelStr = "WARNING";
                color = YELLOW;
                break;
            case LogLevel::ERROR:
                levelStr = "ERROR";
                color = RED;
                break;
            case LogLevel::FINAL:
                levelStr = "FINAL";
                color = BLUE;
                break;
        }

        // Build the log message
        std::ostringstream logMessage;
        logMessage << color << TimeUtils::GetCurrentTime() << " [" << levelStr << "] " << message << RESET;

        // Output the log message
        out << logMessage.str() << std::endl;
    }
};

#endif
