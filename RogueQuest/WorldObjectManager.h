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
//Maybe
#include <list>
#include "olcUTIL_Geometry2D.h"
#include "olcUTIL_QuadTree.h"

// Forward Declare For Factory Functions
class Unit;
class Projectile;
class Building;
// -----------------

class WorldManager {
	std::vector<std::shared_ptr<Map>> mapList; //List of maps to play on
	std::vector<std::shared_ptr<WorldObject>> newObjectList; // new objects live here
	// std::vector<std::shared_ptr<WorldObject>> objectList; // living objects - old
	olc::utils::QuadTreeContainer<std::shared_ptr<WorldObject>> objectList; // living objects
	std::vector<std::reference_wrapper<std::shared_ptr<WorldObject>>> objectListView; // view transform on object list
	int World_Object_id;

	olc::utils::geom2d::rect<float> screen;
	void refreshObjectView();
	void addObjectToList(std::shared_ptr<WorldObject> obj);

	std::vector<std::shared_ptr<WorldObject>> garbageList;
	Clock worldclock;
public:
	std::shared_ptr<Map> currentMap;
	WorldManager();
	virtual ~WorldManager();
    
	void Update(float delta);
    void Draw();
	void DestroyObject(WorldObject* self);
	void DestroyObject(const std::shared_ptr<WorldObject>& self);
	void CollectGarbage();
	void PopulateNewObjects();
	void RelocateObject(std::shared_ptr<WorldObject> self);

	//Additional 
    bool IterateObjects(std::function<bool(std::shared_ptr<WorldObject>&)> cb);

	bool IterateObjectsQT(olc::utils::geom2d::rect<float> searchArea, std::function<bool(std::shared_ptr<WorldObject>&)> cb);
	olc::utils::geom2d::rect<float> getObjectQuadTreeArea(const std::shared_ptr<WorldObject>& obj) const;



	// Class Specific Factory Functions'
	void ImportMapData();
	bool ChangeMap(const std::string& name);

	std::vector<int> InitializeObject(int owner);

	inline const Map& curMap() { assert(currentMap.get() != nullptr); return *currentMap; } // do not store


	std::shared_ptr<Unit> GenerateUnit(const std::string& name,  int owner,olc::vf2d pos={0.f, 0.f});

	std::shared_ptr<Building> GenerateBuilding(const std::string& name, int owner, olc::vf2d pos = { 0.f,0.f });

	std::shared_ptr<Projectile> GenerateProjectile(const std::string& name,std::shared_ptr<WorldObject> pos, std::weak_ptr<WorldObject> trgt);

	std::shared_ptr<Projectile> GenerateProjectile(std::shared_ptr<Unit> HeroName,float angle);

	friend class WorldObject;
	friend class Unit;
	friend class UnitManager;
	friend class Hud;
	friend class Projectile;
	friend class CollisionMap;
};