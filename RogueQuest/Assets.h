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
#include "Map.h"


class cAssets {
	sol::state lua_state;
public:

    struct UnitType {        
        struct TextureMetaData {
            size_t tex_id, ani_len;
            olc::vi2d sprite_size, tile_size, target_size;
            std::vector<int> Sprite_Order;
            olc::vf2d scale;
        };
        std::map<std::string, TextureMetaData> texture_metadata;
        sol::table lua_data;

        struct {
            olc::vi2d tl, sz;
            size_t tex_id;
        } head;
    };

private:
    //Unit name, Unit data
    std::map<std::string, UnitType> assetCache;
public:
	cAssets();
	~cAssets();

	olc::vi2d vTileSize;
    int Animation;

    inline bool UnitExists(const std::string& name) { return !!assetCache.count(name); }
    inline const UnitType& GetUnitData(const std::string& name) { return assetCache[name]; }    
  
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