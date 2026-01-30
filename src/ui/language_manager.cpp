#include "language_manager.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

LanguageManager::LanguageManager() : currentLanguage(Language::CHINESE) {
    initializeTranslations();
}

LanguageManager::LanguageManager(const std::string& jsonFilename) : currentLanguage(Language::CHINESE) {
    loadTranslationsFromJson(jsonFilename);
}

void LanguageManager::initializeTranslations() {
    // 主菜单
    translations["File"][Language::ENGLISH] = "File";
    translations["File"][Language::CHINESE] = "文件";
    
    translations["New"][Language::ENGLISH] = "New";
    translations["New"][Language::CHINESE] = "新建";
    
    translations["Open"][Language::ENGLISH] = "Open";
    translations["Open"][Language::CHINESE] = "打开";
    
    translations["Save"][Language::ENGLISH] = "Save";
    translations["Save"][Language::CHINESE] = "保存";
    
    translations["SaveAs"][Language::ENGLISH] = "Save As";
    translations["SaveAs"][Language::CHINESE] = "另存为";
    
    translations["Exit"][Language::ENGLISH] = "Exit";
    translations["Exit"][Language::CHINESE] = "退出";
    
    translations["Create"][Language::ENGLISH] = "Create";
    translations["Create"][Language::CHINESE] = "创建";
    
    translations["Cube"][Language::ENGLISH] = "Cube";
    translations["Cube"][Language::CHINESE] = "立方体";
    
    translations["Sphere"][Language::ENGLISH] = "Sphere";
    translations["Sphere"][Language::CHINESE] = "球体";
    
    translations["Cylinder"][Language::ENGLISH] = "Cylinder";
    translations["Cylinder"][Language::CHINESE] = "圆柱体";
    
    translations["Plane"][Language::ENGLISH] = "Plane";
    translations["Plane"][Language::CHINESE] = "平面";
    
    translations["Line"][Language::ENGLISH] = "Line";
    translations["Line"][Language::CHINESE] = "线段";
    
    translations["Point"][Language::ENGLISH] = "Point";
    translations["Point"][Language::CHINESE] = "点";
    
    translations["Edit"][Language::ENGLISH] = "Edit";
    translations["Edit"][Language::CHINESE] = "编辑";
    
    translations["Undo"][Language::ENGLISH] = "Undo";
    translations["Undo"][Language::CHINESE] = "撤销";
    
    translations["Redo"][Language::ENGLISH] = "Redo";
    translations["Redo"][Language::CHINESE] = "重做";
    
    translations["Delete"][Language::ENGLISH] = "Delete";
    translations["Delete"][Language::CHINESE] = "删除";
    
    translations["Boolean"][Language::ENGLISH] = "Boolean";
    translations["Boolean"][Language::CHINESE] = "布尔运算";
    
    translations["Union"][Language::ENGLISH] = "Union";
    translations["Union"][Language::CHINESE] = "并集";
    
    translations["Difference"][Language::ENGLISH] = "Difference";
    translations["Difference"][Language::CHINESE] = "差集";
    
    translations["Intersection"][Language::ENGLISH] = "Intersection";
    translations["Intersection"][Language::CHINESE] = "交集";
    
    translations["Settings"][Language::ENGLISH] = "Settings";
    translations["Settings"][Language::CHINESE] = "设置";
    
    translations["KeyBindings"][Language::ENGLISH] = "Key Bindings";
    translations["KeyBindings"][Language::CHINESE] = "快捷键";
    
    translations["UISettings"][Language::ENGLISH] = "UI Settings";
    translations["UISettings"][Language::CHINESE] = "界面设置";
    
    translations["Language"][Language::ENGLISH] = "Language";
    translations["Language"][Language::CHINESE] = "语言";
    
    // 界面文本
    translations["ViewOptions"][Language::ENGLISH] = "View Options";
    translations["ViewOptions"][Language::CHINESE] = "视图选项";
    
    translations["ShowGrid"][Language::ENGLISH] = "Show Grid";
    translations["ShowGrid"][Language::CHINESE] = "显示网格";
    
    translations["GridColor"][Language::ENGLISH] = "Grid Color";
    translations["GridColor"][Language::CHINESE] = "网格颜色";
    
    translations["Camera"][Language::ENGLISH] = "Camera";
    translations["Camera"][Language::CHINESE] = "相机";
    
    translations["Distance"][Language::ENGLISH] = "Distance";
    translations["Distance"][Language::CHINESE] = "距离";
    
    translations["AngleX"][Language::ENGLISH] = "Angle X";
    translations["AngleX"][Language::CHINESE] = "X角度";
    
    translations["AngleY"][Language::ENGLISH] = "Angle Y";
    translations["AngleY"][Language::CHINESE] = "Y角度";
    
    translations["Performance"][Language::ENGLISH] = "Performance";
    translations["Performance"][Language::CHINESE] = "性能";
    
    translations["FPSLimit"][Language::ENGLISH] = "FPS Limit";
    translations["FPSLimit"][Language::CHINESE] = "帧率限制";
    
    translations["UIFont"][Language::ENGLISH] = "UI Font";
    translations["UIFont"][Language::CHINESE] = "界面字体";
    
    translations["FontSize"][Language::ENGLISH] = "Font Size";
    translations["FontSize"][Language::CHINESE] = "字体大小";
    
    translations["Objects"][Language::ENGLISH] = "Objects";
    translations["Objects"][Language::CHINESE] = "对象";
    
    translations["AddCube"][Language::ENGLISH] = "Add Cube";
    translations["AddCube"][Language::CHINESE] = "添加立方体";
    
    translations["AddSphere"][Language::ENGLISH] = "Add Sphere";
    translations["AddSphere"][Language::CHINESE] = "添加球体";
    
    translations["AddCylinder"][Language::ENGLISH] = "Add Cylinder";
    translations["AddCylinder"][Language::CHINESE] = "添加圆柱体";
    
    translations["AddPlane"][Language::ENGLISH] = "Add Plane";
    translations["AddPlane"][Language::CHINESE] = "添加平面";
    
    translations["AddPoint"][Language::ENGLISH] = "Add Point";
    translations["AddPoint"][Language::CHINESE] = "添加点";
    
    translations["AddLine"][Language::ENGLISH] = "Add Line";
    translations["AddLine"][Language::CHINESE] = "添加线段";
    
    translations["ObjectProperties"][Language::ENGLISH] = "Object Properties";
    translations["ObjectProperties"][Language::CHINESE] = "对象属性";
    
    translations["Name"][Language::ENGLISH] = "Name";
    translations["Name"][Language::CHINESE] = "名称";
    
    translations["Transform"][Language::ENGLISH] = "Transform";
    translations["Transform"][Language::CHINESE] = "变换";
    
    translations["Position"][Language::ENGLISH] = "Position";
    translations["Position"][Language::CHINESE] = "位置";
    
    translations["Rotation"][Language::ENGLISH] = "Rotation";
    translations["Rotation"][Language::CHINESE] = "旋转";
    
    translations["Scale"][Language::ENGLISH] = "Scale";
    translations["Scale"][Language::CHINESE] = "缩放";
    
    translations["Color"][Language::ENGLISH] = "Color";
    translations["Color"][Language::CHINESE] = "颜色";
    
    translations["DeleteObject"][Language::ENGLISH] = "Delete Object";
    translations["DeleteObject"][Language::CHINESE] = "删除对象";
    
    translations["SelectedObject"][Language::ENGLISH] = "Selected Object";
    translations["SelectedObject"][Language::CHINESE] = "选中对象";
    
    translations["MaterialColor"][Language::ENGLISH] = "Material Color";
    translations["MaterialColor"][Language::CHINESE] = "材质颜色";
    
    translations["EnterCommand"][Language::ENGLISH] = "Enter command (e.g., translate, scale, rotate): ";
    translations["EnterCommand"][Language::CHINESE] = "输入命令 (例如: 平移, 缩放, 旋转): ";
    
    translations["Execute"][Language::ENGLISH] = "Execute";
    translations["Execute"][Language::CHINESE] = "执行";
    
    translations["Clear"][Language::ENGLISH] = "Clear";
    translations["Clear"][Language::CHINESE] = "清除";
    
    translations["RecentCommands"][Language::ENGLISH] = "Recent commands:";
    translations["RecentCommands"][Language::CHINESE] = "最近命令:";
    
    translations["3DModelingTool"][Language::ENGLISH] = "3D Modeling Tool";
    translations["3DModelingTool"][Language::CHINESE] = "3D建模工具";
    
    translations["ObjectsCount"][Language::ENGLISH] = " objects | ";
    translations["ObjectsCount"][Language::CHINESE] = " 个对象 | ";
}

