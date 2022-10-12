#pragma once

#include "olcPixelGameEngine.h"
#include "olcPGEX_TransformedView.h"
#include "WorldObject.h"
// #include "Unit.h"
#include "UnitManager.h"
#include "Map.h"
// #include "Building.h"
#include "BuildingManager.h"
// #include "Projectiles.h"
#include <vector>
#include <memory>
#include <algorithm>
#include <exception>
#include <string>
#include <cmath>
#include <functional>
#include <cassert>

// Forward Declare For Factory Functions
class Unit;
class Projectile;
class Building;
// -----------------

class WorldManager {
	std::vector<std::shared_ptr<Map>> mapList; // Maybe
	std::vector<std::shared_ptr<WorldObject>> objectList; // fixed size for optimization
	std::vector<std::shared_ptr<WorldObject>> garbageList;
	bool cu;
	bool cb;

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

	std::vector<int> InitializeObject(int owner);

	inline const Map& curMap() { assert(currentMap.get() != nullptr); return *currentMap; } // do not store

	std::shared_ptr<Unit> GenerateUnit(const std::string& name,  int owner,olc::vf2d pos={0.f, 0.f});

	std::shared_ptr<Building> GenerateBuilding(const std::string& name, int owner, olc::vf2d pos = { 0.f,0.f });

	std::shared_ptr<Projectile> GenerateProjectile(const std::string& name, olc::vf2d pos, std::weak_ptr<WorldObject> trgt);
	
	friend class WorldObject;
	friend class Unit;
	friend class UnitManager;
	friend class Hud;
	friend class Projectile;
};