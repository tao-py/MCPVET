#ifndef LANGUAGE_MANAGER_H
#define LANGUAGE_MANAGER_H

#include <string>
#include <unordered_map>

enum class Language {
    ENGLISH,
    CHINESE
};

class LanguageManager {
private:
    Language currentLanguage;
    std::unordered_map<std::string, std::unordered_map<Language, std::string>> translations;

    void initializeTranslations();
    void loadTranslationsFromJson(const std::string& filename); // 声明函数但不暴露json实现

public:
    LanguageManager();
    LanguageManager(const std::string& jsonFilename);
    
    void setLanguage(Language lang);
    Language getCurrentLanguage() const { return currentLanguage; }
    
    std::string getText(const std::string& key) const;
    
    // 获取语言名称
    std::string getLanguageName(Language lang) const;
};

#endif // LANGUAGE_MANAGER_H