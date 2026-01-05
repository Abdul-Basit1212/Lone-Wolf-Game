#include "GameData.h"   // Matches the header file above
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <algorithm> // Required for std::min

// Forward declarations
void SetupImGuiStyle();
void RenderGameUI(WolfStats& stats,
                  std::vector<Item>& inventory,
                  int& currentScenarioID,
                  bool& showInventory,
                  bool& showStatsDetails,
                  bool& running);

int main()
{
    // ----------------------------------
    // 1. GLFW Window + OpenGL Setup
    // ----------------------------------
    if (!glfwInit())
        return -1;

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Wolf Survival Game", NULL, NULL);
    if (window == NULL)
        return -1;

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // ----------------------------------
    // 2. ImGui Setup
    // ----------------------------------
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    SetupImGuiStyle();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // ----------------------------------
    // 3. Game Data (Initial State)
    // ----------------------------------
    WolfStats stats;                // Default values from GameData.h
    std::vector<Item> inventory;    // Empty inventory
    
    // FIX 1: Corrected Item Initialization
    // Constructor requires: Name, Type, Effect, Quantity
    inventory.push_back(Item("Raw Meat", FOOD, 15, 2));

    int currentScenarioID = 0;
    bool showInventory = false;
    bool showStatsDetails = false;
    bool running = true;

    // ----------------------------------
    // 4. Main Loop
    // ----------------------------------
    while (!glfwWindowShouldClose(window) && running)
    {
        // Poll events
        glfwPollEvents();

        // Start new ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Render the Game UI
        RenderGameUI(stats, inventory, currentScenarioID, showInventory, showStatsDetails, running);

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        
        // Background color
        glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

void SetupImGuiStyle()
{
    ImGui::StyleColorsDark();
    // Optional: Customize style here (rounding, colors, etc.)
}

void RenderGameUI(WolfStats& stats,
                  std::vector<Item>& inventory,
                  int& currentScenarioID,
                  bool& showInventory,
                  bool& showStatsDetails,
                  bool& running)
{
    // Make the main game window fill the entire OS window
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Always);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize |
                                    ImGuiWindowFlags_NoCollapse |
                                    ImGuiWindowFlags_NoMove |
                                    ImGuiWindowFlags_NoBringToFrontOnFocus |
                                    ImGuiWindowFlags_NoTitleBar;

    if (ImGui::Begin("Wolf Survival Game", nullptr, window_flags))
    {
        // Columns: Main Content (Left) | Side Panel (Right)
        ImGui::Columns(2, "MainAndSideColumns", true);

        // ==========================================
        // LEFT COLUMN
        // ==========================================
        
        // --- Status Panel ---
        ImGui::TextDisabled("STATUS");
        ImGui::Separator();
        
        float healthNorm = stats.health / 100.0f;
        float hungerNorm = stats.hunger / 100.0f;
        float energyNorm = stats.energy / 100.0f;

        ImGui::Text("Health"); ImGui::SameLine(60); 
        ImGui::ProgressBar(healthNorm, ImVec2(-1, 0), (std::to_string(stats.health) + "/100").c_str());

        ImGui::Text("Hunger"); ImGui::SameLine(60); 
        ImGui::ProgressBar(hungerNorm, ImVec2(-1, 0), (std::to_string(stats.hunger) + "/100").c_str());

        ImGui::Text("Energy"); ImGui::SameLine(60); 
        ImGui::ProgressBar(energyNorm, ImVec2(-1, 0), (std::to_string(stats.energy) + "/100").c_str());

        ImGui::Spacing();
        ImGui::Text("Day: %d  |  Weather: Clear  |  Time: Morning", stats.dayCount);
        ImGui::Separator();

        // --- Story Display ---
        ImGui::Spacing();
        ImGui::TextDisabled("NARRATIVE");
        
        ImGui::BeginChild("StoryRegion", ImVec2(0, 200), true);
        ImGui::TextWrapped("Scenario %d: You wake up in a dense forest. The air is cold, and your stomach growls. The pack is nowhere to be seen.", currentScenarioID);
        ImGui::EndChild();

        // --- Choices ---
        ImGui::Spacing();
        ImGui::TextDisabled("ACTIONS");
        
        if (ImGui::Button("Hunt for food (Cost: 20 Energy)", ImVec2(-1, 40))) {
            if (stats.energy >= 20) {
                stats.energy -= 20;
                stats.hunger = std::min(100, stats.hunger + 30);
                
                // FIX 2: Corrected Item Initialization
                inventory.push_back(Item("Bone", TOOL, 0, 1));
            }
        }
        
        if (ImGui::Button("Rest (Restores Energy)", ImVec2(-1, 40))) {
            stats.energy = std::min(100, stats.energy + 50);
            stats.dayCount++;
        }

        // --- Controls ---
        ImGui::Spacing();
        ImGui::Separator();
        if (ImGui::Button("Inventory")) showInventory = !showInventory;
        ImGui::SameLine();
        if (ImGui::Button("Stats")) showStatsDetails = !showStatsDetails;
        ImGui::SameLine();
        if (ImGui::Button("Quit Game")) running = false;

        ImGui::NextColumn();

        // ==========================================
        // RIGHT COLUMN
        // ==========================================
        ImGui::TextDisabled("LOG");
        ImGui::Separator();
        ImGui::BeginChild("LogRegion", ImVec2(0, 0), false);
        ImGui::TextWrapped("> Game Started.");
        ImGui::TextWrapped("> You feel hungry.");
        ImGui::EndChild();
    }
    ImGui::End();

    // ==========================================
    // POPUPS
    // ==========================================
    if (showInventory) {
        ImGui::Begin("Inventory", &showInventory);
        if (inventory.empty()) ImGui::Text("Empty.");
        for (auto& item : inventory) {
            ImGui::Text("- %s (x%d)", item.name.c_str(), item.quantity);
        }
        ImGui::End();
    }

    if (showStatsDetails) {
        ImGui::Begin("Details", &showStatsDetails);
        ImGui::Text("Reputation: %d", stats.reputation);
        ImGui::End();
    }
}