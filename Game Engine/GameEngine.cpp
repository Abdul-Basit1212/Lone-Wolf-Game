#include "GameEngine.h"
#include <iostream>
#include <algorithm>
#include <vector>

// =========================================================
// 1. INITIALIZATION
// =========================================================
void GameEngine::initGame() {
    // 1. Reset Player State
    // Health=100, Hunger=0 (Not Hungry), Energy=100
    currentStats = {100, 0, 100, 0, 1}; 
    inventory.clear();
    storyMap.clear();
    gameLog.clear();
    gameLog.push_back("--- NEW GAME STARTED ---");
    
    gameOver = false;
    gameWon = false;

    // ==================================================================================
    // BUILD THE STORY TREE (30 LEVELS)
    // ==================================================================================

    // --- LEVEL 1 ---
    addNode(1, "LEVEL 1: Frozen Awakening\nAlex wakes alone in the snow. The pack is gone.\nSilence surrounds you.", 0, 0, 0, 0, 0);
    // FIX: Moving adds +5 Hunger
    addNode(201, "You find shelter. It is safer.", +5, -5, +5, 0, 1);
    addNode(202, "You search for food.", 0, -10, +10, 0, 1, "Energy>10", "Meat"); 
    connect(1, "Search for Shelter", 201);
    connect(1, "Search for Food", 202);

    // --- LEVEL 2 ---
    addNode(2, "LEVEL 2: Echoes in the Snow\nEvery sound feels familiar... and painful.", 0, 0, 0, 0, 0);
    connect(201, "Continue", 2);
    connect(202, "Continue", 2);
    addNode(301, "You move carefully.", 0, -4, +5, +2, 0);
    addNode(302, "You move fast.", 0, -8, +8, 0, 0); 
    connect(2, "Move Carefully", 301);
    connect(2, "Move Fast", 302);

    // --- LEVEL 3 ---
    addNode(3, "LEVEL 3: The First Hunger\nYour stomach growls loudly.", 0, 0, 0, 0, 1);
    connect(301, "Continue", 3);
    connect(302, "Continue", 3);
    addNode(401, "You hunt small prey.", 0, -8, +10, 0, 0, "Energy>8", "Meat");
    addNode(402, "You forage quietly.", 0, -4, +5, 0, 0, "None", "Herbs");
    connect(3, "Hunt (Needs Energy)", 401);
    connect(3, "Forage Quietly", 402);

    // --- LEVEL 4 ---
    addNode(4, "LEVEL 4: Silent Trees\nThe forest goes quiet. Danger is near.", 0, 0, 0, 0, 1);
    connect(401, "Continue", 4);
    connect(402, "Continue", 4);
    addNode(501, "You hide.", +2, -3, +5, 0, 0);
    addNode(502, "You escape!", -4, -7, +10, 0, 0);
    connect(4, "Hide", 501);
    connect(4, "Escape", 502);

    // --- LEVEL 5 ---
    addNode(5, "LEVEL 5: First Blood\nYou found food. Eat now or save it?", 0, 0, 0, 0, 0);
    connect(501, "Continue", 5);
    connect(502, "Continue", 5);
    // FIX: Eating REDUCES hunger (-30) and Heals (+10)
    addNode(601, "You eat the meat.", +10, +5, -30, 0, 0, "Meat"); 
    addNode(602, "You save it.", 0, 0, +5, 0, 0);
    connect(5, "Eat Meat", 601);
    connect(5, "Save Meat", 602);

    // --- LEVEL 6 ---
    addNode(6, "LEVEL 6: Cold Night\nNight falls. Rest restores energy, but hunger grows.", 0, 0, 0, 0, 1);
    connect(601, "Continue", 6);
    connect(602, "Continue", 6);
    addNode(701, "You sleep deeply.", 0, +15, +10, 0, 0);
    addNode(702, "You stay alert.", 0, -6, +5, +3, 0);
    connect(6, "Sleep", 701);
    connect(6, "Stay Alert", 702);

    // --- LEVEL 7 ---
    addNode(7, "LEVEL 7: A Distant Howl\nA howl echoes. It could be family... or a threat.", 0, 0, 0, 0, 1);
    connect(701, "Continue", 7);
    connect(702, "Continue", 7);
    addNode(801, "You follow the howl.", 0, -5, +5, +8, 0);
    addNode(802, "You ignore it.", 0, 0, +5, -5, 0);
    connect(7, "Follow", 801);
    connect(7, "Ignore", 802);

    // --- LEVEL 8 ---
    addNode(8, "LEVEL 8: Hidden Claws\nSomething attacks without warning!", 0, 0, 0, 0, 0);
    connect(801, "Continue", 8);
    connect(802, "Continue", 8);
    addNode(901, "You fight back!", -5, -8, +5, 0, 0);
    addNode(902, "You escape, taking damage.", -10, -5, +5, 0, 0);
    connect(8, "Fight", 901);
    connect(8, "Escape", 902);

    // --- LEVEL 9 ---
    addNode(9, "LEVEL 9: Bleeding Path\nBlood stains the snow.", 0, 0, 0, 0, 1);
    connect(901, "Continue", 9);
    connect(902, "Continue", 9);
    addNode(1001, "You heal your wounds.", +20, 0, 0, 0, 0, "Herbs"); 
    addNode(1002, "You ignore the pain.", -5, 0, +5, 0, 0);
    connect(9, "Heal (Needs Herbs)", 1001);
    connect(9, "Ignore Pain", 1002);

    // --- LEVEL 10: The Bear Trial (BOSS) ---
    addNode(10, "LEVEL 10: The Bear Trial\nA massive bear blocks your path.", 0, 0, 0, 0, 0);
    connect(1001, "Continue", 10);
    connect(1002, "Continue", 10);
    // FIX: Winning fight still causes DAMAGE (-25 Health)
    addNode(1101, "You defeat the bear! (Victory)", -25, -20, +10, +15, 0, "Energy>20", "Bone");
    addNode(1102, "You are crushed.", -100, 0, 0, 0, 0); // Death
    connect(10, "Fight (Needs 20 Energy)", 1101);
    connect(10, "Give Up", 1102);

    // --- LEVEL 11 ---
    addNode(11, "LEVEL 11: Frozen River\nThe ice may crack.", 0, 0, 0, 0, 1);
    connect(1101, "Continue", 11);
    addNode(1201, "You cross carefully.", 0, -8, +5, 0, 0);
    addNode(1202, "You follow the bank.", 0, -5, +5, 0, 0, "None", "Herbs");
    connect(11, "Cross River", 1201);
    connect(11, "Follow River", 1202);

    // --- LEVEL 12 ---
    addNode(12, "LEVEL 12: Lonely Stars\nYou feel small... but alive.", 0, 0, 0, 0, 1);
    connect(1201, "Continue", 12);
    connect(1202, "Continue", 12);
    addNode(1301, "You rest.", 0, +10, +5, 0, 1);
    addNode(1302, "You keep moving.", 0, -5, +5, 0, 1);
    connect(12, "Rest", 1301);
    connect(12, "Move On", 1302);

    // --- LEVEL 13 ---
    addNode(13, "LEVEL 13: Human Scent\nHunters may be nearby.", 0, 0, 0, 0, 0);
    connect(1301, "Continue", 13);
    connect(1302, "Continue", 13);
    addNode(1401, "You hide.", 0, -2, +5, +5, 0);
    addNode(1402, "You flee.", -5, -8, +8, 0, 0);
    connect(13, "Hide", 1401);
    connect(13, "Flee", 1402);

    // --- LEVEL 14 ---
    addNode(14, "LEVEL 14: Strength in Silence\nSolitude hardens your body.", 0, 0, 0, 0, 1);
    connect(1401, "Continue", 14);
    connect(1402, "Continue", 14);
    addNode(1501, "You train.", +5, -10, +10, 0, 0);
    addNode(1502, "You explore.", 0, -5, +5, 0, 0, "None", "TerritoryMark");
    connect(14, "Train", 1501);
    connect(14, "Explore", 1502);

    // --- LEVEL 15 ---
    addNode(15, "LEVEL 15: Marking the Land\nTerritory means dominance.", 0, 0, 0, 0, 1);
    connect(1501, "Continue", 15);
    connect(1502, "Continue", 15);
    addNode(1601, "You mark territory.", 0, -5, +5, +10, 0, "None", "TerritoryMark");
    addNode(1602, "You observe.", 0, 0, +5, 0, 0);
    connect(15, "Mark Territory", 1601);
    connect(15, "Observe", 1602);

    // --- LEVEL 16 ---
    addNode(16, "LEVEL 16: Silverpaw Borders\nAnother clan's land.", 0, 0, 0, 0, 1);
    connect(1601, "Continue", 16);
    connect(1602, "Continue", 16);
    addNode(1701, "Approach respectfully.", 0, 0, +5, +10, 0);
    addNode(1702, "Avoid contact.", 0, -4, +5, 0, 0);
    connect(16, "Approach", 1701);
    connect(16, "Avoid", 1702);

    // --- LEVEL 17 ---
    addNode(17, "LEVEL 17: Trust or Fear", 0, 0, 0, 0, 0);
    connect(1701, "Continue", 17);
    connect(1702, "Continue", 17);
    addNode(1801, "Trust.", 0, 0, +5, +8, 0);
    addNode(1802, "Remain alone.", 0, 0, +5, -5, 0);
    connect(17, "Trust", 1801);
    connect(17, "Stay Alone", 1802);

    // --- LEVEL 18 ---
    addNode(18, "LEVEL 18: Old Truths", 0, 0, 0, 0, 1);
    connect(1801, "Continue", 18);
    connect(1802, "Continue", 18);
    addNode(1901, "Seek revenge.", 0, -5, +5, +5, 0);
    addNode(1902, "Seek peace.", 0, 0, +5, +10, 0);
    connect(18, "Revenge", 1901);
    connect(18, "Peace", 1902);

    // --- LEVEL 19 ---
    addNode(19, "LEVEL 19: Preparing for War", 0, 0, 0, 0, 1);
    connect(1901, "Continue", 19);
    connect(1902, "Continue", 19);
    addNode(2001, "Prepare weapons.", 0, -5, +5, 0, 0, "None", "Bone");
    addNode(2002, "Rest for strength.", 0, +10, +5, 0, 0);
    connect(19, "Prepare Weapons", 2001);
    connect(19, "Rest", 2002);

    // --- LEVEL 20: Rival Alpha (BOSS) ---
    addNode(20, "LEVEL 20: Rival Alpha\nA powerful alpha challenges you.", 0, 0, 0, 0, 0);
    connect(2001, "Continue", 20);
    connect(2002, "Continue", 20);
    addNode(2101, "You win the duel!", -15, -15, +10, +20, 0, "Rep>30", "None");
    addNode(2102, "You are defeated.", -100, 0, 0, 0, 0);
    connect(20, "Fight (Needs 30 Rep)", 2101);
    connect(20, "Surrender", 2102);

    // --- LEVEL 21 ---
    addNode(21, "LEVEL 21: Scars of Victory", 0, 0, 0, 0, 1);
    connect(2101, "Continue", 21);
    addNode(2201, "Heal wounds.", +20, 0, 0, 0, 0, "Herbs");
    addNode(2202, "Push forward.", -5, -5, +5, 0, 0);
    connect(21, "Heal (Needs Herbs)", 2201);
    connect(21, "Push Forward", 2202);

    // --- LEVEL 22 ---
    addNode(22, "LEVEL 22: Call of the Pack", 0, 0, 0, 0, 1);
    connect(2201, "Continue", 22);
    connect(2202, "Continue", 22);
    addNode(2301, "Recruit followers.", 0, -5, +5, +10, 0, "Rep>40", "PackMember");
    addNode(2302, "Walk alone.", 0, 0, +5, 0, 0);
    connect(22, "Recruit (Needs 40 Rep)", 2301);
    connect(22, "Walk Alone", 2302);

    // --- LEVEL 23 ---
    addNode(23, "LEVEL 23: Leader's Trial", 0, 0, 0, 0, 0);
    connect(2301, "Continue", 23);
    connect(2302, "Continue", 23);
    addNode(2401, "Protect them.", -5, -5, +5, +10, 0);
    addNode(2402, "Command strictly.", 0, 0, +5, +8, 0);
    connect(23, "Protect Pack", 2401);
    connect(23, "Command Strictly", 2402);

    // --- LEVEL 24 ---
    addNode(24, "LEVEL 24: Territory Invasion", 0, 0, 0, 0, 1);
    connect(2401, "Continue", 24);
    connect(2402, "Continue", 24);
    addNode(2501, "Defend territory.", -10, -10, +10, +10, 0);
    addNode(2502, "Retreat.", 0, -5, +5, -10, 0);
    connect(24, "Defend", 2501);
    connect(24, "Retreat", 2502);

    // --- LEVEL 25 ---
    addNode(25, "LEVEL 25: Strength of Bonds", 0, 0, 0, 0, 0);
    connect(2501, "Continue", 25);
    connect(2502, "Continue", 25);
    addNode(2601, "Strengthen bonds.", 0, -5, +5, +5, 0);
    addNode(2602, "Scout ahead.", 0, -5, +5, 0, 0, "None", "Herbs");
    connect(25, "Strengthen Bonds", 2601);
    connect(25, "Scout", 2602);

    // --- LEVEL 26 ---
    addNode(26, "LEVEL 26: March Toward Fate", 0, 0, 0, 0, 1);
    connect(2601, "Continue", 26);
    connect(2602, "Continue", 26);
    addNode(2701, "Train hard.", +5, -10, +10, 0, 0);
    addNode(2702, "Rest.", 0, +10, +5, 0, 0);
    connect(26, "Train", 2701);
    connect(26, "Rest", 2702);

    // --- LEVEL 27 ---
    addNode(27, "LEVEL 27: Old Wounds", 0, 0, 0, 0, 1);
    connect(2701, "Continue", 27);
    connect(2702, "Continue", 27);
    addNode(2801, "Heal wounds.", +15, 0, 0, 0, 0, "Herbs");
    addNode(2802, "Ignore pain.", -12, 0, +5, 0, 0);
    connect(27, "Heal (Needs Herbs)", 2801);
    connect(27, "Ignore", 2802);

    // --- LEVEL 28 ---
    addNode(28, "LEVEL 28: Calm Before the Storm", 0, 0, 0, 0, 0);
    connect(2801, "Continue", 28);
    connect(2802, "Continue", 28);
    addNode(2901, "Reflect.", 0, 0, +5, +5, 0);
    addNode(2902, "Stay alert.", 0, -5, +5, 0, 0);
    connect(28, "Reflect", 2901);
    connect(28, "Stay Alert", 2902);

    // --- LEVEL 29 ---
    addNode(29, "LEVEL 29: Shadow of Zolver", 0, 0, 0, 0, 1);
    connect(2901, "Continue", 29);
    connect(2902, "Continue", 29);
    addNode(3001, "Prepare ambush.", 0, -10, +5, 0, 0);
    addNode(3002, "Challenge directly.", -5, -5, +5, 0, 0);
    connect(29, "Ambush", 3001);
    connect(29, "Direct Challenge", 3002);

    // --- LEVEL 30: Zolver Nighttreaver (FINAL BOSS) ---
    addNode(30, "LEVEL 30: Zolver Nighttreaver\nThe final battle. Only one will rule.", 0, 0, 0, 0, 0);
    connect(3001, "Continue", 30);
    connect(3002, "Continue", 30);

    // VICTORY NODE (ID 999)
    addNode(999, "VICTORY\nYou have defeated Zolver and reclaimed your destiny!", 0, 0, 0, +100, 0);

    // Final Choices connecting to Victory
    connect(30, "Strike Final Blow", 999);
    connect(30, "Command Pack to Attack", 999);

    // Set Start Node
    currentNode = storyMap[1];
}

