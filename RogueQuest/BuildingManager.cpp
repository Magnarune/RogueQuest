#include "BuildingManager.h"
BuildingManager::BuildingManager() {
	BuildingList.reserve(100);
}

void BuildingManager::addNewBuilding(std::weak_ptr<Building> Building) {
	BuildingList.emplace_back(Building);
}

void  BuildingManager::CollectGarbage() {
    auto it = std::find_if(BuildingList.begin(), BuildingList.end(), [](const auto& building) { return building.expired(); });
    if (it++ != BuildingList.end()) {
        std::vector<std::weak_ptr<Building>> copyList;
        for (auto& building : BuildingList) {
            if (!building.expired()) copyList.emplace_back(std::move(building));
        }
        BuildingList = std::move(copyList);
    }
}
