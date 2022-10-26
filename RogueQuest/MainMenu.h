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
	
	
	//void MenuButton(olc::vf2d button, olc::vi2d mouse, olc::vf2d size);
	bool MenuButton(olc::vf2d button, olc::vi2d mouse, olc::vf2d size, bool hover = false);
	
	bool MainMenuSelection(float delta);

	bool StartMenu(float delta);
	void SetupGame(float delta);
	void CreateGame();
	void SettingMenu(float delta);
	void ExitProgram();

	float Occialtions(float delta,float speed, float& timer, float& occilationvalue, float turnaround , bool& swap );

	float TSDN=0.f;//Title Screen Day Night Cycle
	float TSOV = 0.f; //TitleScreen return value
	bool TS = false; //TitleScreen flipper

	float occilate= 0.f;
	float timer = 0.f;
	bool down = false;
	enum {
		None,
		Setup_Game,
		Settings,
		Exit_Game

	}MainMenuSelect = None;




};