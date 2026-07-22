#include "logger.h"
#include <Windows.h>
#include <filesystem>

void Logger::Initialize(const std::string& log_file) {
    std::lock_guard<std::mutex> lock(log_mutex);

    log_file_path = log_file;

    // Create logs directory if it doesn't exist
    std::filesystem::path log_path(log_file);
    std::filesystem::create_directories(log_path.parent_path());

    // Open log file in append mode
    log_file.open(log_file_path, std::ios::app);
    if (!log_file.is_open()) {
        OutputDebugStringA("[Logger] Failed to open log file\n");
        file_logging_enabled = false;
    }
    else {
        Info("===== CS2 External ESP Log Started =====");
    }
}

void Logger::Shutdown() {
    std::lock_guard<std::mutex> lock(log_mutex);
    if (log_file.is_open()) {
        Info("===== CS2 External ESP Log Ended =====");
        log_file.close();
    }
}

std::string Logger::GetTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    char buffer[32];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&time));

    std::string timestamp(buffer);
    timestamp += "." + std::to_string(ms.count()).substr(0, 3);
    return timestamp;
}

std::string Logger::LevelToString(LogLevel level) const {
    switch (level) {
    case LogLevel::DEBUG:    return "DEBUG";
    case LogLevel::INFO:     return "INFO";
    case LogLevel::WARNING:  return "WARNING";
    case LogLevel::ERROR:    return "ERROR";
    case LogLevel::CRITICAL: return "CRITICAL";
    default:                 return "UNKNOWN";
    }
}

void Logger::LogToFile(const std::string& formatted_message) {
    if (file_logging_enabled && log_file.is_open()) {
        log_file << formatted_message << "\n";
        log_file.flush();
    }
}

void Logger::LogToConsole(const std::string& formatted_message) {
    if (console_logging_enabled) {
        OutputDebugStringA((formatted_message + "\n").c_str());
    }
}

void Logger::Log(LogLevel level, const std::string& message) {
    if (static_cast<int>(level) < static_cast<int>(min_log_level)) {
        return;
    }

    std::lock_guard<std::mutex> lock(log_mutex);

    std::ostringstream oss;
    oss << "[" << GetTimestamp() << "] [" << LevelToString(level) << "] " << message;
    std::string formatted = oss.str();

    LogToFile(formatted);
    LogToConsole(formatted);
}