void LanguageManager::setLanguage(Language lang) {
    currentLanguage = lang;
}

void LanguageManager::loadTranslationsFromJson(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open language file: " << filename << std::endl;
        initializeTranslations(); // 使用默认翻译
        return;
    }
    
    nlohmann::json jsonData;
    file >> jsonData;
    file.close();
    
    // 清空现有翻译
    translations.clear();
    
    // 从JSON数据加载翻译
    if (jsonData.contains("translations")) {
        for (auto& [key, value] : jsonData["translations"].items()) {
            if (value.contains("ENGLISH")) {
                translations[key][Language::ENGLISH] = value["ENGLISH"].get<std::string>();
            }
            if (value.contains("CHINESE")) {
                translations[key][Language::CHINESE] = value["CHINESE"].get<std::string>();
            }
        }
    }
}

std::string LanguageManager::getText(const std::string& key) const {
    auto it = translations.find(key);
    if (it != translations.end()) {
        auto lang_it = it->second.find(currentLanguage);
        if (lang_it != it->second.end()) {
            return lang_it->second;
        }
    }
    // 如果找不到对应语言的文本，返回英文版本
    auto it_en = translations.find(key);
    if (it_en != translations.end()) {
        auto en_it = it_en->second.find(Language::ENGLISH);
        if (en_it != it_en->second.end()) {
            return en_it->second;
        }
    }
    // 如果连英文版本都没有，返回key本身
    return key;
}

std::string LanguageManager::getLanguageName(Language lang) const {
    switch (lang) {
        case Language::ENGLISH:
            return "English";
        case Language::CHINESE:
            return "中文";
        default:
            return "Unknown";
    }
}