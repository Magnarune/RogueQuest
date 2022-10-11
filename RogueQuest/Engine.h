#pragma once

#include <iostream>
#include <string>
#include <algorithm>
#include <map>
#include "olcPixelGameEngine.h"
#include "olcPGEX_TransformedView.h"
#include "sol/sol.hpp"
#include "Building.h"
#include "BuildingManager.h"
#include "UserInput.h"
#include "UserInputManager.h"
#include "Unit.h"
#include "UnitManager.h"
#include "Assets.h"
#include "Hud.h"
#include "Map.h"
#include "HudManager.h"
#include "TaskManager.h"
#include "WorldObject.h"
#include "WorldObjectManager.h"
#include "Particles.h"
#include "Projectiles.h"
#include "Config.h"
#include "Options.h"
#include "clock.h"


extern std::string StringifyObject(sol::object val);

class Game_Engine : public olc::PixelGameEngine {
	static Game_Engine* self;

	bool bIsLocked;
	bool bWasLocked = false;
public:
    Game_Engine();
    virtual ~Game_Engine();

	Game_Engine(const Game_Engine&) = delete;
	Game_Engine& operator=(const Game_Engine&) = delete;

	cAssets::Cursor* curCursor = nullptr;

	float StandardTime= 0.0f;

	static inline Game_Engine& Current() { assert(self != nullptr); return *self; }
	olc::TileTransformedView tv;

	std::unique_ptr<Config> config;

	//Cursor BS
	RECT my_rect;

    enum MapModes
    {
		MODE_LOCAL_MAP,
		MODE_OPTIONS_MENU
    };
    int m_nGameMode = MODE_LOCAL_MAP;
protected:
    virtual bool OnUserCreate() override;
    virtual bool OnUserUpdate(float fElapsedTime) override;   
	virtual bool OnUserDestroy() override;
	
	
	bool _lastfocus = true;
	void OnFocusUpdated(bool focus);

    virtual bool UpdateLocalMap(float fElapsedTime);
	virtual bool UpdateOptions(float fElapsedTime);
	virtual void DrawCursor();
public:
	std::unique_ptr<WorldManager> worldManager;
	std::unique_ptr<UnitManager> unitManager;
	std::unique_ptr<BuildingManager> buildingManager;
	std::unique_ptr<cAssets> assetManager;
	std::unique_ptr<Hud> hud;
	std::unique_ptr<HudManager> hudManager;
	std::unique_ptr<Particles> particles;
	std::unique_ptr<Options> optionsManager;
	std::unique_ptr<UserInput> userinputs;
	std::unique_ptr<UserInputManager> inputmanager;

	bool ActivityDone;
	struct Player{
		olc::vf2d vPOS;
		olc::vf2d vVel;		
	};
	Player Camera;
	std::weak_ptr<Unit> followUnit;

	inline void ChangeCursor(const std::string& name) { auto c = assetManager->GetCursor(name); curCursor = c == nullptr ? curCursor : c; }
	
	inline bool GetLocked() { return bIsLocked; }
	void SetLocked(bool locked, bool permanent=true);

	bool OnConsoleCommand(const std::string& stext) override;
	friend class Hud;
	friend class Projectile;
	
};
