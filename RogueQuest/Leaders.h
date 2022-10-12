#pragma once
#include "olcPixelGameEngine.h"

class Leader {
public:
	Leader();
	virtual ~Leader();

	int Gold;
	int Lumber;
	int Food;

	// Allignment Friends and enemies
	
	std::map<int, std::vector<int>> Allies;

	friend class Game_Engine;
};