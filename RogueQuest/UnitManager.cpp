#include "UnitManager.h"

#include "Engine.h"


UnitManager::UnitManager() {
    unitList.reserve(size_t(1024) * 1024); // preserve lots of unit ptrs

    // task registration looks similar to
    // lua scripts for units should also have a list to associate the Allowed Task Names

    // Eveything you want the Unit to be able to do, goes here:
    // Hotkey doesn't work yet
    auto& engine = Game_Engine::Current();
    taskMgr.RegisterTask("Move",
        { [&](std::shared_ptr<TaskManager::Task> task) -> bool { // task begin
            // required
            auto arguments = std::any_cast<std::pair<std::shared_ptr<Unit>,std::any>>(task->data);
            auto& unit = arguments.first;
            // customizable parameters
            const auto& params = std::any_cast<std::pair<olc::vf2d,bool>>(arguments.second);
            const olc::vf2d& target = params.first;
            const bool& attackstate = params.second;
            // action code            
           // unit->ULogic = attackstate ? unit->Attack : unit->Neutral;
            unit->ActionZone.x = 8.f;
            unit->ActionZone.y = 8.f;

            unit->Target = target;
            return true;
        },
        [&](std::shared_ptr<TaskManager::Task> task) -> bool {
            auto arguments = std::any_cast<std::pair<std::shared_ptr<Unit>,std::any>>(task->data);
            auto& unit = arguments.first;
            return true;
        },
        [&](std::shared_ptr<TaskManager::Task> task) -> bool { // check if task is finished
            // required
            auto arguments = std::any_cast<std::pair<std::shared_ptr<Unit>,std::any>>(task->data);
            auto& unit = arguments.first;
            if (!unit->Target.has_value()) return true;
            return unit->Distance.mag2() < unit->ActionZone.mag2();
        }
        , 0, olc::Key::M }); // metadata , hotkey

    taskMgr.RegisterTask("Build",
        { [&](std::shared_ptr<TaskManager::Task> task) -> bool {
            // required
            auto arguments = std::any_cast<std::pair<std::shared_ptr<Unit>,std::any>>(task->data);
            auto& unit = arguments.first;
            // customizable parameters
            const auto& params = std::any_cast<std::pair<std::string, olc::vf2d>>(arguments.second);
            const std::string& buildingname = params.first;// Only allowed two >.<
            const olc::vf2d& target = params.second;

            // action code
           // unit->UTask = unit->isBuilding;
            //unit->ULogic = unit->Neutral;
            unit->TrytoBuild(buildingname, target);
            unit->ActionZone = olc::vf2d(32.f, 32.f);//Fix this
            //Build Location

             return true;
         },
         [&](std::shared_ptr<TaskManager::Task> task) -> bool {
             auto arguments = std::any_cast<std::pair<std::shared_ptr<Unit>,std::any>>(task->data);
             auto& unit = arguments.first;
             engine.worldManager->GenerateBuilding(unit->buildName, unit->Target.value() - unit->buildingSize / 2.f);
             return true;

         },
         [&](std::shared_ptr<TaskManager::Task> task) -> bool { // check if task is finished
             // required
             auto arguments = std::any_cast<std::pair<std::shared_ptr<Unit>,std::any>>(task->data);
             auto& unit = arguments.first;
             if (!unit->Target.has_value()) return true;
             return unit->Distance.mag2() < unit->ActionZone.mag2();
         }
         , 0, olc::Key::B }); // metadata , hotkey

    taskMgr.RegisterTask("Repair",
        { [&](std::shared_ptr<TaskManager::Task> task) -> bool {
            // required
            auto arguments = std::any_cast<std::pair<std::shared_ptr<Unit>,std::any>>(task->data);
            auto& unit = arguments.first;
            // customizable parameters

            const auto& params = std::any_cast<std::pair<std::weak_ptr<Building>, olc::vf2d>>(arguments.second);
            
            std::weak_ptr <Building> build = params.first;
            const olc::vf2d& target = params.second;
            unit->Target = build.lock()->Position + unit->buildingSize / 2.f;
            unit->ActionZone = olc::vf2d(32.f, 32.f);
            unit->repairedbuilding = build;
            

             return true;
         },

        [&](std::shared_ptr<TaskManager::Task> task) -> bool {
            auto arguments = std::any_cast<std::pair<std::shared_ptr<Unit>,std::any>>(task->data);
            auto& unit = arguments.first;
            if (unit->repairedbuilding.lock())
                unit->RepairBuilding();
            return true;
            
         },
         [&](std::shared_ptr<TaskManager::Task> task) -> bool { // check if task is finished
             // required
             auto arguments = std::any_cast<std::pair<std::shared_ptr<Unit>,std::any>>(task->data);
             auto& unit = arguments.first;
             if (unit->repairedbuilding.lock() && unit->repairedbuilding.lock()->health > unit->repairedbuilding.lock()->maxHealth)
                 unit->repairedbuilding.reset();

             if (unit->repairedbuilding.expired())
                 return true;
             else
                 return false;

             
         }
         , 0, olc::Key::R }); // metadata , hotkey
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

// Task delegation
void UnitManager::DelegateTask(const std::string& name, const std::any& data) {
	for (auto& _unit : selectedUnits) {
		if (_unit.expired()) continue;
		auto unit = _unit.lock();
        const auto& abilities = unit->unitType.task_abilities;
        if (std::find(abilities.begin(), abilities.end(), name) == abilities.end()) continue;
		if (unit->bFriendly) {
            unit->taskQueue.push(taskMgr.PrepareTask(name, std::pair<std::shared_ptr<Unit>, std::any>{unit, data}));
		}
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
            //unit->MarchingtoTarget(Target);
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
            //unit->MarchingtoTarget(Target);
        }
    }
}

std::shared_ptr<Collidable> UnitManager::FindObject(olc::vf2d Mouse) {
    auto& engine = Game_Engine::Current();
    for (auto& _unit : unitList) {
        auto unit = _unit.lock();
        const float& r = unit->Unit_Collision_Radius;
        const float r2 = 0; // extra collision distance
        if ((unit->Position - Mouse).mag2() < (r * r + r2 * r2)) {
            return unit;
        }
    }
    for (auto& _build : engine.buildingManager->BuildingList) {
        auto build = _build.lock();
        const float& sz = (build->Size.x + build->Size.y) / 2.f;
        const float r2 = 0;
        if ((build->Position - Mouse).mag2() < (sz * sz + r2 * r2)) {
            return build;
        }
    }  
    return {};
}


void UnitManager::ParseObject(std::shared_ptr<Collidable> object, std::weak_ptr<Building>& _build, std::weak_ptr<Unit>& _unit) {
    std::shared_ptr<Unit> unit;
    if (unit = std::dynamic_pointer_cast<Unit>(object)) {
        _unit = std::move(unit);
    }else
        _unit.reset();
    std::shared_ptr<Building> build;
    if (build = std::dynamic_pointer_cast<Building>(object)) {
         _build = std::move(build);
    }else
        _build.reset();
}