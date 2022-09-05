#pragma once

#include <iostream>
#include <string>
#include <set>
#include <functional>
#include <exception>
#include <map>
#include <vector>
#include <assert.h>

#include "olcPixelGameEngine.h"
#include "sol/sol.hpp"

#include "Unit.h"


class cAssets
{
	sol::state lua_state;

    struct UnitType {
        std::map<std::string, size_t> tex_ids;
        std::map<std::string, size_t> ani_lens;
        sol::table lua_data;
    };

    //Unit name, Unit data
    std::map<std::string, UnitType> assetCache;

public:
	cAssets();
	~cAssets();

	olc::vi2d vTileSize;
    int Animation;

    inline const UnitType& GetUnitData(const std::string& Name) { return assetCache[Name]; }
  
    void LoadUnitAssets();

};


class TextureCache {
    TextureCache();
    static TextureCache* self;


    std::vector<olc::Sprite*> textures;

public:
    ~TextureCache();

    static void InitCache();
    static void FreeCache();
    
    static inline TextureCache& GetCache() { assert(self != nullptr); return *self; }


    size_t CreateTexture(const std::string& path); // create texture and load from file
    olc::Sprite* GetTexture(size_t texid); // get the sprite from the texture id
};