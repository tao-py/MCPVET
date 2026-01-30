#pragma once
#include "../MWindows.h"
#include "../../io/config_manager.h"
#include "../../io/command_parser.h"
#include "../../core/log_manager.h"

namespace mcnp::ui {

class BottomBarWindow final : public MWindows {
public:
    BottomBarWindow()
        : MWindows("Console / Status")
    {
        SetClosable(false);
        SetFlags(ImGuiWindowFlags_NoCollapse);
    }

    void SetHeight(float h) noexcept { height_ = h; }
    float Height() const noexcept { return height_; }

private:
    void OnDraw() override
    {
        height_ = ImGui::GetWindowSize().y;

        ImGui::Text("Status: %d objects | %.1f FPS", (int)meshes.size(), ImGui::GetIO().Framerate);
        if (selectedMesh >= 0 && selectedMesh < (int)meshes.size()) {
            ImGui::SameLine();
            ImGui::Text("| Selected: %s", meshes[selectedMesh].name.c_str());
        }
        ImGui::Separator();

        static CommandParser commandParser;
        static char cmd[256] = {};
        bool submitted = ImGui::InputText("Command", cmd, sizeof(cmd), ImGuiInputTextFlags_EnterReturnsTrue);
        ImGui::SameLine();
        if (ImGui::Button("Run") || submitted) {
            if (cmd[0] != '\0') {
                if (commandParser.parseAndExecute(std::string(cmd), meshes)) {
                    LogManager::getInstance()->logOperation("Command", std::string("Executed: ") + cmd);
                } else {
                    LogManager::getInstance()->logWarning(std::string("Invalid command: ") + cmd);
                }
                cmd[0] = '\0';
            }
        }
    }

    float height_{180.0f};
};

} // namespace mcnp::ui
