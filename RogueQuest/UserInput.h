#pragma once

#include "olcPixelGameEngine.h"

class UserInput{
public:
void GetUserInput();
void DrawUserInput();
	UserInput();
	virtual ~UserInput();

	olc::vf2d Initial_Click;
	olc::vf2d Final_Click;
};

