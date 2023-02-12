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
#include "olcSoundWaveEngine.h"
#include "sol/sol.hpp"
#include "Map.h"


class cAssets {
	sol::state lua_state;
public:

    struct UnitType { 
        struct TextureMetaData {
            size_t tex_id, ani_len;
            olc::vi2d sprite_size, tile_size, target_size;
            olc::vf2d draw_origin;
            std::vector<int> Sprite_Order;
            olc::vf2d scale;
            //unit sound manager [death, attack, move] 
        };
        struct SoundMetaData {
            size_t sound_id, sound_type;
        };


        //struct SoundMetaData {
        //    std::map<std::string, std::vector<olc::sound::Wave>> unit_sounds;// sound type  | random sound from waves

        //    std::vector<olc::sound::Wave> selected_sounds;
        //    std::vector<olc::sound::Wave> death_sounds;//i died
        //    std::vector<olc::sound::Wave> hurt_sounds;
        //    std::vector<olc::sound::Wave> attack_sounds;//Attack impact sounds
        //    std::vector<olc::sound::Wave> swing_sounds;//Swinging sword | pulling bow | shooting magic juice 
        //    std::vector<olc::sound::Wave> move_sounds; // Move talk
        //    std::vector<olc::sound::Wave> meme_sounds;//if you have a selected unit but afk for idk 5 min
        //};
        std::string Description;
        int Cost;
        int Food;
        std::string projectileName;
        std::vector<std::string> Requirements;
        std::map<std::string, TextureMetaData> texture_metadata;
        std::vector<std::string> task_abilities;
        sol::table lua_data;

        struct {
            olc::vi2d tl, sz;
            size_t tex_id;
        } head = {};
    };


    struct HeroType {
        struct TextureMetaData {
            size_t tex_id, ani_len;
            olc::vi2d sprite_size, tile_size, target_size;
            olc::vf2d draw_origin;
            std::vector<int> Sprite_Order;
            olc::vf2d scale;
        };
        std::string Description;        
        std::string projectileName;
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
            olc::vf2d draw_origin;
            olc::vf2d scale;
            std::map<std::string, LevelOffset> level_offsets;
        };
        std::string Description;
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
    
    struct ResearchType {
        struct {
            olc::vi2d sz;
            olc::vi2d fsz;
            size_t tex_id;
        } icon = {};
        std::vector<std::string> Requirements;
        std::string Description;
        int Cost;
        
        sol::table lua_data;
    };
    struct Cursor {
		std::unique_ptr<olc::Decal> decal;
		olc::vf2d scale;
	};
private:
            //Unit name, Object data
    std::map<std::string, HeroType> heroCache;
    std::map<std::string, UnitType> unitCache;
    std::map<std::string, BuildingType> buildCache;
    std::map<std::string, ProjectileType> projCache;
    std::map<std::string, ResearchType> resCache;
    std::map<std::string, Cursor> assetCursorCache;

public:
	cAssets();
	~cAssets();

	olc::vi2d vTileSize;
    int Animation {};
    inline bool BuildingExists(const std::string& name) { return !!buildCache.count(name); }
    inline const BuildingType& GetBuildingData(const std::string& name) { return buildCache.at(name); }

    inline bool HeroExists(const std::string& name) { return !!heroCache.count(name); }
    inline const HeroType& GetHeroData(const std::string& name) { return heroCache.at(name); }

    inline bool UnitExists(const std::string& name) { return !!unitCache.count(name); }
    inline const UnitType& GetUnitData(const std::string& name) { return unitCache.at(name); }
    
    inline bool ProjectileExists(const std::string& name) { return !!projCache.count(name); }
    inline const ProjectileType& GetProjectileData(const std::string& name) { return projCache.at(name); }

    inline bool ResearchExists(const std::string& name) { return !!resCache.count(name); }
    inline const ResearchType& GetResearchData(const std::string& name) { return resCache.at(name); }

    void LoadUnitAssets();
    void LoadHeroAssets();
    void LoadBuildingAssets();
    void LoadProjectileAssets();
    void LoadResearchAssets();

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
    friend class Research;
    friend class Building;
    friend class WorldManager;
    friend class CollisionMap;
};

class SoundManager {
    SoundManager();
       //     The object,    name of sound requested, Random sound from list of sounds
    std::pair<WorldObject,std::map<std::string, std::vector<olc::sound::Wave>>> object_sounds;//Objects in game's sounds

    std::map<std::string, olc::sound::Wave> game_sounds;//Game sounds name of sound = sound

    std::map<std::string, olc::sound::Wave> music_sounds; //Music sounds

    //std::vector<olc::sound::Wave> sound_instances;//What about music too Main menu, options menu,
    
    //so you have object id, object sound playing name;
    
    std::vector<olc::sound::PlayingWave> playing_sounds;
    
public:
    ~SoundManager();
    //System controls
    void Stop_all_sounds();
    void Stop_Game_Sound(int sound_id);
    void Master_Volume(float volume); //maybe have an into to decide what channel audio goes down
    uint64_t Play_sound_pack(const std::string& sound, void* link = nullptr); // For single event sounds E.g. "Death"  "Hurt"   "Selected" etc  
    
private://Only internally use
    void Play_Sound_Effect(olc::sound::Wave);
    void Play_Music(olc::sound::Wave);
    void Stop_Sound(olc::sound::Wave);

};