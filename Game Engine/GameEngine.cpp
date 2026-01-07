#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "GameEngine.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <GLFW/glfw3.h> // Includes GL.h

// FIX 1: Manually define this constant if Windows is missing it
#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

// =========================================================
// TEXTURE LOADING
// =========================================================
unsigned int GameEngine::getNodeTexture(std::string path) {
    if (path.empty()) return 0;
    if (textureCache.find(path) != textureCache.end()) return textureCache[path];
    unsigned int texID = loadTextureFromFile(path.c_str());
    textureCache[path] = texID;
    return texID;
}

unsigned int GameEngine::loadTextureFromFile(const char* filename) {
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (!data) return 0; 
    
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    // FIX 2: Simplified Texture Parameters (Removed Mipmaps to fix build errors)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    
    // FIX 3: Removed glGenerateMipmap() because it requires modern OpenGL extensions
    // glGenerateMipmap(GL_TEXTURE_2D); 

    stbi_image_free(data);
    return textureID;
}

// =========================================================
// INITIALIZATION
// =========================================================
void GameEngine::initGame() {
    currentStats = {100, 0, 100, 0, 1}; 
    inventory.clear(); 
    storyMap.clear();
    gameLog.clear();
    gameLog.push_back("--- NEW GAME STARTED ---");
    gameOver = false;
    gameWon = false;
    textureCache.clear();

    // --- LEVEL 1 ---
    addNode(1, "LEVEL 1: Frozen Awakening\nAlex wakes alone in the snow. The pack is gone.", 0, 0, 0, 0, 0);
    addNode(201, "Shelter.", +5, -5, +5, 0, 1);
    addNode(202, "Search food.", 0, -10, +10, 0, 1, "Energy>10", "Meat"); 
    connect(1, "Shelter", 201); connect(1, "Search", 202);

    // --- LEVEL 2 ---
    addNode(2, "LEVEL 2: Echoes in the Snow", 0, 0, 0, 0, 0);
    connect(201, "Continue", 2); connect(202, "Continue", 2);
    addNode(301, "Move carefully.", 0, -4, +5, +2, 0);
    addNode(302, "Move fast.", 0, -8, +8, 0, 0); 
    connect(2, "Careful", 301); connect(2, "Fast", 302);

    // --- LEVEL 3 ---
    addNode(3, "LEVEL 3: The First Hunger", 0, 0, 0, 0, 1);
    connect(301, "Continue", 3); connect(302, "Continue", 3);
    addNode(401, "Hunt.", 0, -8, +10, 0, 0, "Energy>8", "Meat");
    addNode(402, "Forage.", 0, -4, +5, 0, 0, "None", "Herbs");
    connect(3, "Hunt", 401); connect(3, "Forage", 402);

    // --- LEVEL 4 ---
    addNode(4, "LEVEL 4: Silent Trees", 0, 0, 0, 0, 1);
    connect(401, "Continue", 4); connect(402, "Continue", 4);
    addNode(501, "Hide.", +2, -3, +5, 0, 0);
    addNode(502, "Escape!", -4, -7, +10, 0, 0);
    connect(4, "Hide", 501); connect(4, "Escape", 502);

    // --- LEVEL 5 ---
    addNode(5, "LEVEL 5: First Blood", 0, 0, 0, 0, 0);
    connect(501, "Continue", 5); connect(502, "Continue", 5);
    addNode(601, "Eat meat.", +10, +5, -30, 0, 0, "Meat"); 
    addNode(602, "Save it.", 0, 0, +5, 0, 0);
    connect(5, "Eat", 601); connect(5, "Save", 602);

    // --- LEVEL 6 ---
    addNode(6, "LEVEL 6: Cold Night", 0, 0, 0, 0, 1);
    connect(601, "Continue", 6); connect(602, "Continue", 6);
    addNode(701, "Sleep.", 0, +15, +10, 0, 0);
    addNode(702, "Alert.", 0, -6, +5, +3, 0);
    connect(6, "Sleep", 701); connect(6, "Alert", 702);

    // --- LEVEL 7 ---
    addNode(7, "LEVEL 7: Distant Howl", 0, 0, 0, 0, 1);
    connect(701, "Continue", 7); connect(702, "Continue", 7);
    addNode(801, "Follow.", 0, -5, +5, +8, 0);
    addNode(802, "Ignore.", 0, 0, +5, -5, 0);
    connect(7, "Follow", 801); connect(7, "Ignore", 802);

    // --- LEVEL 8 ---
    addNode(8, "LEVEL 8: Hidden Claws", 0, 0, 0, 0, 0);
    connect(801, "Continue", 8); connect(802, "Continue", 8);
    addNode(901, "Fight!", -5, -8, +5, 0, 0);
    addNode(902, "Escape.", -10, -5, +5, 0, 0);
    connect(8, "Fight", 901); connect(8, "Escape", 902);

    // --- LEVEL 9 ---
    addNode(9, "LEVEL 9: Bleeding Path", 0, 0, 0, 0, 1);
    connect(901, "Continue", 9); connect(902, "Continue", 9);
    addNode(1001, "Heal.", +20, 0, 0, 0, 0, "Herbs"); 
    addNode(1002, "Ignore.", -5, 0, +5, 0, 0);
    connect(9, "Heal", 1001); connect(9, "Ignore", 1002);

    // --- LEVEL 10 ---
    addNode(10, "LEVEL 10: The Bear Trial", 0, 0, 0, 0, 0);
    connect(1001, "Continue", 10); connect(1002, "Continue", 10);
    addNode(1101, "Victory!", -25, -20, +10, +15, 0, "Energy>20", "Bone");
    addNode(1102, "Defeat.", -100, 0, 0, 0, 0);
    connect(10, "Fight", 1101); connect(10, "Run", 1102);

    // --- LEVEL 11 ---
    addNode(11, "LEVEL 11: Frozen River", 0, 0, 0, 0, 1);
    connect(1101, "Continue", 11);
    addNode(1201, "Cross.", 0, -8, +5, 0, 0);
    addNode(1202, "Bank.", 0, -5, +5, 0, 0, "None", "Herbs");
    connect(11, "Cross", 1201); connect(11, "Bank", 1202);

    // --- LEVEL 12 ---
    addNode(12, "LEVEL 12: Lonely Stars", 0, 0, 0, 0, 1);
    connect(1201, "Continue", 12); connect(1202, "Continue", 12);
    addNode(1301, "Rest.", 0, +10, +5, 0, 1);
    addNode(1302, "Move.", 0, -5, +5, 0, 1);
    connect(12, "Rest", 1301); connect(12, "Move", 1302);

    // --- LEVEL 13 ---
    addNode(13, "LEVEL 13: Human Scent", 0, 0, 0, 0, 0);
    connect(1301, "Continue", 13); connect(1302, "Continue", 13);
    addNode(1401, "Hide.", 0, -2, +5, +5, 0);
    addNode(1402, "Flee.", -5, -8, +8, 0, 0);
    connect(13, "Hide", 1401); connect(13, "Flee", 1402);

    // --- LEVEL 14 ---
    addNode(14, "LEVEL 14: Strength", 0, 0, 0, 0, 1);
    connect(1401, "Continue", 14); connect(1402, "Continue", 14);
    addNode(1501, "Train.", +5, -10, +10, 0, 0);
    addNode(1502, "Explore.", 0, -5, +5, 0, 0, "None", "TerritoryMark");
    connect(14, "Train", 1501); connect(14, "Explore", 1502);

    // --- LEVEL 15 ---
    addNode(15, "LEVEL 15: Marking Land", 0, 0, 0, 0, 1);
    connect(1501, "Continue", 15); connect(1502, "Continue", 15);
    addNode(1601, "Mark.", 0, -5, +5, +10, 0, "None", "TerritoryMark");
    addNode(1602, "Observe.", 0, 0, +5, 0, 0);
    connect(15, "Mark", 1601); connect(15, "Observe", 1602);

    // --- LEVEL 16 ---
    addNode(16, "LEVEL 16: Silverpaw Borders", 0, 0, 0, 0, 1);
    connect(1601, "Continue", 16); connect(1602, "Continue", 16);
    addNode(1701, "Approach.", 0, 0, +5, +10, 0);
    addNode(1702, "Avoid.", 0, -4, +5, 0, 0);
    connect(16, "Approach", 1701); connect(16, "Avoid", 1702);

    // --- LEVEL 17 ---
    addNode(17, "LEVEL 17: Trust or Fear", 0, 0, 0, 0, 0);
    connect(1701, "Continue", 17); connect(1702, "Continue", 17);
    addNode(1801, "Trust.", 0, 0, +5, +8, 0);
    addNode(1802, "Alone.", 0, 0, +5, -5, 0);
    connect(17, "Trust", 1801); connect(17, "Alone", 1802);

    // --- LEVEL 18 ---
    addNode(18, "LEVEL 18: Old Truths", 0, 0, 0, 0, 1);
    connect(1801, "Continue", 18); connect(1802, "Continue", 18);
    addNode(1901, "Revenge.", 0, -5, +5, +5, 0);
    addNode(1902, "Peace.", 0, 0, +5, +10, 0);
    connect(18, "Revenge", 1901); connect(18, "Peace", 1902);

    // --- LEVEL 19 ---
    addNode(19, "LEVEL 19: Preparing War", 0, 0, 0, 0, 1);
    connect(1901, "Continue", 19); connect(1902, "Continue", 19);
    addNode(2001, "Weapons.", 0, -5, +5, 0, 0, "None", "Bone");
    addNode(2002, "Rest.", 0, +10, +5, 0, 0);
    connect(19, "Weapons", 2001); connect(19, "Rest", 2002);

    // --- LEVEL 20 ---
    addNode(20, "LEVEL 20: Rival Alpha", 0, 0, 0, 0, 0);
    connect(2001, "Continue", 20); connect(2002, "Continue", 20);
    addNode(2101, "Win Duel!", -15, -15, +10, +20, 0, "Rep>30", "None");
    addNode(2102, "Defeat.", -100, 0, 0, 0, 0);
    connect(20, "Duel", 2101); connect(20, "Surrender", 2102);

    // --- LEVEL 21 ---
    addNode(21, "LEVEL 21: Scars", 0, 0, 0, 0, 1);
    connect(2101, "Continue", 21);
    addNode(2201, "Heal.", +20, 0, 0, 0, 0, "Herbs");
    addNode(2202, "Push on.", -5, -5, +5, 0, 0);
    connect(21, "Heal", 2201); connect(21, "Push", 2202);

    // --- LEVEL 22 ---
    addNode(22, "LEVEL 22: Pack Call", 0, 0, 0, 0, 1);
    connect(2201, "Continue", 22); connect(2202, "Continue", 22);
    addNode(2301, "Recruit.", 0, -5, +5, +10, 0, "Rep>40", "PackMember");
    addNode(2302, "Alone.", 0, 0, +5, 0, 0);
    connect(22, "Recruit", 2301); connect(22, "Alone", 2302);

    // --- LEVEL 23 ---
    addNode(23, "LEVEL 23: Trial", 0, 0, 0, 0, 0);
    connect(2301, "Continue", 23); connect(2302, "Continue", 23);
    addNode(2401, "Protect.", -5, -5, +5, +10, 0);
    addNode(2402, "Command.", 0, 0, +5, +8, 0);
    connect(23, "Protect", 2401); connect(23, "Command", 2402);

    // --- LEVEL 24 ---
    addNode(24, "LEVEL 24: Invasion", 0, 0, 0, 0, 1);
    connect(2401, "Continue", 24); connect(2402, "Continue", 24);
    addNode(2501, "Defend.", -10, -10, +10, +10, 0);
    addNode(2502, "Retreat.", 0, -5, +5, -10, 0);
    connect(24, "Defend", 2501); connect(24, "Retreat", 2502);

    // --- LEVEL 25 ---
    addNode(25, "LEVEL 25: Bonds", 0, 0, 0, 0, 0);
    connect(2501, "Continue", 25); connect(2502, "Continue", 25);
    addNode(2601, "Bond.", 0, -5, +5, +5, 0);
    addNode(2602, "Scout.", 0, -5, +5, 0, 0, "None", "Herbs");
    connect(25, "Bond", 2601); connect(25, "Scout", 2602);

    // --- LEVEL 26 ---
    addNode(26, "LEVEL 26: March", 0, 0, 0, 0, 1);
    connect(2601, "Continue", 26); connect(2602, "Continue", 26);
    addNode(2701, "Train.", +5, -10, +10, 0, 0);
    addNode(2702, "Rest.", 0, +10, +5, 0, 0);
    connect(26, "Train", 2701); connect(26, "Rest", 2702);

    // --- LEVEL 27 ---
    addNode(27, "LEVEL 27: Wounds", 0, 0, 0, 0, 1);
    connect(2701, "Continue", 27); connect(2702, "Continue", 27);
    addNode(2801, "Heal.", +15, 0, 0, 0, 0, "Herbs");
    addNode(2802, "Ignore.", -12, 0, +5, 0, 0);
    connect(27, "Heal", 2801); connect(27, "Ignore", 2802);

    // --- LEVEL 28 ---
    addNode(28, "LEVEL 28: Calm", 0, 0, 0, 0, 0);
    connect(2801, "Continue", 28); connect(2802, "Continue", 28);
    addNode(2901, "Reflect.", 0, 0, +5, +5, 0);
    addNode(2902, "Alert.", 0, -5, +5, 0, 0);
    connect(28, "Reflect", 2901); connect(28, "Alert", 2902);

    // --- LEVEL 29 ---
    addNode(29, "LEVEL 29: Shadow", 0, 0, 0, 0, 1);
    connect(2901, "Continue", 29); connect(2902, "Continue", 29);
    addNode(3001, "Ambush.", 0, -10, +5, 0, 0);
    addNode(3002, "Challenge.", -5, -5, +5, 0, 0);
    connect(29, "Ambush", 3001); connect(29, "Challenge", 3002);

    // --- LEVEL 30 ---
    addNode(30, "LEVEL 30: Zolver Nighttreaver", 0, 0, 0, 0, 0);
    connect(3001, "Continue", 30); connect(3002, "Continue", 30);

    // VICTORY
    addNode(999, "VICTORY\nYou have defeated Zolver and reclaimed your destiny!", 0, 0, 0, +100, 0);
    connect(30, "Final Strike", 999);

    currentNode = storyMap[1];
}

