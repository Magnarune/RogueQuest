#pragma once
#include "olcPixelGameEngine.h"
#include "Research.h"
#include"Building.h"
class Leader {
public:
	Leader();
	virtual ~Leader();
	std::string AGE; //What Age is it
	std::weak_ptr<Building> HomeBase; //Home Base Castle /Town Hall
	std::vector<std::string> Buildings; //List of Leader's buildings 
	std::vector<std::string> ResearchUpgrades;//strings of upgrades you have and their level
	int Gold;
	int Lumber;
	int Food;

	std::vector<int, std::weak_ptr<Researchable>> researchUpgrades; //Unit/Leader Upgrades
	std::vector<std::string> Unlockes; //Unlocked objects that gets checked often
	// Allignment Friends and enemies	
	std::map<int, std::vector<int>> Allies;
};