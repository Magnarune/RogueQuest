#include "UnitManager.h"
#include "Engine.h"


UnitManager::UnitManager() {
    unitList.reserve(size_t(1024) * 1024); // preserve lots of unit ptrs

    // task registration looks similar to
    // lua scripts for units should also have a list to associate the Allowed Task Names
    taskMgr.RegisterTask("Move",
        { [&](std::shared_ptr<TaskManager::Task> task) -> bool {
            const auto& params = std::any_cast<std::pair<olc::vf2d,bool>>(task->data);
            const olc::vf2d& target = params.first;
            const bool& attackstate = params.second;

            for (auto& _unit : selectedUnits) {
                if (_unit.expired()) continue;
                auto unit = _unit.lock();
                if (unit->bFriendly == true) {
                    unit->ULogic = attackstate ? unit->Attack : unit->Neutral;
                    unit->MarchingtoTarget(target);
                    unit->currentTask = task;
                }
            }
            return true;
        }, 0});
}

// internal do not touch
void UnitManager::addNewUnit(std::weak_ptr<Unit> unit) {
    unitList.emplace_back(unit);
}

void UnitManager::Update(float delta) {
}

// GC do not touch
void UnitManager::CollectGarbage() {
    auto it = std::find_if(unitList.begin(), unitList.end(), [](const auto& unit){ return unit.expired(); });
    if(it++ != unitList.end()){
        std::vector<std::weak_ptr<Unit>> copyList;
        for(auto& unit : unitList){
            if(!unit.expired()) copyList.emplace_back(std::move(unit));
        }
        unitList = std::move(copyList);
    }
}

// Get methods
size_t UnitManager::GetUnitCount(const std::string& name) {
    return name == "" ? unitList.size() :
        std::accumulate(unitList.begin(), unitList.end(), 0ULL,
            [&](size_t n, const auto& unit) -> size_t {
                return n + (unit.lock()->sUnitName == name);
            });
}

size_t UnitManager::GetSelectedUnitCount() {
    return std::accumulate(unitList.begin(), unitList.end(), 0ULL,
        [&](size_t n, const auto& unit) -> size_t {
            return n + (unit.lock()->bSelected == true);
        });
}

std::shared_ptr<Unit> UnitManager::GetUnit(const std::string& name, size_t index) {
    size_t n = 0;
    for(auto& _unit : unitList){
        auto unit = _unit.lock();
        if(unit->sUnitName == name && n++ == index){
            return unit;
        }
    }
    return nullptr; // couldn't find unit
}

std::shared_ptr<Unit> UnitManager::GetUnit(size_t index) {
    size_t n = 0;
    for(auto& _unit : selectedUnits){
        auto unit = _unit.lock();
        if(n++ == index){
            return unit;
        }
    }
    return nullptr; // couldn't find unit
}
// Public method for selecting unit
void UnitManager::SelectUnit(olc::vf2d mousePos) {
    for (auto& _unit : unitList) {
        auto unit = _unit.lock();
        if(unit->bSelected) continue;

        const float &r = unit->Unit_Collision_Radius;
        const float r2 = 0; // extra collision distance

        if( (unit->Position - mousePos).mag2() < (r*r + r2*r2)){
            unit->bSelected = true;
            selectedUnits.push_back(_unit);
            break;
        }
    }
}
// Public method for selecting units
void UnitManager::SelectUnits(olc::vf2d Initial, olc::vf2d Final) {    
    for (auto& _unit : unitList){
        auto unit = _unit.lock();
        if(unit->bSelected) continue;

        if (unit->Position.x > std::min(Initial.x, Final.x) && 
            unit->Position.y > std::min(Initial.y, Final.y) &&
            unit->Position.x < std::max(Final.x, Initial.x) &&
            unit->Position.y < std::max(Final.y, Initial.y) &&
            unit->bFriendly) 
        {
           unit->bSelected = true;
           selectedUnits.push_back(_unit);
        }
    }
}

// Public method for deselecting all units
void UnitManager::DeselectUnits() {
    for(auto& _unit : selectedUnits){
        if(_unit.expired()) continue;
        auto unit = _unit.lock();
        unit->bSelected = false;
    }
    selectedUnits.clear();
}

bool UnitManager::IterateSelectedUnits(std::function<bool(std::shared_ptr<Unit>)> cb) {
    for(auto& unit : selectedUnits) {
        if(unit.expired()) continue;
        if(!cb(unit.lock())){
            return false; // user abort
        }
    }
    return true; // iterate completed successfully
}

bool UnitManager::IterateAllUnits(std::function<bool(std::shared_ptr<Unit>)> cb) {
    for(auto& unit : unitList) {
        if(unit.expired()) continue;
        if(!cb(unit.lock())){
            return false; // user abort
        }
    }
    return true; // iterate completed successfully
}



/*
    Unit Manager Tasks For Selected Units - Will eventually be migrated into TaskManager
*/
void UnitManager::StopUnits() {
    auto& engine = Game_Engine::Current();
    // all being replaced by a new and improved task delegation system
    for (auto& _unit : engine.unitManager->selectedUnits) {
        if (_unit.expired()) continue;
        auto unit = _unit.lock();
        unit->Stop();
    }
}

void UnitManager::MoveUnits(olc::vf2d Target, bool attackstate) {
    auto& engine = Game_Engine::Current();
    // all being replaced by a new and improved task delegation system
    for (auto& _unit : engine.unitManager->selectedUnits) {
        if (_unit.expired()) continue;
        auto unit = _unit.lock();
        if (unit->bFriendly == true) {
            unit->ULogic = attackstate ? unit->Attack : unit->Neutral;
            unit->MarchingtoTarget(Target);
        }
    }
}

void UnitManager::MoveConstructBuilding(const std::string& buildingName, const olc::vf2d& Target) {
    auto& engine = Game_Engine::Current();
    // all being replaced by a new and improved task delegation system
    for (auto& _unit : engine.unitManager->selectedUnits) {
        if (_unit.expired()) continue;
        auto unit = _unit.lock();
        if (unit->bFriendly == true) {
            unit->ULogic = unit->Neutral;
            unit->MarchingtoTarget(Target);
        }
    }
}