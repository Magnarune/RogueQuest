#pragma once
#include "Engine.h"
class HudManager{
public:
	bool Button(olc::vf2d Position, olc::vi2d Mouse, olc::vf2d Width_Height);

	void UnitSelected();
	void UnitsSelected();
	void BuildingSelected();
	void BuildingsSelected();
	void UnitAbilities(std::shared_ptr<Unit> unitinfo);
	void BuildingAbilities(std::shared_ptr<Building> buildinginfo);
	HudManager();
	~HudManager();

	float IconY;
};

