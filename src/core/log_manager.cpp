#include "log_manager.h"
#include "../path/savepath.h"        // 可执行文件路径获取
#include <iomanip>
#include <chrono>
#include <ctime>

LogManager* LogManager::instance = nullptr;

LogManager::LogManager() {
    // 设置日志目录为可执行文件目录下的 logs 文件夹
    const std::string& exeDir = getExecutableDirectoryString();
    if (!exeDir.empty()) {
        logDirectory = exeDir + "/logs";
    } else {
        logDirectory = "logs";  // 回退到当前目录
    }
    ensureLogDirectory();
    currentDate = getCurrentDateString();
}

bool LogManager::ensureLogDirectory() {
    try {
        if (!fs::exists(logDirectory)) {
            return fs::create_directories(logDirectory);
        }
        return true;
    } catch (const fs::filesystem_error& ex) {
        std::cerr << "Error creating log directory: " << ex.what() << std::endl;
        return false;
    }
}

std::string LogManager::getCurrentDateString() {
    auto now = std::chrono::system_clock::now();
    std::time_t time_t_now = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d");
    return ss.str();
}

void LogManager::cleanupOldLogs() {
    try {
        std::vector<fs::path> logFiles;
        
        // 收集所有日志文件
        for (const auto& entry : fs::directory_iterator(logDirectory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".log") {
                logFiles.push_back(entry.path());
            }
        }
        
        // 按时间排序（最新的在前）
        std::sort(logFiles.begin(), logFiles.end(), [](const fs::path& a, const fs::path& b) {
            return fs::last_write_time(a) > fs::last_write_time(b);
        });
        
        // 删除超过数量限制的旧日志
        for (size_t i = MAX_LOG_DAYS; i < logFiles.size(); ++i) {
            fs::remove(logFiles[i]);
        }
    } catch (const fs::filesystem_error& ex) {
        std::cerr << "Error cleaning up old logs: " << ex.what() << std::endl;
    }
}

void LogManager::saveDailyLogToFile() {
    if (dailyLogs.empty()) return;
    
    std::string fileName = logDirectory + "/log_" + currentDate + ".log";
    
    try {
        std::ofstream file(fileName, std::ios::app);
        if (file.is_open()) {
            for (const auto& logEntry : dailyLogs) {
                file << logEntry << std::endl;
            }
            file.close();
            
            // 清空内存中的日志
            dailyLogs.clear();
        } else {
            std::cerr << "Could not open log file: " << fileName << std::endl;
        }
    } catch (const std::exception& ex) {
        std::cerr << "Error writing to log file: " << ex.what() << std::endl;
    }
}

LogManager* LogManager::getInstance() {
    if (instance == nullptr) {
        instance = new LogManager();
    }
    return instance;
}

void LogManager::log(const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex);
    
    // 检查是否是新的一天
    std::string newDate = getCurrentDateString();
    if (newDate != currentDate) {
        // 保存前一天的日志
        saveDailyLogToFile();
        currentDate = newDate;
        cleanupOldLogs();
    }
    
    // 获取当前时间
    auto now = std::chrono::system_clock::now();
    std::time_t time_t_now = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    
    std::string timestamp = ss.str();
    std::string logEntry = "[" + timestamp + "] " + message;
    
    // 如果超过最大日志数量，则移除最旧的日志
    if (dailyLogs.size() >= MAX_LOG_ENTRIES) {
        dailyLogs.erase(dailyLogs.begin());
    }
    
    dailyLogs.push_back(logEntry);

    if (consoleEnabled_) {
        std::cout << logEntry << std::endl;
    }
}

void LogManager::logOperation(const std::string& operation, const std::string& details) {
    std::string message = "OPERATION: " + operation;
    if (!details.empty()) {
        message += " | Details: " + details;
    }
    log(message);
}

void LogManager::logInfo(const std::string& message) {
    log("INFO: " + message);
}

void LogManager::logWarning(const std::string& message) {
    log("WARNING: " + message);
}

void LogManager::logError(const std::string& message) {
    log("ERROR: " + message);
}

void LogManager::flush() {
    std::lock_guard<std::mutex> lock(logMutex);
    saveDailyLogToFile();
}