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
#include "sol/sol.hpp"
#include "olcPixelGameEngine.h"
#include "WorldObject.h"
#include "Map.h"
#include "CollisionMapObject.h"
class CollisionMap{
	sol::state lua_state;
	std::map<std::string, olc::vf2d> CollisionBox;// {Name of object and Object Size X and -Y; Bottom left tile
	std::unique_ptr<Map> map;
	std::vector<std::unique_ptr<olc::Decal>> decals;
public:
	std::vector<int> CLayerdata;//Collision layer posisions
	virtual ~CollisionMap();
	void ImportCollisionData();// Take 2D array of map 
	void AssignCollisionTextures();
	void GenerateCollisionObject(const std::string& name, olc::vf2d pos);//Puts the rocks and trees and rivers on the map
	void PlaceMapObjects();
	bool FindMapObject(std::vector<int> Object_ids,olc::vf2d shape,int position);

	CollisionMap();
	struct CollidableMapObject {
		struct TreeType {
			std::vector<int> textureids;
			std::vector<int> tileids;
			int tilesize;
			int tilemapwidth;
			olc::vi2d ObjectShape;
			olc::vi2d Origin;
			olc::vf2d CollisionOrigin;
			olc::vf2d collisionbox;
		};
		size_t TreeTextureID;
		std::unique_ptr<olc::Decal> TreeDecal;
		std::map<std::string, TreeType> TreeTypes; //list of all TreeTypes
	};
	
	CollidableMapObject CMapObjectCache;
	friend class Map;
};



class MapTextureCache {
	MapTextureCache();
	static MapTextureCache* self;
	std::vector<olc::Sprite*> Maptextures;

public:
	~MapTextureCache();
	static void InitCache();
	static void FreeCache();
	static inline MapTextureCache& GetCache() { assert(self != nullptr); return *self; }
	size_t CreateTexture(const std::string& path); // create texture and load from file
	olc::Sprite* GetTexture(size_t texid); // get the sprite from the texture id
	friend class CollisionMap;
	friend class WorldManager;
};