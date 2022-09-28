#pragma once

#include "olcPixelGameEngine.h"
#include "clock.h"

class UserInput{
	Clock clipTimer;
public:
void GetUserInput();
void DrawUserInput();
	UserInput();
	virtual ~UserInput();

	//Selection click
	olc::vf2d Initial, Final;
	bool Clicked = false;
};

