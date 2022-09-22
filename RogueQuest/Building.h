#pragma once
#include "olcPixelGameEngine.h"
#include "olcPGEX_TransformedView.h"
class Building
{
	Building();
	void BuildingBehaviour();
	void DrawBuilding();
	void UpdateBuilding(float delta);
public:
	virtual ~Building();
	std::string name;
	olc::vf2d pos;
	float health;
	float maxHealth;
	int Stage;//What stage is this in? lvl 1 house lvl 2 house etc
	int hurtstage;//if hurt show burning flames?	
};

