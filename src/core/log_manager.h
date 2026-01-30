#ifndef LOG_MANAGER_H
#define LOG_MANAGER_H

#include <string>
#include <vector>
#include <mutex>
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <iterator>

namespace fs = std::filesystem;

class LogManager {
private:
    static LogManager* instance;
    std::mutex logMutex;
    std::vector<std::string> dailyLogs;
    const size_t MAX_LOG_ENTRIES = 1000;  // 每天最多1000行
    const size_t MAX_LOG_DAYS = 15;       // 最多保留15天的日志
    std::string logDirectory;
    std::string currentDate;

    LogManager();
    bool ensureLogDirectory();
    void cleanupOldLogs();
    std::string getCurrentDateString();
    void saveDailyLogToFile();

public:
    static LogManager* getInstance();
    void setConsoleEnabled(bool enabled) noexcept { consoleEnabled_ = enabled; }
    bool consoleEnabled() const noexcept { return consoleEnabled_; }
    void log(const std::string& message);
    void logOperation(const std::string& operation, const std::string& details = "");
    void logInfo(const std::string& message);
    void logWarning(const std::string& message);
    void logError(const std::string& message);
    void flush(); // 立即将日志写入文件

private:
    bool consoleEnabled_{true};
};

#endif // LOG_MANAGER_H