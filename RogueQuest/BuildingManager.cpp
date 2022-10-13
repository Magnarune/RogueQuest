#include "BuildingManager.h"
#include "Engine.h"


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


void BuildingManager::SelectBuilding(olc::vf2d Mouse) {
    for (auto& _build : BuildingList) {
        if (_build.expired()) continue;
        auto build = _build.lock();
        
        if (build->bSelected) continue;
        const float& r = (build->Size.x /2.f);
        const float r2 = 0.55f;
        if (Mouse.x > build->Position.x + r2 && Mouse.y > build->Position.y + r2 &&
            Mouse.x < build->Position.x + build->Size.x - r2 &&
            Mouse.y < build->Position.y + build->Size.y - r2) {
            build->bSelected = true;
            selectedBuildings.push_back(_build);
            break;
        }
    }
}

void BuildingManager::SentUnitslocation(olc::vf2d Mouse) {
    for (auto& _build : selectedBuildings) {
        if (_build.expired()) continue;
        auto build = _build.lock();
        build->sentUnitPos = Mouse;
    }
}

void BuildingManager::DeselectBuildings() {
    for (auto& _build : selectedBuildings) {
        if (_build.expired()) continue;
        auto build = _build.lock();
        build->bSelected = false;       
    }
    selectedBuildings.clear();
}

size_t BuildingManager::GetSelectedBuildingCount() {
    return std::accumulate(BuildingList.begin(), BuildingList.end(), 0ULL,
        [&](size_t n, const auto& build) -> size_t {
            if(build.lock())
                return n + (build.lock()->bSelected == true);
        });
}

std::shared_ptr<Building> BuildingManager::GetBuilding(const std::string& name, size_t index) {
    size_t n = 0;
    for (auto& _build : BuildingList) {
        auto build = _build.lock();
        if (build->name == name && n++ == index) {
            return build;
        }
    }
    return nullptr;
}

std::shared_ptr<Building> BuildingManager::GetBuilding(size_t index) {
    size_t n = 0;
    for (auto& _build : selectedBuildings) {
        auto build = _build.lock();
        if (n++ == index) {
            return build;
        }
    }
    return nullptr;
}/*
    Building Manager Tasks For Selected Units - Will eventually be migrated into TaskManager
*/

void BuildingManager::StopBuilding() {
   
    for (auto& _building : selectedBuildings) {
        if (_building.expired()) continue;
        auto building = _building.lock();
        building->Stop();
    }
}

std::shared_ptr<Building> BuildingManager::This_Building(olc::vf2d pos) {
    std::shared_ptr<Building> thisBuild;
    for (auto& _build: BuildingList) {
        if (_build.expired())
            continue;
        auto build = _build.lock();

        if (build->Position == pos) {
            thisBuild = build;
        }
    }
    return thisBuild;
}