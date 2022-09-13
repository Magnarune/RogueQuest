#include "UnitManager.h"
#include "Engine.h"


UnitManager::UnitManager() {
    unitList.reserve(1024 * 1024); // preserve lots of unit ptrs
}

void UnitManager::addNewUnit(std::weak_ptr<Unit> unit) {
    unitList.emplace_back(unit);
}

void UnitManager::Update() { // meh will work on later to improve :(
}

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

size_t UnitManager::GetUnitCount(const std::string& name) {//got it
    return std::accumulate(unitList.begin(), unitList.end(), 0ULL,
        [&](size_t n, const auto& unit) -> size_t {
            return n + (unit.lock()->sUnitName == name);
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
void UnitManager::SelectUnits(olc::vf2d Initial, olc::vf2d Final) {    
    Game_Engine& engine = Game_Engine::Current();
    engine.tv.DrawLineDecal(Final,   { Initial.x,Final.y }, olc::RED);//Draw Rectangle
    engine.tv.DrawLineDecal(Initial, { Initial.x,Final.y }, olc::RED);
    engine.tv.DrawLineDecal(Initial, { Final.x,Initial.y }, olc::RED);
    engine.tv.DrawLineDecal(Final,   { Final.x,Initial.y }, olc::RED);

    for(auto& _unit : selectedUnits){
        if(_unit.expired()) continue;
        auto unit = _unit.lock();
        unit->bSelected = false;
    }
    selectedUnits.clear();

    for (auto& _unit : unitList){
        auto unit = _unit.lock();
        if (unit->Position.x > std::min(Initial.x, Final.x) && //Are you the Man in the box?
            unit->Position.y > std::min(Initial.y, Final.y) &&
            unit->Position.x < std::max(Final.x, Initial.x) &&
            unit->Position.y < std::max(Final.y, Initial.y) &&
            unit->bFriendly) // do not allow the selecting of enemies
        {
           unit->bSelected = true;
           selectedUnits.push_back(_unit);
        }
    }
}

void UnitManager::MoveUnits(olc::vf2d Target, bool attackstate){
    for (auto& _unit : selectedUnits){
        if(_unit.expired()) continue;
        auto unit = _unit.lock();

        unit->ULogic = attackstate ? unit->Attack : unit->Neutral;
        unit->MarchingtoTarget(Target);
    }
}