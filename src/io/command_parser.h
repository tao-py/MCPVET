#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <string>
#include <vector>
#include <functional>
#include "vertex_mesh.h"

struct Command {
    std::string name;
    std::vector<float> parameters;
    std::function<void()> execute;
};

class CommandParser {
private:
    std::vector<std::string> commandHistory;
    static const int MAX_HISTORY = 50;

public:
    CommandParser();
    
    // 解析并执行命令
    bool parseAndExecute(const std::string& command, std::vector<Mesh>& meshes);
    
    // 解析命令但不执行
    bool parseCommand(const std::string& command, Command& cmd);
    
    // 获取命令历史
    const std::vector<std::string>& getCommandHistory() const;
    
    // 清除命令历史
    void clearHistory();
    
private:
    // 解析特定命令
    bool parsePointCommand(const std::string& params, Command& cmd);
    bool parseLineCommand(const std::string& params, Command& cmd);
    bool parsePlaneCommand(const std::string& params, Command& cmd);
    bool parseCircleCommand(const std::string& params, Command& cmd);
    bool parseSphereCommand(const std::string& params, Command& cmd);
    bool parseBoxCommand(const std::string& params, Command& cmd);
    bool parseCylinderCommand(const std::string& params, Command& cmd);
    
    // 执行命令
    void executeCommand(const Command& cmd, std::vector<Mesh>& meshes);
};

#endif // COMMAND_PARSER_H