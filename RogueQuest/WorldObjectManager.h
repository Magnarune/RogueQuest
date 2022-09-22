#pragma once

#include "olcPixelGameEngine.h"
#include "olcPGEX_TransformedView.h"
#include "WorldObject.h"
#include "Unit.h"
#include "Map.h"
#include "Projectiles.h"
#include <vector>
#include <memory>
#include <algorithm>
#include <exception>
#include <string>
#include <functional>
#include <cassert>


class WorldManager {
	std::vector<std::shared_ptr<Map>> mapList; // Maybe
	std::vector<std::shared_ptr<WorldObject>> objectList; // fixed size for optimization
	std::vector<std::shared_ptr<WorldObject>> garbageList;

	std::shared_ptr<Map> currentMap;
public:
	WorldManager();
	virtual ~WorldManager();
    
	void Update(float delta);
	
    void Draw();
	void DestroyObject(WorldObject* self);
	void CollectGarbage();

	//Additional
	std::shared_ptr<WorldObject> FindObject(size_t index = 0);
    
    bool IterateObjects(std::function<bool(std::shared_ptr<WorldObject>)> cb);

	// Class Specific Factory Functions'
	void ImportMapData();
	bool ChangeMap(const std::string& name);

	inline const Map& curMap() { assert(currentMap.get() != nullptr); return *currentMap; } // do not store

	std::shared_ptr<Unit> GenerateUnit(const std::string& name, olc::vf2d pos={0.f, 0.f});
    std::shared_ptr<Projectile> GenerateProjectile(olc::vf2d pos, olc::vf2d trgt);
	friend class WorldObject;
};