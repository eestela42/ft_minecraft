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
        colorCode = LogColor().DEBUG;
        break;
    case LogLevel::INFO:
        colorCode = LogColor().INFO;
        break;
    case LogLevel::SUCCESS:
        colorCode = LogColor().SUCCESS;
        break;
    case LogLevel::WARNING:
        colorCode = LogColor().WARNING;
        break;
    case LogLevel::ERROR:
        colorCode = LogColor().ERROR;
        break;
    }
    std::string resetCode = LogColor().RESET;

    out << colorCode << "[" << getCurrentTime() << "] "
        << "[" << getLevelString(level) << "] "
        << message << resetCode << std::endl;
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
