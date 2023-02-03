#pragma once
#include "Unit.h"

#include "Building.h"

class Unit;

class HudManager{
public:
	

	void UnitSelected();
	void UnitsSelected(size_t unitcount);
	void UnitAbilities(std::shared_ptr<Unit> unitinfo);
	void PrimaryUnitSelection();//if you select units of differing types what abilities do you want shown

	void BuildingSelected();
	void BuildingsSelected(size_t buildingcount);	
	void BuildingAbilities(std::shared_ptr<Building> buildinginfo);//Units
	void ResearchAbilites(std::shared_ptr<Building> buildinginfo);
	void ProductionAbilites(std::shared_ptr<Building> buildinginfo);
	void PrimaryBuildingSelection();//if you select buildings of differing types what abilities do you want shown

	void HerosHud();

	void ObjectDetails();


	enum {
		None,
		Research,
		Producable
	}selection = None;



	bool CheckRequirements(int owner, cAssets::BuildingType object);
	bool CheckRequirements(int owner, cAssets::UnitType object);
	bool CheckRequirements(int owner, cAssets::ResearchType object, std::string name);


	std::vector<std::string> Hide_Research; //If your are researching something dont let users research again 
	
	
	HudManager();
	~HudManager();

	float IconY;
};

