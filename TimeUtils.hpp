// TimeUtils.hpp
//
// Provides utility functions for working with time, including getting the current time and formatting time points.
//
// @class TimeUtils
// @description A utility class offering static methods to retrieve and format time values with millisecond precision.
//
// @methods
// - GetCurrentTime: Returns the current system time as a formatted string.
// - FormatTime: Converts a given `time_point` to a formatted string with a customizable format.
//
// @constants
// - Default format: "%Y-%m-%d %H:%M:%S" for `FormatTime`.
//
// @date 2024-12-20
// @version 1.1
//
// @author Junhao Yu

#ifndef TIMEUTILS_HPP
#define TIMEUTILS_HPP

#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>

class TimeUtils {
public:
    // Get the current time as a formatted string
    static std::string GetCurrentTime() {
        return FormatTime(std::chrono::system_clock::now());
    }

    // Format a given time_point to a string
    static std::string FormatTime(std::chrono::system_clock::time_point now, const std::string& format = "%Y-%m-%d %H:%M:%S") {
        using namespace std::chrono;
        time_t now_c = system_clock::to_time_t(now);

        tm now_tm;
#ifdef _WIN32
        localtime_s(&now_tm, &now_c); // Thread-safe version for Windows
#else
        localtime_r(&now_c, &now_tm); // Thread-safe version for POSIX
#endif

        auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
        std::stringstream ss;
        ss << std::put_time(&now_tm, format.c_str());
        ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }
};

#endif
