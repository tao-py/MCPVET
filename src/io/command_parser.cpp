#include "command_parser.h"
#include "geometry_factory.h"
#include "log_manager.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cmath>

CommandParser::CommandParser() {}

bool CommandParser::parseAndExecute(const std::string& command, std::vector<Mesh>& meshes) {
    // 添加到命令历史
    if (commandHistory.size() >= MAX_HISTORY) {
        commandHistory.erase(commandHistory.begin());
    }
    commandHistory.push_back(command);
    
    // 解析命令
    Command cmd;
    if (!parseCommand(command, cmd)) {
        return false;
    }
    
    // 执行命令
    executeCommand(cmd, meshes);
    
    return true;
}

bool CommandParser::parseCommand(const std::string& command, Command& cmd) {
    if (command.empty()) {
        return false;
    }
    
    std::string cmdStr = command;
    // 转换为小写以进行比较
    std::transform(cmdStr.begin(), cmdStr.end(), cmdStr.begin(), ::tolower);
    
    // 移除所有空格
    cmdStr.erase(std::remove_if(cmdStr.begin(), cmdStr.end(), ::isspace), cmdStr.end());
    
    size_t parenStart = cmdStr.find('(');
    size_t parenEnd = cmdStr.find(')');
    
    if (parenStart == std::string::npos || parenEnd == std::string::npos) {
        return false; // 需要参数括号
    }
    
    std::string commandName = cmdStr.substr(0, parenStart);
    std::string params = cmdStr.substr(parenStart + 1, parenEnd - parenStart - 1);
    
    // 解析参数
    std::stringstream ss(params);
    std::string item;
    std::vector<float> paramValues;
    
    while (std::getline(ss, item, ',')) {
        try {
            float value = std::stof(item);
            paramValues.push_back(value);
        } catch (...) {
            return false; // 参数格式错误
        }
    }
    
    cmd.name = commandName;
    cmd.parameters = paramValues;
    
    // 设置执行函数
    if (commandName == "point") {
        return parsePointCommand(params, cmd);
    } else if (commandName == "line") {
        return parseLineCommand(params, cmd);
    } else if (commandName == "plane" || commandName == "rectangle") {
        return parsePlaneCommand(params, cmd);
    } else if (commandName == "circle") {
        return parseCircleCommand(params, cmd);
    } else if (commandName == "sphere") {
        return parseSphereCommand(params, cmd);
    } else if (commandName == "box") {
        return parseBoxCommand(params, cmd);
    } else if (commandName == "cylinder") {
        return parseCylinderCommand(params, cmd);
    }
    
    return false; // 未知命令
}

bool CommandParser::parsePointCommand(const std::string& params, Command& cmd) {
    std::stringstream ss(params);
    std::string item;
    std::vector<float> values;
    
    while (std::getline(ss, item, ',')) {
        try {
            values.push_back(std::stof(item));
        } catch (...) {
            return false;
        }
    }
    
    if (values.size() != 3) {
        return false; // Point需要3个参数 (x, y, z)
    }
    
    // 设置执行函数
    cmd.execute = [values]() {
        // 这个函数会在executeCommand中被调用
    };
    
    return true;
}

bool CommandParser::parseLineCommand(const std::string& params, Command& cmd) {
    std::stringstream ss(params);
    std::string item;
    std::vector<float> values;
    
    while (std::getline(ss, item, ',')) {
        try {
            values.push_back(std::stof(item));
        } catch (...) {
            return false;
        }
    }
    
    if (values.size() != 6) {
        return false; // Line需要6个参数 (x1, y1, z1, x2, y2, z2)
    }
    
    cmd.execute = [values]() {
        // 这个函数会在executeCommand中被调用
    };
    
    return true;
}

bool CommandParser::parsePlaneCommand(const std::string& params, Command& cmd) {
    std::stringstream ss(params);
    std::string item;
    std::vector<float> values;
    
    while (std::getline(ss, item, ',')) {
        try {
            values.push_back(std::stof(item));
        } catch (...) {
            return false;
        }
    }
    
    if (values.size() != 2 && values.size() != 1) {
        return false; // Plane需要1或2个参数 (width[, height])
    }
    
    cmd.execute = [values]() {
        // 这个函数会在executeCommand中被调用
    };
    
    return true;
}

bool CommandParser::parseCircleCommand(const std::string& params, Command& cmd) {
    std::stringstream ss(params);
    std::string item;
    std::vector<float> values;
    
    while (std::getline(ss, item, ',')) {
        try {
            values.push_back(std::stof(item));
        } catch (...) {
            return false;
        }
    }
    
    if (values.size() != 1) {
        return false; // Circle需要1个参数 (radius)
    }
    
    cmd.execute = [values]() {
        // 这个函数会在executeCommand中被调用
    };
    
    return true;
}

