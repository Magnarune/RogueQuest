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
	std::vector<std::shared_ptr<Map>> mapList; // Maybe
	std::vector<std::shared_ptr<WorldObject>> objectList; // living objects
	std::vector<std::shared_ptr<WorldObject>> newObjectList; // new objects live here
	std::vector<std::reference_wrapper<std::shared_ptr<WorldObject>>> objectListView; // view transform on object list

	//QuadTree
	 // The object goes into the tree
	
	std::vector<std::shared_ptr<WorldObject>> sigSorter;//Sorts the QuadTreeList 
	std::vector<std::pair<std::shared_ptr<WorldObject>, olc::utils::geom2d::rect<float>>> newobjects;//new objects to be added
	olc::utils::QuadTreeContainer<std::shared_ptr<WorldObject>> quadtreeList; // The object goes into the tree

	olc::utils::geom2d::rect<float> screen;
	void refreshObjectView();
	void addObjectToList(std::shared_ptr<WorldObject> obj, olc::utils::geom2d::rect<float> size);


	std::vector<std::shared_ptr<WorldObject>> garbageList;
	Clock worldclock;
public:
	std::shared_ptr<Map> currentMap;
	WorldManager();
	virtual ~WorldManager();
    
	void Update(float delta);
	bool Checkonscreen(std::shared_ptr<WorldObject> obj);
    void Draw();
	void DestroyObject(WorldObject* self);
	void CollectGarbage();
	void PopulateNewObjects();

	//Additional
	std::shared_ptr<WorldObject> FindObject(size_t index = 0);
    
    bool IterateObjects(std::function<bool(std::shared_ptr<WorldObject>)> cb);

	bool IterateObjectQT(std::function<bool(std::shared_ptr<WorldObject>)> cb, olc::utils::geom2d::rect<float> searchsize);




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
	friend class CollisionMap;
};