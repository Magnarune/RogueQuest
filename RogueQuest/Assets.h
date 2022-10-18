#pragma once

#include <iostream>
#include <string>
#include <set>
#include <functional>
#include <exception>
#include <map>
#include <vector>
#include <string>
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
        int Cost;
        int Food;
        std::vector<std::string> Requirements;
        std::map<std::string, TextureMetaData> texture_metadata;
        std::vector<std::string> task_abilities;
        sol::table lua_data;

        struct {
            olc::vi2d tl, sz;
            size_t tex_id;
        } head = {};
    };

    struct BuildingType {
        struct LevelOffset {
            olc::vi2d offset;
            olc::vi2d tile_size;
        };
        struct TextureMetaData {
            size_t tex_id;
            olc::vi2d target_size;
            olc::vf2d scale;
            std::map<std::string, LevelOffset> level_offsets;
        };
        int Cost;
        std::vector<std::string> Requirements;
        std::map<std::string, TextureMetaData> texture_metadata;
        sol::table lua_data;

        struct {
            olc::vi2d sz;
            olc::vi2d fsz;
            size_t tex_id;
        } icon = {};
    };

    struct ProjectileType {
        struct TextureMetaData {
            size_t tex_id;
            olc::vi2d target_size;
            olc::vf2d scale;
            bool Any;
        };
        std::map<std::string, TextureMetaData> texture_metadata;
        sol::table lua_data;
    };


    struct Cursor {
		std::unique_ptr<olc::Decal> decal;
		olc::vf2d scale;
	};


private:
    //Unit name, Unit data
    std::map<std::string, UnitType> unitCache;
    std::map<std::string, BuildingType> buildCache;
    std::map<std::string, ProjectileType> projCache;
	
    std::map<std::string, Cursor> assetCursorCache;

public:
	cAssets();
	~cAssets();

	olc::vi2d vTileSize;
    int Animation {};
    inline bool BuildingExists(const std::string& name) { return !!buildCache.count(name); }
    inline const BuildingType& GetBuildingData(const std::string& name) { return buildCache.at(name); }

    inline bool UnitExists(const std::string& name) { return !!unitCache.count(name); }
    inline const UnitType& GetUnitData(const std::string& name) { return unitCache.at(name); }
    
    inline bool ProjectileExists(const std::string& name) { return !!projCache.count(name); }
    inline const ProjectileType& GetProjectileData(const std::string& name) { return projCache.at(name); }

    void LoadUnitAssets();
    void LoadBuildingAssets();
    void LoadProjectileAssets();

    inline Cursor* GetCursor(const std::string& name) { return assetCursorCache.count(name) ? &(assetCursorCache.at(name)) : nullptr; }
	bool ImportCursor(const std::string& name, const std::string& path, const olc::vf2d& size = {16.f, 16.f});
    void LoadCursorAssets();
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
    friend class Building;
    friend class WorldManager;
};