bool CommandParser::parseSphereCommand(const std::string& params, Command& cmd) {
    std::stringstream ss(params);
    std::string item;
    std::vector<float> values;
    
    while (std::getline(ss, item, ',')) {
        try {
            values.push_back(std::stof(item));
        } catch (...) {
            return false;
        }
    }
    
    if (values.size() != 1) {
        return false; // Sphere需要1个参数 (radius)
    }
    
    cmd.execute = [values]() {
        // 这个函数会在executeCommand中被调用
    };
    
    return true;
}

bool CommandParser::parseBoxCommand(const std::string& params, Command& cmd) {
    std::stringstream ss(params);
    std::string item;
    std::vector<float> values;
    
    while (std::getline(ss, item, ',')) {
        try {
            values.push_back(std::stof(item));
        } catch (...) {
            return false;
        }
    }
    
    if (values.size() != 3) {
        return false; // Box需要3个参数 (width, height, depth)
    }
    
    cmd.execute = [values]() {
        // 这个函数会在executeCommand中被调用
    };
    
    return true;
}

bool CommandParser::parseCylinderCommand(const std::string& params, Command& cmd) {
    std::stringstream ss(params);
    std::string item;
    std::vector<float> values;
    
    while (std::getline(ss, item, ',')) {
        try {
            values.push_back(std::stof(item));
        } catch (...) {
            return false;
        }
    }
    
    if (values.size() != 2) {
        return false; // Cylinder需要2个参数 (radius, height)
    }
    
    cmd.execute = [values]() {
        // 这个函数会在executeCommand中被调用
    };
    
    return true;
}

void CommandParser::executeCommand(const Command& cmd, std::vector<Mesh>& meshes) {
    if (cmd.name == "point" && cmd.parameters.size() == 3) {
        Mesh newMesh("Point_" + std::to_string(meshes.size() + 1));
        GeometryFactory::createPoint(newMesh, glm::vec3(cmd.parameters[0], cmd.parameters[1], cmd.parameters[2]));
        meshes.push_back(newMesh);
        LogManager::getInstance()->logOperation("Object Creation", "Point created via command: (" + 
            std::to_string(cmd.parameters[0]) + ", " + std::to_string(cmd.parameters[1]) + ", " + 
            std::to_string(cmd.parameters[2]) + ")");
    } 
    else if (cmd.name == "line" && cmd.parameters.size() == 6) {
        Mesh newMesh("Line_" + std::to_string(meshes.size() + 1));
        GeometryFactory::createLine(newMesh, 
            glm::vec3(cmd.parameters[0], cmd.parameters[1], cmd.parameters[2]),
            glm::vec3(cmd.parameters[3], cmd.parameters[4], cmd.parameters[5]));
        meshes.push_back(newMesh);
        LogManager::getInstance()->logOperation("Object Creation", "Line created via command");
    } 
    else if ((cmd.name == "plane" || cmd.name == "rectangle") && (cmd.parameters.size() == 1 || cmd.parameters.size() == 2)) {
        Mesh newMesh((cmd.name == "rectangle" ? "Rectangle_" : "Plane_") + std::to_string(meshes.size() + 1));
        float width = cmd.parameters[0];
        float height = (cmd.parameters.size() == 2) ? cmd.parameters[1] : width; // 如果只有一个参数，则为正方形
        GeometryFactory::createRectangle(newMesh, width, height);
        meshes.push_back(newMesh);
        LogManager::getInstance()->logOperation("Object Creation", "Plane created via command");
    } 
    else if (cmd.name == "circle" && cmd.parameters.size() == 1) {
        Mesh newMesh("Circle_" + std::to_string(meshes.size() + 1));
        GeometryFactory::createCircle(newMesh, cmd.parameters[0]);
        meshes.push_back(newMesh);
        LogManager::getInstance()->logOperation("Object Creation", "Circle created via command");
    } 
    else if (cmd.name == "sphere" && cmd.parameters.size() == 1) {
        Mesh newMesh("Sphere_" + std::to_string(meshes.size() + 1));
        GeometryFactory::createSphere(newMesh, cmd.parameters[0]);
        meshes.push_back(newMesh);
        LogManager::getInstance()->logOperation("Object Creation", "Sphere created via command");
    } 
    else if (cmd.name == "box" && cmd.parameters.size() == 3) {
        Mesh newMesh("Box_" + std::to_string(meshes.size() + 1));
        GeometryFactory::createBox(newMesh, cmd.parameters[0], cmd.parameters[1], cmd.parameters[2]);
        meshes.push_back(newMesh);
        LogManager::getInstance()->logOperation("Object Creation", "Box created via command");
    } 
    else if (cmd.name == "cylinder" && cmd.parameters.size() == 2) {
        Mesh newMesh("Cylinder_" + std::to_string(meshes.size() + 1));
        GeometryFactory::createCylinder(newMesh, cmd.parameters[0], cmd.parameters[1]);
        meshes.push_back(newMesh);
        LogManager::getInstance()->logOperation("Object Creation", "Cylinder created via command");
    }
}

const std::vector<std::string>& CommandParser::getCommandHistory() const {
    return commandHistory;
}

void CommandParser::clearHistory() {
    commandHistory.clear();
}