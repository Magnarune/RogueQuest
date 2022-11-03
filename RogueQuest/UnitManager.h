#pragma once
#include <numeric>
#include <algorithm>
#include <functional>
#include <memory>
//#include "Engine.h"
#include "Hud.h"
#include "HudManager.h"
#include "Unit.h"
#include "Building.h"
#include "TaskManager.h"
#include "Assets.h"
#include "Projectiles.h"
#include "WorldObject.h"
#include "WorldObjectManager.h"
#include "CollisionMapObject.h"
#include <queue>

class UnitManager {
    std::vector<std::weak_ptr<Unit>> unitList; // copy of just units

    void addNewUnit(std::weak_ptr<Unit> unit); // world manager use only

    std::vector<std::weak_ptr<Unit>> selectedUnits; // units currently selected - DO NOT USE EXTERNALY
public:
    UnitManager();
    ~UnitManager() = default;

    void Update(float delta);
    void CollectGarbage();
	
	TaskManager taskMgr;

	void DelegateTask(const std::string& name, const std::any& data);

	void ConditionedDelegateTask(std::shared_ptr<Unit> unit, const std::string& name, const std::any& data);

	void CheckTaskAbility(std::shared_ptr<Collidable> object , bool A_Click);

	std::shared_ptr<Unit> This_shared_pointer(olc::vf2d pos);
	void StopUnits();
	void MoveConstructBuilding(const std::string& buildingName, const olc::vf2d& Target);
    void MoveUnits(olc::vf2d Target , bool Attackstate);
	void SelectUnit(olc::vf2d Mouse);
    void SelectUnits(olc::vf2d Initial, olc::vf2d Final);
	//prob a better way

	std::shared_ptr<Collidable> FindObject(olc::vf2d Mouse);
	void ParseObject(std::shared_ptr<Collidable> object, std::weak_ptr<Building>& build, std::weak_ptr<Unit>& unit, std::weak_ptr<CollisionMapObject>& tree);

	std::vector<std::shared_ptr<Collidable>> testobjects;

	std::vector<std::shared_ptr<Collidable>> FindObjects(olc::vf2d pos,float Radius);
	void ParseObjects(std::vector<std::shared_ptr<Collidable>> object, std::queue<std::weak_ptr<Building>>& build, std::queue<std::weak_ptr<Unit>>& unit);

	std::shared_ptr<Collidable> SearchClosestEnemy(int owner,olc::vf2d pos, float Radius);

	void ParseSearch(std::shared_ptr<Collidable> object, std::weak_ptr<Building>& build, std::weak_ptr<Unit>& unit);

	bool CheckBuildObstruction(std::shared_ptr<Building> potBuilding);

	void DeselectUnits();

	bool FindHomeBase(std::shared_ptr<Unit>& unit);

	size_t GetSelectedUnitCount();
    size_t GetUnitCount(const std::string& name="");
    std::shared_ptr<Unit> GetUnit(const std::string& name, size_t index=0);
    std::shared_ptr<Unit> GetUnit(size_t index=0);

	bool IterateSelectedUnits(std::function<bool(std::shared_ptr<Unit>)> cb);
	bool IterateAllUnits(std::function<bool(std::shared_ptr<Unit>)> cb);
	olc::vf2d ArrangeSelectedUnits(int Size, int interator);
	

	
	int numberofselectedunits;
	int Columnoffset;
    friend class WorldManager;
	friend class TaskManager;
	friend class LeaderManager;
	friend class Game_Engine;
};