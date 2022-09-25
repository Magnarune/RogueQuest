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
		MAIN_OPTIONS,
		SOUND_OPTIONS,
		GAME_OPTIONS
	};
	int Button = MAIN_OPTIONS;



	//All My Booleans Go here?
	bool evil = false;
};

