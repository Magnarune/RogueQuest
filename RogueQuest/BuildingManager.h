#pragma once
#include <numeric>
#include <algorithm>
#include <functional>
#include <memory>
#include "Building.h"
#include "WorldObjectManager.h"
#include "TaskManager.h"
#include "Unit.h"
#include "UnitManager.h"
#include "CollisionMapObject.h"
class Unit;
class BuildingManager {
    std::vector<std::weak_ptr<Building>> BuildingList;
    std::vector<std::weak_ptr<Building>> selectedBuildings;

    TaskManager taskMgr;

    void addNewBuilding(std::weak_ptr<Building> Building);
public:
    BuildingManager();
    ~BuildingManager() = default;

    void Update(float delta);
    void CollectGarbage();

    void AssignTask(olc::vf2d Location,std::shared_ptr<Unit>& unit);


    std::shared_ptr<Building> This_Building(const olc::vf2d& Pos); // is this even used?
    void StopBuilding();
    //void MoveUnits(olc::vf2d Target, bool Attackstate);
    void SelectBuilding(olc::vf2d Mouse);
    void SelectBuildings(olc::vf2d Initial, olc::vf2d Final);
    void DeselectBuildings();

    size_t GetSelectedBuildingCount();
    size_t GetBuildingCount(const std::string& name = "");
    std::shared_ptr<Building> GetBuilding(const std::string& name, size_t index = 0);
    std::shared_ptr<Building> GetBuilding(size_t index = 0);

    void SentUnitslocation(olc::vf2d Target);

    bool IterateSelectedBuildings(std::function<bool(std::shared_ptr<Building>)> cb);
    bool IterateAllBuildings(std::function<bool(std::shared_ptr<Building>)> cb);

    friend class WorldManager;
    friend class TaskManager;
    friend class UnitManager;
};