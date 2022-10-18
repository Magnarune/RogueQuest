#pragma once
#include "olcPixelGameEngine.h"
#include"Building.h"
class Leader {
public:
	Leader();
	virtual ~Leader();

	std::weak_ptr<Building> HomeBase;
	std::vector<std::string> Buildings;
	std::vector<std::string> ResearchUpgrades;
	int Gold;
	int Lumber;
	int Food;

	// Allignment Friends and enemies
	
	std::map<int, std::vector<int>> Allies;

	friend class Game_Engine;
};