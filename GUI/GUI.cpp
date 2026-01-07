#include "GameData.h"
#include "GameEngine.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <vector>
#include <string>

ImVec4 GetStatusColor(float val, float max, bool inverted = false) {
    float ratio = val / max;
    if (inverted) ratio = 1.0f - ratio; 
    if (ratio < 0.3f) return ImVec4(0.8f, 0.2f, 0.2f, 1.0f); 
    if (ratio < 0.6f) return ImVec4(0.9f, 0.7f, 0.1f, 1.0f); 
    return ImVec4(0.2f, 0.8f, 0.2f, 1.0f); 
}

void SetupImGuiStyle() { ImGui::StyleColorsDark(); }

void RenderGameUI(GameEngine& engine, bool& showInventory, bool& showStatsDetails, bool& running) {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.05f, 0.05f, 0.08f, 1.0f));

    if (ImGui::Begin("Wolf Survival", nullptr, ImGuiWindowFlags_NoDecoration)) {
        ImGui::Columns(2, "MainLayout", true);
        static bool init = true; if(init){ ImGui::SetColumnWidth(0, 320); init = false; }

        ImGui::TextDisabled("--- STATUS ---");
        WolfStats& stats = engine.getStats();
        
        ImGui::Spacing(); ImGui::Text("Health");
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, GetStatusColor(stats.health, 100));
        ImGui::ProgressBar(stats.health/100.0f, ImVec2(-1, 0), std::to_string(stats.health).c_str());
        ImGui::PopStyleColor();

        ImGui::Spacing(); ImGui::Text("Hunger");
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, GetStatusColor(stats.hunger, 100, true));
        ImGui::ProgressBar(stats.hunger/100.0f, ImVec2(-1, 0), std::to_string(stats.hunger).c_str());
        ImGui::PopStyleColor();

        ImGui::Spacing(); ImGui::Text("Energy");
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.1f, 0.5f, 0.9f, 1));
        ImGui::ProgressBar(stats.energy/100.0f, ImVec2(-1, 0), std::to_string(stats.energy).c_str());
        ImGui::PopStyleColor();

        ImGui::Spacing(); ImGui::Text("Reputation");
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.9f, 0.7f, 0.0f, 1));
        ImGui::ProgressBar(stats.reputation/100.0f, ImVec2(-1, 0), std::to_string(stats.reputation).c_str());
        ImGui::PopStyleColor();

        ImGui::Separator();
        ImGui::Text("Day: %d", stats.dayCount);
        ImGui::Text("Rank: %s", engine.getFinalTitle().c_str());

        ImGui::Spacing();
        if(ImGui::Button("BACKPACK", ImVec2(-1, 40))) showInventory = !showInventory;

        ImGui::NextColumn();

        StoryNode* current = engine.getCurrentNode();
        
        // Draw Image
        if (current && !current->imagePath.empty()) {
            unsigned int texID = engine.getNodeTexture(current->imagePath);
            if (texID != 0) {
                float w = ImGui::GetContentRegionAvail().x;
                ImGui::Image((void*)(intptr_t)texID, ImVec2(w, 350)); 
            }
        }

        // Draw Story
        ImGui::Spacing();
        ImGui::BeginChild("StoryTxt", ImVec2(0, 150), true);
        if(engine.isGameEnded()) {
            ImGui::TextColored(engine.isVictory()?ImVec4(0,1,0,1):ImVec4(1,0,0,1), 
                engine.isVictory()?"VICTORY":"GAME OVER");
        }
        if(current) ImGui::TextWrapped("%s", current->text.c_str());
        ImGui::EndChild();

        // Draw Buttons
        ImGui::Spacing();
        if (!engine.isGameEnded() && current) {
            for (int i=0; i<current->children.size(); i++) {
                ImGui::PushID(i);
                if (ImGui::Button(current->children[i].first.c_str(), ImVec2(-1, 50))) {
                    engine.processChoice(i);
                }
                ImGui::PopID();
                ImGui::Spacing();
            }
        } else {
            if (ImGui::Button("RESTART", ImVec2(-1, 50))) engine.initGame();
        }

        ImGui::Separator();
        ImGui::BeginChild("Log", ImVec2(0,0));
        for(auto& s : engine.getGameLog()) ImGui::TextWrapped("> %s", s.c_str());
        if(ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) ImGui::SetScrollHereY(1.0f);
        ImGui::EndChild();
    }
    ImGui::End();
    ImGui::PopStyleColor();

    if (showInventory) {
        ImGui::Begin("Pack", &showInventory);
        for(auto& i : engine.getInventory()) ImGui::Text("%s x%d", i.name.c_str(), i.quantity);
        ImGui::End();
    }
}

int main() {
    if (!glfwInit()) return -1;
    GLFWwindow* w = glfwCreateWindow(1280, 720, "Wolf Survival", NULL, NULL);
    glfwMakeContextCurrent(w);
    glfwSwapInterval(1);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(w, true);
    ImGui_ImplOpenGL3_Init("#version 130");
    SetupImGuiStyle();
    
    GameEngine game; game.initGame();
    bool inv=false, stat=false, run=true;
    while(!glfwWindowShouldClose(w) && run) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame(); ImGui_ImplGlfw_NewFrame(); ImGui::NewFrame();
        RenderGameUI(game, inv, stat, run);
        ImGui::Render();
        int width, height; glfwGetFramebufferSize(w, &width, &height);
        glViewport(0, 0, width, height);
        glClearColor(0,0,0,1); glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(w);
    }
    ImGui_ImplOpenGL3_Shutdown(); ImGui_ImplGlfw_Shutdown(); ImGui::DestroyContext();
    glfwDestroyWindow(w); glfwTerminate();
    return 0;
}                                  
