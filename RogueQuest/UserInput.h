#pragma once

#include "olcPixelGameEngine.h"
#include "clock.h"

class UserInput{
	Clock clipTimer;
public:

void CameraInput();

void GetUserInput();
void DrawUserInput();

	UserInput();
	virtual ~UserInput();

	olc::vf2d CenterofScreen;	
};

