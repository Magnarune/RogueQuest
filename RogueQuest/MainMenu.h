#pragma once
#include "olcPixelGameEngine.h"

class MainMenu {
	virtual ~MainMenu();
public:
	MainMenu();
	
	
	
	void MainMenuSelection(float delta);

	void StartMenu(float delta);
	void SetupGame(float delta);
	void CreateGame();
	void SettingMenu(float delta);
	void ExitProgram();




	enum {
		None,
		Setup_Game,
		Settings,
		Exit_Game

	}MainMenuSelect = None;




};