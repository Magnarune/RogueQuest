#pragma once
#include "olcPixelGameEngine.h"
#include "olcPGEX_TransformedView.h"
#include <set>
#include <functional>
#include <vector>
#include "sol/sol.hpp"
#include "Assets.h"



class Map {
	sol::state lua;
    bool valid;
    std::vector<std::unique_ptr<olc::Decal>> decals;

public:
	Map();
	Map(const std::string& path);
	virtual ~Map();

    struct TileSet {
        olc::Decal* decal; // do not store this ever
        int gid;
    };

    //Import
    sol::table mapData, tileSets;
    //MetaData
    std::string name;
    int mapLayers;//number of map layers
    int tileSetsSize;//number of tileSets
    olc::vi2d offset;//Initial offset
    olc::vi2d layerSize;//Size of Layer
    std::vector<int> vFirstGid;//First Gid of each layer
    std::vector<std::vector<int>> layerData; // gid list representing the tile map
    olc::vi2d tileSize;//Size of tiles
    //Camera Update
    olc::vi2d topLeftTile;//Top left tile on screen
    olc::vi2d bottomRightTile;//Bottom right tile on screen

    //MapUpdate
    bool Cycle =false;
    float DayLength =0.f;
    float darkness_timer =0;
    olc::Pixel Darkness{0,0,0,0};//Inverse
    float NightLength =30.f;
    float DayNightPeriod = 4.f * 3.14159265f;
    const float TPI = 2.f * 3.14159265f;
    int PhaseofDay = 1;
    //Rain
    struct Rain
    {
        float Rains;
        olc::vf2d Position;
    };
    std::vector<Rain> RainDrops;
    olc::Sprite* Rain;
    olc::Decal* RainDcl;

    // private drawing methods
    TileSet GetTileSet(int id) const;
    olc::vi2d GetTile(int id) const;

public:
    void UpdateRain(float fElapsedTime);
    void UpdateTimeofDay(float fElapsedTime);
	void UpdateMap(float fElapsedTime);
    void DrawMap(olc::TileTransformedView* gfx);
    bool ImportMap(const std::string& path);
    
    inline bool isValid() { return valid; }

    friend class cAssets;
    friend class WorldManager;	
};
