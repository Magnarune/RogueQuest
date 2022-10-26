#pragma once
#include "olcPixelGameEngine.h"

class MainMenu {
	void loadImage(const std::string& name, const std::string& path);//Main Menu Assets
	void loadImage(const std::string& name, size_t tex_id);
public:
	void ImportMainMenuAssets();
	std::map<std::string, std::unique_ptr<olc::Decal>> menudecals;
	virtual ~MainMenu();
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