#pragma once

#include "olcPixelGameEngine.h"
#include "clock.h"

class UserInput{
	Clock clipTimer;
public:

void CameraInput();

//Boxes 
void MiniMapBox();




void Doubleclick();
void GetUserInput();
void DrawUserInput();

	UserInput();
	virtual ~UserInput();

	olc::vf2d CenterofScreen;	
};