std::vector<Item> GameEngine::getInventoryForDisplay() {
    return inventory.toVector();
}

std::string GameEngine::getFinalTitle() {
    if (currentStats.reputation >= 80) return "Legendary Alpha";
    if (currentStats.reputation >= 50) return "Pack Leader";
    return "Lone Survivor";
}
std::string GameEngine::getCurrentStoryText() { return currentNode ? currentNode->text : "Error"; }

// FIX 4: Removed legacy unused function definitions to match .h

void GameEngine::attemptRest() {}
void GameEngine::useItem(int itemIndex) {}
void GameEngine::connect(int parentID, std::string choiceText, int childID) {
    if (storyMap.count(parentID)) storyMap[parentID]->children.push_back({choiceText, childID});
}

// =========================================================
// LOGIC (Handling Clicks) - FIXED TO USE LINKED LIST
// =========================================================
void GameEngine::makeChoice(int choiceIndex) {
    if (gameOver || gameWon || !currentNode) return;
    if (choiceIndex >= currentNode->children.size()) return;

    std::string choiceText = currentNode->children[choiceIndex].first;
    gameLog.push_back(">> " + choiceText);

    int nextID = currentNode->children[choiceIndex].second;
    StoryNode* nextNode = storyMap[nextID];

    // 1. Requirements (Fixed: uses hasItem)
    bool reqMet = true;
    if (nextNode->requiredItem == "Meat" || nextNode->requiredItem == "Herbs") {
        if (!inventory.hasItem(nextNode->requiredItem)) { 
            gameLog.push_back("LOCKED: Need " + nextNode->requiredItem); 
            reqMet = false; 
        }
    } else if (nextNode->requiredItem.find("Energy>") != std::string::npos) {
        int val = std::stoi(nextNode->requiredItem.substr(7));
        if (currentStats.energy < val) { gameLog.push_back("LOCKED: Low Energy"); reqMet = false; }
    } else if (nextNode->requiredItem.find("Rep>") != std::string::npos) {
        int val = std::stoi(nextNode->requiredItem.substr(4));
        if (currentStats.reputation < val) { gameLog.push_back("LOCKED: Low Reputation"); reqMet = false; }
    }
    
    if (!reqMet) return;

    // 2. Apply Stats
    currentStats.health += nextNode->healthChange;
    currentStats.energy += nextNode->energyChange;
    currentStats.hunger += nextNode->hungerChange;
    currentStats.reputation += nextNode->reputationChange;
    currentStats.dayCount += nextNode->dayChange;
    
    if (currentStats.health < 0) currentStats.health = 0; if (currentStats.health > 100) currentStats.health = 100;
    if (currentStats.energy < 0) currentStats.energy = 0; if (currentStats.energy > 100) currentStats.energy = 100;
    if (currentStats.hunger < 0) currentStats.hunger = 0; if (currentStats.hunger > 100) currentStats.hunger = 100;

    // 3. Consumables (Fixed: uses removeOne)
    if (nextNode->requiredItem == "Meat" || nextNode->requiredItem == "Herbs") {
         if (inventory.removeOne(nextNode->requiredItem)) {
             gameLog.push_back("Used " + nextNode->requiredItem);
         }
    }

    // 4. Rewards (Fixed: uses addItem)
    if (nextNode->rewardItem != "None") {
        ItemType type = TOOL;
        if(nextNode->rewardItem == "Meat") type = FOOD;
        if(nextNode->rewardItem == "Herbs") type = HERB;
        if(nextNode->rewardItem == "Bone") type = WEAPON;
        
        if (inventory.addItem(Item(nextNode->rewardItem, type, 0, 1))) {
            gameLog.push_back("Obtained: " + nextNode->rewardItem);
        } else {
            gameLog.push_back("Inventory Full! Left behind: " + nextNode->rewardItem);
        }
    }

    currentNode = nextNode;

    if (currentStats.health <= 0 || currentStats.hunger >= 100) { gameOver = true; gameLog.push_back("GAME OVER"); }
    else if (nextID == 999) { gameWon = true; gameLog.push_back("VICTORY!"); }
}

void GameEngine::addNode(int id, std::string text, int h, int e, int hu, int r, int d, std::string req, std::string rew) {
    std::string img = "";
    if (id >= 1 && id <= 30) img = std::to_string(id) + ".png"; // Image Auto-Assign
    
    StoryNode* node = new StoryNode(id, text, img);
    node->healthChange = h; node->energyChange = e; node->hungerChange = hu; 
    node->reputationChange = r; node->dayChange = d; node->requiredItem = req; node->rewardItem = rew;
    storyMap[id] = node;
}
