#pragma once
#include "olcPixelGameEngine.h"
class Options{ //Maybe a single .lua file that can be edited ohh 0.0
	
public:
	void DrawMainMenu();
	void DrawSoundMenu();
	void DrawGameMenu();
	void MenuSelect();

	Options();
	virtual ~Options();

	enum Options_Menu
	{
		Main_Options,
		Sound_Options,
		Game_Options
	};
	int Button = Main_Options;



	//All My Booleans Go here?
};

