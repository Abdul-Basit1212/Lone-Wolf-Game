#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include "GameData.h"
#include <map>
#include <vector>
#include <string>

class GameEngine {
public:
    void initGame();

    // ========================================================
    // GUI CONNECTIONS (Inline Wrappers)
    // ========================================================
    WolfStats& getStats() { return currentStats; }
    StoryNode* getCurrentNode() { return currentNode; }
    const std::vector<Item>& getInventory() const { return inventory; }
    const std::vector<std::string>& getGameLog() const { return gameLog; }
    
    void processChoice(int choiceIndex) { makeChoice(choiceIndex); }

    // --- NEW LOGIC HELPERS ---
    bool isGameEnded() const { return gameOver || gameWon; }
    bool isVictory() const { return gameWon; }
    std::string getFinalTitle(); // Calculates Alpha/Lone Wolf/etc.

    // ========================================================
    // BACKEND METHODS
    // ========================================================
    std::vector<Item> getInventoryForDisplay();
    std::string getCurrentStoryText();
    std::string getChoiceAText(); 
    std::string getChoiceBText();

    void makeChoice(int choiceIndex); 
    void attemptRest();
    void useItem(int itemIndex);
    
private:
    std::map<int, StoryNode*> storyMap;
    StoryNode* currentNode;
    WolfStats currentStats;
    std::vector<Item> inventory;
    std::vector<std::string> gameLog; 

    // Game State Flags
    bool gameOver = false;
    bool gameWon = false;

    void addNode(int id, std::string text, int h, int e, int hu, int r, int d, std::string req = "None", std::string rew = "None");
    void connect(int parentID, std::string choiceText, int childID);
};

#endif