#pragma once

#include <iostream>
#include <string>
#include <map>
#include "olcPixelGameEngine.h"
#include "olcPGEX_TransformedView.h"
#include "sol/sol.hpp"
#include "Unit.h"
#include "Assets.h"
#include "UnitManager.h"
#include "clock.h"

class Game_Engine : public olc::PixelGameEngine
{

	static Game_Engine* self;

public:
    Game_Engine();
    virtual ~Game_Engine();
	float StandardTime= 0.0f;
	// give the audience a way to access the game engine instance from anywhere - we need it
	static inline Game_Engine& Current() { assert(self != nullptr); return *self; }
	olc::TileTransformedView tv;
    //lua_State* state;
	sol::table mapLayers; //Mine
    sol::state lua;
	sol::table mapData, tileSets;
	olc::vi2d vTileSize;
    std::vector<olc::Renderable*> renderGraphics; //For Background
	//Map Logic
	std::vector<int> vLayer;
	std::vector<std::vector<int>> vLayerData;
	sol::table layer;
	sol::table data;
	olc::vi2d offset;
	olc::vi2d layerSize;
	olc::vi2d pos;
	std::vector<std::string> vTileSet;
	std::vector<int> vFirstGid;
	olc::vi2d TopleftTile;
	olc::vi2d BottomeRightTile;
	struct TileSet {
	        olc::Renderable* Rfx;
	        int gid;
	};
	
    //Selection click
	olc::vi2d Initial;
	bool Clicked = false;
    
private:
    enum MapModes
    {
        MODE_LOCAL_MAP
    };
private:
    int m_nGameMode = MODE_LOCAL_MAP;
    TileSet GetTileSet(int id);
    olc::vi2d GetTile(int id);
    virtual void UpdateRect(olc::vi2d Initial, olc::vi2d Finial);
protected:
    bool OnUserCreate() override;
    bool OnUserUpdate(float fElapsedTime) override;
    bool OnUserDestroy() override;
    bool UpdateLocalMap(float fElapsedTime);

public:
	// Unit Manager
	std::unique_ptr<UnitManager> unitManager;
	// Asset Manager
	std::unique_ptr<cAssets> assetManager;


	//Time of Day code
	float TimeofDay= 101;
	float FTIME = 0;
	bool DayTime = true;
	bool NightTime = false;

	struct Player //Debug camera
	{
			olc::vf2d vPOS;
			olc::vf2d vVel;
			
	};
	Player object;
	std::weak_ptr<Unit> followUnit;

		//Rain
	struct Rain
	{
		float Rains;
		olc::vf2d Position;	
	};
	std::vector<Rain> RainDrops;
	olc::Sprite* Rain;
	olc::Decal* RainDcl;
	void UpdateRain(float FelapsedTime);
	//Debug Commands
	bool OnConsoleCommand(const std::string& stext) override;
};