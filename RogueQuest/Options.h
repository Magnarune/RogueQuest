#pragma once
#include "olcPixelGameEngine.h"

#include <map>
#include <functional>
#include <string>
#include <variant>

class Options {
public:
	struct Option { // custom option type
		//			 		 button			checkbox 	textbox		slider
		typedef std::variant<std::monostate,bool,		std::string,double> ValueType;
		typedef std::function<void(Option&)> CallbackType;

		enum GuiType {
			Checkbox,
			Textbox,
			Button,
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

private:
	OptionsList mainMenu;
	OptionsList soundMenu;
	OptionsList gameMenu;
	OptionsList optionsMenu;

	OptionsList* currentMenu = nullptr;

	std::map<std::string, OptionsList*> menus; // string to menus reference for easy switching

	void DrawGui(OptionsList& list);

public:

	bool MenuSelect();
	bool Exit_Game = true;
	inline void SetGuiMenu(const std::string& name) { currentMenu = menus.count(name) ? menus.at(name) : nullptr; }

	Options();
	virtual ~Options();

	enum Options_Menu
	{
		MAIN_OPTIONS,
		SOUND_OPTIONS,
		GAME_OPTIONS
	};
	int Button = MAIN_OPTIONS;

};