#pragma once
#include "olcPixelGameEngine.h"
#include <map>
#include <functional>
#include <string>
#include <variant>

class MainMenu {
	void loadImage(const std::string& name, const std::string& path);//Main Menu Assets
	void loadImage(const std::string& name, size_t tex_id);
	struct Option { // custom option type
		//			 		 button			checkbox 	textbox		slider
		typedef std::variant<std::monostate, bool, std::string, double> ValueType;
		typedef std::function<void(Option&)> CallbackType;

		enum GuiType {
			Checkbox,
			Textbox,
			Button,
			BigBoxButton,
			Label,
			Slider
		} type;

		olc::vf2d position;
		std::string label;
		CallbackType callback;
		ValueType value;

		Option(GuiType t, const std::string& l, CallbackType cb, olc::vf2d pos, ValueType def = {});
		Option() = default; // whatever - add a default
	};

	typedef std::map<std::string, Option> OptionsList;
public:


	OptionsList mainMenu;
	OptionsList setupMenu;
	OptionsList creditsMenu;
	OptionsList networkMenu;

	OptionsList* currentMenu = nullptr;
	inline void SetGuiMenu(const std::string& name) { currentMenu = menus.count(name) ? menus.at(name) : nullptr; }
	std::map<std::string, OptionsList*> menus; // string to menus reference for easy switching

	void DrawGui(OptionsList& list);


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
	
	float Occialtions(float delta, float& timer,float rate);
	
	bool GameStarted = false;
	float TSDN=0.f;//Title Screen Day Night Cycle
	float TSOV = 0.f; //TitleScreen return value
	bool TS = false; //TitleScreen flipper
	bool ExitGame=false;

	float occilate= 0.f;
	float timer = 0.f;
	bool down = false;
	bool InitiateGame = false;
};
