#include <classes/Tools/Logger.hpp>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

std::mutex Logger::logMutex;

void Logger::log(LogLevel level, const std::string &message)
{
    std::lock_guard<std::mutex> lock(logMutex);

    std::ostream &out = (level == LogLevel::ERROR) ? std::cerr : std::cout;

    std::string colorCode;
    switch (level)
    {
    case LogLevel::DEBUG:
        colorCode = "\033[36m";
        break; // Cyan
    case LogLevel::INFO:
        colorCode = "\033[32m";
        break; // Green
    case LogLevel::WARNING:
        colorCode = "\033[33m";
        break; // Yellow
    case LogLevel::ERROR:
        colorCode = "\033[31m";
        break; // Red
    }
    std::string resetCode = "\033[0m";

    out << "[" << getCurrentTime() << "] "
        << colorCode << "[" << getLevelString(level) << "] " << resetCode
        << message << std::endl;
}

void Logger::debug(const std::string &message)
{
    log(LogLevel::DEBUG, message);
}

void Logger::info(const std::string &message)
{
    log(LogLevel::INFO, message);
}

void Logger::warn(const std::string &message)
{
    log(LogLevel::WARNING, message);
}

void Logger::error(const std::string &message)
{
    log(LogLevel::ERROR, message);
}

std::string Logger::getLevelString(LogLevel level)
{
    switch (level)
    {
    case LogLevel::DEBUG:
        return "DEBUG";
    case LogLevel::INFO:
        return "INFO";
    case LogLevel::WARNING:
        return "WARNING";
    case LogLevel::ERROR:
        return "ERROR";
    default:
        return "UNKNOWN";
    }
}

std::string Logger::getCurrentTime()
{
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}
