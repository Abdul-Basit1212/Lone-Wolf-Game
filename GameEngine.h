#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include "GameData.h"
#include "Inventory.h" 
#include <map>
#include <vector>
#include <string>

class GameEngine {
public:
    void initGame();

    // GUI Connections
    WolfStats& getStats() { return currentStats; }
    StoryNode* getCurrentNode() { return currentNode; }
    const std::vector<std::string>& getGameLog() const { return gameLog; }
    void processChoice(int choiceIndex) { makeChoice(choiceIndex); }
    
    // Inventory Getter
    std::vector<Item> getInventory() const { return inventory.toVector(); }

    bool isGameEnded() const { return gameOver || gameWon; }
    bool isVictory() const { return gameWon; }
    std::string getFinalTitle();

    unsigned int getNodeTexture(std::string path);

    // Backend Methods
    std::vector<Item> getInventoryForDisplay(); 
    std::string getCurrentStoryText();

    // REMOVED: getChoiceAText/BText (Legacy functions no longer used)

    void makeChoice(int choiceIndex); 
    void attemptRest();
    void useItem(int itemIndex);
    
private:
    std::map<int, StoryNode*> storyMap;
    StoryNode* currentNode;
    WolfStats currentStats;
    InventoryList inventory; 
    std::vector<std::string> gameLog; 
    bool gameOver = false;
    bool gameWon = false;

    std::map<std::string, unsigned int> textureCache; 
    unsigned int loadTextureFromFile(const char* filename);

    void addNode(int id, std::string text, int h, int e, int hu, int r, int d, std::string req = "None", std::string rew = "None");
    void connect(int parentID, std::string choiceText, int childID);
};

#endif
