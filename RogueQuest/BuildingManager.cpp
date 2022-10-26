#include "BuildingManager.h"
#include "Engine.h"


BuildingManager::BuildingManager() {
	BuildingList.reserve(100);
  
}


void BuildingManager::Update(float delta) {
    auto& engine = Game_Engine::Current();
    engine.leaders->BuildingMaintenance(BuildingList);
}

void BuildingManager::addNewBuilding(std::weak_ptr<Building> Building) {
	BuildingList.emplace_back(Building);
}

void  BuildingManager::CollectGarbage() {
	std::erase_if(BuildingList, [](const auto& build) { return build.expired(); });
}

void BuildingManager::SelectBuilding(olc::vf2d Mouse) {
    for (auto& _build : BuildingList) {
        if (_build.expired()) continue;
        auto build = _build.lock();
        const auto& meta = build->textureMetadata.at(build->Graphic_State);
        if (build->bSelected) continue;
        const float& r = (build->Size.x /2.f);
        const float r2 = 0.55f;
        if (Mouse.x > build->Position.x - meta.draw_origin.x  && 
            Mouse.y > build->Position.y - meta.draw_origin.y  &&
            Mouse.x < build->Position.x + meta.draw_origin.x  &&
            Mouse.y < build->Position.y + meta.draw_origin.y) {
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
            return n + size_t(!build.expired() && build.lock()->bSelected);
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
}

bool BuildingManager::IterateAllBuildings(std::function<bool(std::shared_ptr<Building>)> cb) {
    for(auto& build : BuildingList) {
        if(build.expired()) continue;
        if(!cb(build.lock())){
            return false; // user abort
        }
    }
    return true; // iterate completed successfully
}

bool BuildingManager::IterateSelectedBuildings(std::function<bool(std::shared_ptr<Building>)> cb) {
    for(auto& build : selectedBuildings) {
        if(build.expired()) continue;
        if(!cb(build.lock())){
            return false; // user abort
        }
    }
    return true; // iterate completed successfully
}

/*
    Building Manager Tasks For Selected Units - Will eventually be migrated into TaskManager
*/
void BuildingManager::AssignTask(olc::vf2d Location, std::shared_ptr<Unit>& unit) {
    auto& engine = Game_Engine::Current();
    std::weak_ptr<Unit> testunit;
    std::weak_ptr<Building> testbuilding;
    engine.unitManager->ParseObject(engine.unitManager->FindObject(Location), testbuilding, testunit);        
        const auto& abilities = unit->unitType.task_abilities;
        if (testbuilding.expired() && testunit.expired()) {
            if (std::find(abilities.begin(), abilities.end(), "Move") != abilities.end())
            unit->taskQueue.push(engine.unitManager->taskMgr.PrepareTask("Move", std::pair<std::shared_ptr<Unit>, std::any>{unit, std::pair<olc::vf2d, bool> {Location, true} }));               
        }
        if (testbuilding.lock()) {
            if (testbuilding.lock()->Owner == unit->Owner) {//If it was an ally

                if (std::find(abilities.begin(), abilities.end(), "Gather") != abilities.end() && testbuilding.lock()->isMine)
                    unit->taskQueue.push(engine.unitManager->taskMgr.PrepareTask("Gather", std::pair<std::shared_ptr<Unit>, std::any>{unit,
                        std::pair< std::weak_ptr<Building>, olc::vf2d> {testbuilding, Location} }));
                else
                    if (std::find(abilities.begin(), abilities.end(), "Repair") != abilities.end())
                        unit->taskQueue.push(engine.unitManager->taskMgr.PrepareTask("Repair", std::pair<std::shared_ptr<Unit>, std::any>{unit,
                            std::pair< std::weak_ptr<Building>, olc::vf2d> {testbuilding, Location} }));                       
                    else
                        if (std::find(abilities.begin(), abilities.end(), "Move") != abilities.end())
                            unit->taskQueue.push(engine.unitManager->taskMgr.PrepareTask("Move", std::pair<std::shared_ptr<Unit>, std::any>{unit, std::pair<olc::vf2d, bool> {Location, true} }));
            } else
                if (std::find(abilities.begin(), abilities.end(), "Move") != abilities.end())
                    unit->taskQueue.push(engine.unitManager->taskMgr.PrepareTask("Move", std::pair<std::shared_ptr<Unit>, std::any>{unit, std::pair<olc::vf2d, bool> {Location, true} }));
        } else
            if (std::find(abilities.begin(), abilities.end(), "Move") != abilities.end())
                unit->taskQueue.push(engine.unitManager->taskMgr.PrepareTask("Move", std::pair<std::shared_ptr<Unit>, std::any>{unit, std::pair<olc::vf2d, bool> {Location, true} }));

}

void BuildingManager::StopBuilding() {
   
    for (auto& _building : selectedBuildings) {
        if (_building.expired()) continue;
        auto building = _building.lock();
        building->Stop();
    }
}

std::shared_ptr<Building> BuildingManager::This_Building(const olc::vf2d& pos) {
    std::shared_ptr<Building> thisBuild;
    IterateAllBuildings([&](auto build) -> bool {
        if (build->Position == pos) {
            thisBuild = build;
            return false;
        }
        return true;
    });
    return thisBuild;
}