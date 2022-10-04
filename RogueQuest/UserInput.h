#pragma once

#include "olcPixelGameEngine.h"
#include "clock.h"

class UserInput{
	Clock clipTimer;
public:

void CameraInput();
void GetUserInput();
void GetBuildModeUserInput();

void DrawUserInput();
void DrawBuildModeUserInput();

bool Button(olc::vf2d Position, olc::vi2d Mouse, olc::vf2d Width_Height);

	UserInput();
	virtual ~UserInput();

	//Selection click
	olc::vf2d CenterofScreen;
	olc::vf2d Initial, Final;
	bool Clicked = false;
};

