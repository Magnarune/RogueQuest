#pragma once

#include "olcPixelGameEngine.h"
#include "clock.h"

class UserInput{
	Clock clipTimer;
public:

void CameraInput();

//Boxes 
void MiniMapBox();

void Home();//Find Home Base Or Hero

void DeadZone();//Zones Where action's are not counted
void Doubleclick();
void GetUserInput();
void DrawUserInput();

	UserInput();
	virtual ~UserInput();

	olc::vf2d CenterofScreen;	
};

