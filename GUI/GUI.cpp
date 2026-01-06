#include "GameData.h"
#include "GameEngine.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <vector>
#include <string>

void SetupImGuiStyle() { ImGui::StyleColorsDark(); }

void RenderGameUI(GameEngine& engine, bool& showInventory, bool& showStatsDetails, bool& running)
{
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Always);

    if (ImGui::Begin("Wolf Survival Game", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar))
    {
        ImGui::Columns(2, "MainAndSideColumns", true);

        // ==========================================
        // LEFT COLUMN: STATS
        // ==========================================
        ImGui::TextDisabled("STATUS");
        ImGui::Separator();
        
        WolfStats& stats = engine.getStats(); 

        ImGui::Text("Health"); ImGui::SameLine(80); 
        ImGui::ProgressBar(stats.health / 100.0f, ImVec2(-1, 0), (std::to_string(stats.health)).c_str());

        // FIXED: Hunger Bar works now
        ImGui::Text("Hunger"); ImGui::SameLine(80); 
        ImGui::ProgressBar(stats.hunger / 100.0f, ImVec2(-1, 0), (std::to_string(stats.hunger)).c_str());

        ImGui::Text("Energy"); ImGui::SameLine(80); 
        ImGui::ProgressBar(stats.energy / 100.0f, ImVec2(-1, 0), (std::to_string(stats.energy)).c_str());

        // FIXED: Added Reputation Bar
        ImGui::Text("Reputation"); ImGui::SameLine(80);
        ImGui::ProgressBar(stats.reputation / 100.0f, ImVec2(-1, 0), (std::to_string(stats.reputation)).c_str());

        ImGui::Spacing();
        ImGui::Text("Day: %d", stats.dayCount); 
        ImGui::Separator();

        // ==========================================
        // STORY REGION
        // ==========================================
        ImGui::Spacing();
        ImGui::TextDisabled("STORY");
        ImGui::BeginChild("StoryRegion", ImVec2(0, 250), true);
        
        // --- NEW END SCREEN LOGIC ---
        if (engine.isGameEnded()) {
            ImGui::SetWindowFontScale(1.2f);
            if (engine.isVictory()) {
                ImGui::TextColored(ImVec4(0, 1, 0, 1), "VICTORY!");
                ImGui::TextWrapped("%s", engine.getCurrentNode()->text.c_str());
            } else {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "GAME OVER");
                ImGui::TextWrapped("Your journey has ended in the cold snow.");
            }
            ImGui::SetWindowFontScale(1.0f);
            
            ImGui::Separator();
            ImGui::Text("FINAL RESULTS:");
            ImGui::Text("Title Earned: %s", engine.getFinalTitle().c_str());
            ImGui::Text("Final Reputation: %d", stats.reputation);
            ImGui::Text("Days Survived: %d", stats.dayCount);
        } 
        else {
            // Normal Game State
            StoryNode* current = engine.getCurrentNode();
            if (current) ImGui::TextWrapped("%s", current->text.c_str());
        }
        
        ImGui::EndChild();

        // ==========================================
        // ACTIONS
        // ==========================================
        ImGui::Spacing();
        ImGui::TextDisabled("ACTIONS");
        
        if (!engine.isGameEnded()) {
            StoryNode* current = engine.getCurrentNode();
            if (current) {
                auto& options = current->children;
                for (int i = 0; i < options.size(); ++i) {
                    ImGui::PushID(i); 
                    if (ImGui::Button(options[i].first.c_str(), ImVec2(-1, 40))) {
                        engine.processChoice(i); 
                    }
                    ImGui::PopID();
                }
            }
        } else {
            // End Game Buttons
            if (ImGui::Button("Play Again", ImVec2(-1, 40))) {
                engine.initGame();
            }
            if (ImGui::Button("Quit", ImVec2(-1, 40))) {
                running = false;
            }
        }

        // ==========================================
        // RIGHT COLUMN (LOGS)
        // ==========================================
        ImGui::NextColumn();
        ImGui::TextDisabled("GAME LOG");
        ImGui::Separator();
        ImGui::BeginChild("LogRegion", ImVec2(0, 0), false);
        const std::vector<std::string>& logs = engine.getGameLog();
        for (const auto& logEntry : logs) ImGui::TextWrapped("> %s", logEntry.c_str());
        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) ImGui::SetScrollHereY(1.0f);
        ImGui::EndChild();
    }
    ImGui::End();

    // Inventory Popup logic
    if (showInventory) {
        ImGui::Begin("Inventory", &showInventory, ImGuiWindowFlags_AlwaysAutoResize);
        const std::vector<Item>& inv = engine.getInventory();
        if (inv.empty()) ImGui::Text("Empty.");
        else for (const auto& item : inv) ImGui::Text("- %s (%d)", item.name.c_str(), item.quantity);
        ImGui::End();
    }
}

// Main Function Boilerplate
int main() {
    if (!glfwInit()) return -1;
    const char* glsl_version = "#version 130";
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Wolf Survival", NULL, NULL);
    if (!window) return -1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    SetupImGuiStyle();

    GameEngine game;
    game.initGame();

    bool showInv = false, showStats = false, run = true;
    while (!glfwWindowShouldClose(window) && run) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        RenderGameUI(game, showInv, showStats, run);
        ImGui::Render();
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);
        glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
