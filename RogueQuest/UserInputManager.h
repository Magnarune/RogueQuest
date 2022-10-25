#pragma once
#include "olcPixelGameEngine.h"
#include "WorldObject.h"
#include "UnitManager.h"
#include "Unit.h"
#include "Building.h"
#include "BuildingManager.h"
class UserInputManager {

public:
	UserInputManager();
	virtual ~UserInputManager();
	void StandardUserInput();
	void DecideUnitAction();
	void GetBuildModeUserInput();
	
	bool Button(olc::vf2d Position, olc::vi2d Mouse, olc::vf2d Width_Height);
	bool InsideBox(olc::vf2d Position, olc::vi2d Mouse, olc::vf2d Width_Height);
	bool Hover(olc::vf2d Position, olc::vi2d Mouse, olc::vf2d Width_Height, float triggertime , float felapstedtime);

	float triggertime = 0.f;
	olc::vf2d Initial, Final;
	bool Clicked = false;
	bool A_click;
	std::weak_ptr<Building> build;
	std::weak_ptr<Unit> unit;
};