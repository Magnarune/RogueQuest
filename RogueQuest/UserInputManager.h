#pragma once
#include "olcPixelGameEngine.h"
class UserInputManager {

public:
	UserInputManager();
	virtual ~UserInputManager();
	void StandardUserInput();


	void GetBuildModeUserInput();
	bool Button(olc::vf2d Position, olc::vi2d Mouse, olc::vf2d Width_Height);

	olc::vf2d Initial, Final;
	bool Clicked = false;

};