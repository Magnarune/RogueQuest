#pragma once
#include "olcPixelGameEngine.h"
#include "Research.h"
#include"Building.h"
#include "Hero.h"
class Researchable;
class Leader {
public:
	Leader();
	virtual ~Leader();

	void UpdateBonus();
	int CheckCost(std::string name);
	std::string LeaderName = "";
	olc::Decal* LeaderHead;
	std::string AGE; //What Age is it
	//std::weak_ptr<Hero> HomeHero;//If you choice to be a Hero Unit
	std::weak_ptr<Building> HomeBase; //Home Base Castle /Town Hall
	std::vector<std::string> Buildings; //List of Leader's buildings 
	std::vector<std::string> ResearchUpgrades;//strings of upgrades you have and their level
	int Gold;
	int Lumber;
	int Food;
	bool Kingdom;
	bool HeroDynamic;//Is the hero controled dynamically or Classically
	//std::vector<std::pair<int, std::weak_ptr<Researchable>>> researchUpgrades; //Unit/Leader Upgrades
	std::vector<std::weak_ptr<Researchable>> researchUpgrades; //Unit/Leader Upgrades
	std::vector<std::string> Unlockes; //Unlocked objects that gets checked often
	// Allignment Friends and enemies	
	std::map<int, std::vector<int>> Allies;
	struct {
		int meleedamage = 0;
		int rangedamage = 0;
		int attackrange = 0;
		int armour = 0;
		int movespeed = 0;
		int health = 0;
		int mana = 0;
		float attackspeed = 0;
		int ammo = 0;
	}Bonus;

};