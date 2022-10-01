#include "Building.h"
#include "WorldObjectManager.h"

class BuildingManager {

    std::vector<std::weak_ptr<Building>> BuildingList; // copy of just units

    void addNewBuilding(std::weak_ptr<Building> Building); // world manager use only

    std::vector<std::weak_ptr<Unit>> selectedBuildings; // units currently selected - DO NOT USE EXTERNALY
public:
    BuildingManager();
    ~BuildingManager() { BuildingList.clear(); }

    void Update(float delta);
    void CollectGarbage();

    void StopBuilding();
    //void MoveUnits(olc::vf2d Target, bool Attackstate);
    void SelectBuilding(olc::vf2d Mouse);
    void SelectBuildings(olc::vf2d Initial, olc::vf2d Final);
    void DeselectBuildings();

    size_t GetSelectedBuildingCount();
    size_t GetBuildingCount(const std::string& name = "");
    std::shared_ptr<Unit> GetBuilding(const std::string& name, size_t index = 0);
    std::shared_ptr<Unit> GetBuilding(size_t index = 0);

    bool IterateSelectetBuildings(std::function<bool(std::shared_ptr<Unit>)> cb);
    bool IterateAllBuildings(std::function<bool(std::shared_ptr<Unit>)> cb);

    friend class WorldManager;

};