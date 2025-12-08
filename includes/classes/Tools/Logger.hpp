#pragma once

#include <string>
#include <iostream>
#include <mutex>

enum class LogLevel
{
    DEBUG,
    INFO,
    SUCCESS,
    WARNING,
    ERROR
};

struct LogColor
{
    const std::string RESET = "\033[0m";
    const std::string ERROR = "\033[31m"; // Red
    const std::string WARNING = "\033[33m"; // Yellow
    const std::string SUCCESS = "\033[32m"; // Green
    const std::string INFO = "\033[36m"; // Cyan
    const std::string DEBUG = "\033[34m"; // Blue
};

class Logger
{
public:
    static void log(LogLevel level, const std::string &message);
    static void debug(const std::string &message);
    static void info(const std::string &message);
    static void warn(const std::string &message);
    static void error(const std::string &message);

private:
    static std::string getLevelString(LogLevel level);
    static std::string getCurrentTime();
    static std::mutex logMutex;
};
