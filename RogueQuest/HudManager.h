#pragma once
#include "Unit.h"
#include "Building.h"
//#include "Engine.h"
class Unit;

class HudManager{
public:
	

	void UnitSelected();
	void UnitsSelected(size_t unitcount);
	void UnitAbilities(std::shared_ptr<Unit> unitinfo);
	void PrimaryUnitSelection();//if you select units of differing types what abilities do you want shown

	void BuildingSelected();
	void BuildingsSelected(size_t buildingcount);
	void BuildingAbilities(std::shared_ptr<Building> buildinginfo);
	void PrimaryBuildingSelection();//if you select buildings of differing types what abilities do you want shown

	HudManager();
	~HudManager();

	float IconY;
};

