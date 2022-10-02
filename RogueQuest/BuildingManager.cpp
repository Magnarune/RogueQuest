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
        auto build = _build.lock();
        if (build->bSelected) continue;

        const float& r = (build->Size.x /2.f);
        const float r2 = 0; // extra collision distance
        
        if ((build->pos + build->Size /2.f - Mouse).mag2() < (r*r + r2 * r2)) {
            build->bSelected = true;
            selectedBuildings.push_back(_build);
            break;
        }
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
    return nullptr; // couldn't find unit
}