// =========================================================
// 2. HELPERS
// =========================================================

std::string GameEngine::getFinalTitle() {
    if (currentStats.reputation >= 80) return "Legendary Alpha";
    if (currentStats.reputation >= 50) return "Pack Leader";
    if (currentStats.health >= 90) return "Iron Guardian";
    if (currentStats.energy >= 90) return "Shadow Scout";
    return "Lone Survivor";
}

std::vector<Item> GameEngine::getInventoryForDisplay() { return inventory; }
std::string GameEngine::getCurrentStoryText() { return currentNode ? currentNode->text : "Error"; }
std::string GameEngine::getChoiceAText() { return ""; }
std::string GameEngine::getChoiceBText() { return ""; }

// =========================================================
// 3. LOGIC (Handling Clicks)
// =========================================================

void GameEngine::makeChoice(int choiceIndex) {
    if (gameOver || gameWon) return; // Freeze input on end
    if (!currentNode) return;
    if (choiceIndex >= currentNode->children.size()) return;

    // 1. Log Choice
    std::string choiceText = currentNode->children[choiceIndex].first;
    gameLog.push_back(">> " + choiceText);

    // 2. Next Node
    int nextID = currentNode->children[choiceIndex].second;
    StoryNode* nextNode = storyMap[nextID];

    // 3. Check Requirements
    bool reqMet = true;
    if (nextNode->requiredItem == "Meat" || nextNode->requiredItem == "Herbs") {
        auto it = std::find_if(inventory.begin(), inventory.end(), [&](const Item& i){ return i.name == nextNode->requiredItem; });
        if (it == inventory.end()) { gameLog.push_back("LOCKED: Need " + nextNode->requiredItem); reqMet = false; }
    }
    else if (nextNode->requiredItem.find("Energy>") != std::string::npos) {
        int val = std::stoi(nextNode->requiredItem.substr(7));
        if (currentStats.energy < val) { gameLog.push_back("LOCKED: Low Energy"); reqMet = false; }
    }
    else if (nextNode->requiredItem.find("Rep>") != std::string::npos) {
        int val = std::stoi(nextNode->requiredItem.substr(4));
        if (currentStats.reputation < val) { gameLog.push_back("LOCKED: Low Reputation"); reqMet = false; }
    }

    if (!reqMet) return;

    // 4. Apply Stats
    currentStats.health += nextNode->healthChange;
    currentStats.energy += nextNode->energyChange;
    currentStats.hunger += nextNode->hungerChange;
    currentStats.reputation += nextNode->reputationChange;
    currentStats.dayCount += nextNode->dayChange;

    // FIX: Clamp Values (0-100)
    if (currentStats.health < 0) currentStats.health = 0;
    if (currentStats.health > 100) currentStats.health = 100;

    if (currentStats.energy < 0) currentStats.energy = 0;
    if (currentStats.energy > 100) currentStats.energy = 100;

    if (currentStats.hunger < 0) currentStats.hunger = 0;
    if (currentStats.hunger > 100) currentStats.hunger = 100;

    // 5. Consumables
    if (nextNode->requiredItem == "Meat" || nextNode->requiredItem == "Herbs") {
         auto it = std::find_if(inventory.begin(), inventory.end(), [&](const Item& i){ return i.name == nextNode->requiredItem; });
         if (it != inventory.end()) {
             it->quantity--;
             gameLog.push_back("Used " + nextNode->requiredItem);
             if (it->quantity <= 0) inventory.erase(it);
         }
    }

    // 6. Rewards
    if (nextNode->rewardItem != "None") {
        bool found = false;
        for (auto &item : inventory) {
            if (item.name == nextNode->rewardItem) { item.quantity++; found = true; break; }
        }
        if (!found) {
            ItemType type = TOOL;
            if(nextNode->rewardItem == "Meat") type = FOOD;
            if(nextNode->rewardItem == "Herbs") type = HERB;
            inventory.push_back(Item(nextNode->rewardItem, type, 0, 1));
        }
        gameLog.push_back("Obtained: " + nextNode->rewardItem);
    }

    // 7. Move Pointer
    currentNode = nextNode;

    // 8. WIN / LOSS CHECK
    if (currentStats.health <= 0 || currentStats.hunger >= 100) {
        gameOver = true;
        gameLog.push_back("GAME OVER: You have perished.");
    }
    else if (nextID == 999) { // Victory Node ID
        gameWon = true;
        gameLog.push_back("VICTORY ACHIEVED!");
    }
}

// Helpers
void GameEngine::addNode(int id, std::string text, int h, int e, int hu, int r, int d, std::string req, std::string rew) {
    StoryNode* node = new StoryNode(id, text);
    node->healthChange = h; node->energyChange = e; node->hungerChange = hu; 
    node->reputationChange = r; node->dayChange = d; node->requiredItem = req; node->rewardItem = rew;
    storyMap[id] = node;
}
void GameEngine::connect(int parentID, std::string choiceText, int childID) {
    if (storyMap.count(parentID)) storyMap[parentID]->children.push_back({choiceText, childID});
}
void GameEngine::attemptRest() {}
void GameEngine::useItem(int itemIndex) {}