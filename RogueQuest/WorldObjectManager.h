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
#include "clock.h"
#include <vector>
#include <memory>
#include <algorithm>
#include <exception>
#include <string>
#include <cmath>
#include <functional>
#include <cassert>
#include <optional>

// Forward Declare For Factory Functions
class Unit;
class Projectile;
class Building;
// -----------------

class WorldManager {
	std::vector<std::shared_ptr<Map>> mapList; // Maybe
	std::vector<std::shared_ptr<WorldObject>> objectList; // living objects
	std::vector<std::shared_ptr<WorldObject>> newObjectList; // new objects live here
	std::vector<std::reference_wrapper<std::shared_ptr<WorldObject>>> objectListView; // view transform on object list
	std::vector<std::shared_ptr<WorldObject>> garbageList;

	// vague variables will lead to broken code later down the line - either rename these or label them with a comment properly
	bool cu; // Garbage collector unit
	bool cb; // Garbage collector building

	void refreshObjectView();
	void addObjectToList(std::shared_ptr<WorldObject> obj);
	Clock worldclock;
public:
	std::shared_ptr<Map> currentMap;
	WorldManager();
	virtual ~WorldManager();
    
	void Update(float delta);
	
    void Draw();
	void DestroyObject(WorldObject* self);
	void CollectGarbage();
	void PopulateNewObjects();

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

	std::shared_ptr<Projectile> GenerateProjectile(const std::string& name,std::shared_ptr<WorldObject> pos, std::weak_ptr<WorldObject> trgt);

	friend class WorldObject;
	friend class Unit;
	friend class UnitManager;
	friend class Hud;
	friend class Projectile;
};