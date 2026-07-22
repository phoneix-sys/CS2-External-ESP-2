#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
#include <mutex>
#include <Windows.h>

enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3,
    CRITICAL = 4
};

class Logger {
public:
    static Logger& Instance() {
        static Logger instance;
        return instance;
    }

    void Initialize(const std::string& log_file = "logs/cs2ext.log");
    void Shutdown();

    void Log(LogLevel level, const std::string& message);
    void Debug(const std::string& message) { Log(LogLevel::DEBUG, message); }
    void Info(const std::string& message) { Log(LogLevel::INFO, message); }
    void Warning(const std::string& message) { Log(LogLevel::WARNING, message); }
    void Error(const std::string& message) { Log(LogLevel::ERROR, message); }
    void Critical(const std::string& message) { Log(LogLevel::CRITICAL, message); }

    void SetLogLevel(LogLevel level) { min_log_level = level; }
    void SetFileLogging(bool enabled) { file_logging_enabled = enabled; }
    void SetConsoleLogging(bool enabled) { console_logging_enabled = enabled; }

private:
    Logger() = default;
    ~Logger() { Shutdown(); }

    std::string GetTimestamp() const;
    std::string LevelToString(LogLevel level) const;
    void LogToFile(const std::string& formatted_message);
    void LogToConsole(const std::string& formatted_message);

    std::ofstream log_file;
    std::mutex log_mutex;
    LogLevel min_log_level = LogLevel::DEBUG;
    bool file_logging_enabled = true;
    bool console_logging_enabled = true;
    std::string log_file_path;
};

// Convenience macros
#define LOG_DEBUG(msg) Logger::Instance().Debug(msg)
#define LOG_INFO(msg) Logger::Instance().Info(msg)
#define LOG_WARNING(msg) Logger::Instance().Warning(msg)
#define LOG_ERROR(msg) Logger::Instance().Error(msg)
#define LOG_CRITICAL(msg) Logger::Instance().Critical(msg)
