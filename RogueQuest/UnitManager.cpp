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
            unit->ActionMode = true;
            unit->ULogic = attackstate ? Unit::Aggressive : Unit::Neutral;
            unit->ActionZone.x = 8.f;
            unit->ActionZone.y = 8.f;
            //unit->Distance = target - unit->Position;
            unit->Graphic_State = Unit::Walking;
            unit->Target = target;
            return true;
        },
        [&](std::shared_ptr<TaskManager::Task> task) -> bool {
            auto arguments = std::any_cast<std::pair<std::shared_ptr<Unit>,std::any>>(task->data);
            auto& unit = arguments.first;
            return true;
        },
        [&](std::shared_ptr<TaskManager::Task> task) -> bool {
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
            unit->ActionMode = false;
            unit->TrytoBuild(buildingname, target);
            unit->ActionZone = olc::vf2d(unit->buildingSize / 2.f) + olc::vf2d(12, 12);
             return true;
         },
         [&](std::shared_ptr<TaskManager::Task> task) -> bool {
             auto arguments = std::any_cast<std::pair<std::shared_ptr<Unit>,std::any>>(task->data);
             auto& unit = arguments.first;
             engine.worldManager->GenerateBuilding(unit->buildName,unit->Owner, unit->Target.value() - unit->buildingSize / 2.f);
             engine.particles->GenerateSmoke(unit->Target.value() - unit->buildingSize / 2.f, unit->buildingSize, true);
             engine.particles->GenerateSmoke(unit->Target.value() - unit->buildingSize / 2.f, unit->buildingSize, true);
             return true;
         },
         [&](std::shared_ptr<TaskManager::Task> task) -> bool { // check if task is finished
             // required
             auto arguments = std::any_cast<std::pair<std::shared_ptr<Unit>,std::any>>(task->data);
             auto& unit = arguments.first;
             if (!unit->Target.has_value()) return true;
             return (std::abs(unit->Distance.x) < unit->ActionZone.x && std::abs(unit->Distance.y) < unit->ActionZone.y);
         }
         , 0, olc::Key::B }); // metadata , hotkey

    taskMgr.RegisterTask("Repair",
        { [&](std::shared_ptr<TaskManager::Task> task) -> bool {
            // required
            auto arguments = std::any_cast<std::pair<std::shared_ptr<Unit>,std::any>>(task->data);
            auto& unit = arguments.first;
            // customizable parameters
            const auto& params = std::any_cast<std::pair<std::weak_ptr<Building>, olc::vf2d>>(arguments.second);            
            const std::weak_ptr<Building> &build = params.first;
            const olc::vf2d& target = params.second;
            unit->ActionMode = false;
            unit->Target = build.lock()->Position + olc::vf2d(build.lock()->Size) / 2.f;
            unit->ActionZone = olc::vf2d(build.lock()->Size /2.f) + olc::vf2d(12, 12);
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
             if (unit->repairedbuilding.lock() && unit->repairedbuilding.lock()->Health > unit->repairedbuilding.lock()->maxHealth)
                 unit->repairedbuilding.reset();

             if (!unit->repairedbuilding.lock())
                 return true;
             else
                 return false;             
         }
         , 0, olc::Key::R }); // metadata , hotkey

    taskMgr.RegisterTask("Hunting",
        { [&](std::shared_ptr<TaskManager::Task> task) -> bool {//Initiate Task
            auto arguments = std::any_cast<std::pair<std::shared_ptr<Unit>,std::any>>(task->data);
            auto& unit = arguments.first;           // customizable parameters
            const auto& params = std::any_cast<std::pair<std::weak_ptr<Building>, std::weak_ptr<Unit>>>(arguments.second);
            const std::weak_ptr<Building> &HBuild = params.first;
            const std::weak_ptr<Unit> &HUnit = params.second;
            unit->ActionMode = true;
            if (HBuild.lock()) {
                unit->Target = HBuild.lock()->Position + olc::vf2d(HBuild.lock()->Size) / 2.f; //center of building
                unit->ActionZone.x = unit->fAttackRange + (float)HBuild.lock()->Size.x/ 2.f;
                unit->ActionZone.y = unit->fAttackRange + (float)HBuild.lock()->Size.y / 2.f;
            }
            if (HUnit.lock()) {
                unit->Target = olc::vf2d(HUnit.lock()->Position.x + HUnit.lock()->Unit_Collision_Radius * 1.414f,
                                         HUnit.lock()->Position.y + HUnit.lock()->Unit_Collision_Radius * 1.414f);
                unit->ActionZone.x = unit->fAttackRange + HUnit.lock()->Unit_Collision_Radius * 1.414f;
                unit->ActionZone.y = unit->fAttackRange + HUnit.lock()->Unit_Collision_Radius * 1.414f;
            }            
             return true;
         },
        [&](std::shared_ptr<TaskManager::Task> task) -> bool {//Preform Task
            auto arguments = std::any_cast<std::pair<std::shared_ptr<Unit>,std::any>>(task->data);
            auto& unit = arguments.first;//ATTACK
            if (unit->fAttackCD > 0 || (!unit->targetBuilding.lock() && !unit->targetUnit.lock())) {//if can't attack or target is dead
                return false;
            }
            else {
                unit->Graphic_State = Unit::Attacking;
                if (unit->curFrame == unit->textureMetadata[unit->Graphic_State].ani_len - 1) {
                    unit->PerformAttack();
                   // unit->Graphic_State = Unit::Walking;
                }
                return true;
            }  
            return true;
         },
         [&](std::shared_ptr<TaskManager::Task> task) -> bool { // check if task is finished when unit dies here this == true
             // required
             auto arguments = std::any_cast<std::pair<std::shared_ptr<Unit>,std::any>>(task->data);
             auto& unit = arguments.first;
             if (!unit->targetBuilding.lock() && !unit->targetUnit.lock())
                 return true;
             if (unit->targetBuilding.lock() && unit->targetBuilding.lock()->Health < 0.f) {
                 unit->targetBuilding.reset();
                 unit->targetUnit.reset();
                 return true;
             }
             if (unit->targetUnit.lock() && unit->targetUnit.lock()->Health < 0.f) {
                 unit->targetUnit.reset();
                 unit->targetBuilding.reset();                 
                 return true;
             }  
             return false;
         }
         , 0, olc::Key::A });

        taskMgr.RegisterTask("Gather",
        { [&](std::shared_ptr<TaskManager::Task> task) -> bool {
            // required
            auto arguments = std::any_cast<std::pair<std::shared_ptr<Unit>,std::any>>(task->data);
            auto& unit = arguments.first;
            if (!FindHomeBase(unit))//If their is no home base you cant Mine
                return false;
            // customizable parameters
            const auto& params = std::any_cast<std::pair<std::weak_ptr<Building>, olc::vf2d>>(arguments.second);
            const std::weak_ptr<Building>& build = params.first;
            if (!build.lock()->isMine)//If this building selected is not a mine you cant mine
                return false;
            const olc::vf2d& target = params.second;
            unit->ActionMode = false;
            unit->MineTarget = build;
            unit->ActionZone =( olc::vf2d(unit->MineTarget.lock()->Size /2.f ) + olc::vf2d(12, 12.f));
            unit->Target = build.lock()->Position + olc::vf2d(build.lock()->Size) / 2.f;
            if(unit->Gold > 0)
                unit->currentTask->performTask();
            return true;
         },

        [&](std::shared_ptr<TaskManager::Task> task) -> bool {
            auto arguments = std::any_cast<std::pair<std::shared_ptr<Unit>,std::any>>(task->data);
            auto& unit = arguments.first;
            if (unit->Gold == 0) {//Go some Gather animations here
                unit->Gather();
            }
            
            if (unit->Gold > 0 && unit->Target !=  unit->HomeBase.lock()->Position + unit->HomeBase.lock()->Size / 2.f) {//Change Take to Delivery
                unit->Target = unit->HomeBase.lock()->Position + olc::vf2d(unit->HomeBase.lock()->Size) / 2.f;
                unit->ActionZone = olc::vf2d(unit->HomeBase.lock()->Size /2.f ) +olc::vf2d(12.f, 12.f);
                return true;//This is Dangerous but will work for now

           }
            if (unit->Gold > 0) {//If I have gold and have triggered preform task
                unit->Deliver();

                unit->ActionZone = olc::vf2d(unit->MineTarget.lock()->Size /2.f) + olc::vf2d(12.f, 12.f);//Restart Gather
                unit->Target = unit->MineTarget.lock()->Position + olc::vf2d(unit->MineTarget.lock()->Size) / 2.f;
            }


            return true;
         },
         [&](std::shared_ptr<TaskManager::Task> task) -> bool { // check if task is finished
             // required
             auto arguments = std::any_cast<std::pair<std::shared_ptr<Unit>,std::any>>(task->data);
             auto& unit = arguments.first;
             if (unit->MineTarget.expired() || unit->MineTarget.lock()->Gold < 0) {
                 return true;
            }
             return false;
         }
         , 0, olc::Key::R }); // metadata , hotkey

}


// internal do not touch
void UnitManager::addNewUnit(std::weak_ptr<Unit> unit) {
    unitList.emplace_back(unit);
}

void UnitManager::Update(float delta) {
    Game_Engine& engine = Game_Engine::Current();
    engine.leaders->FoodMaintenance(unitList);
}

// GC do not touch Magnarune ill explain later... ||
void UnitManager::CollectGarbage() {//edited    

    std::erase_if(unitList, [](const auto& unit) { return unit.expired(); });
  /*  std::vector<std::weak_ptr<Unit>> Newlist;
    for (auto& unit : unitList) {
        if (unit.lock())
            Newlist.push_back(unit);       
    }
    unitList.swap(Newlist);*/
    


    //auto IsDead = [](const auto& unit) {return unit.expired()};
    //unitList.erase(std::remove_if(unitList.begin(), unitList.end(), IsDead), unitList.end());
    //auto it = std::find_if(unitList.begin(), unitList.end(), [](const auto& unit){ return unit.expired(); });
    //if(it++ != unitList.end()){
    //    std::vector<std::weak_ptr<Unit>> copyList;
    //    for(auto& unit : unitList){
    //        if(!unit.expired()) copyList.emplace_back(std::move(unit));
    //    }
    //    unitList = std::move(copyList);
    //}

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
//Step One 
bool UnitManager::ConditionedDelegateTask(int Owner, const std::string& name, const std::any& data) {
    for (auto& _unit : selectedUnits) {
        
        if (_unit.expired()) continue;
        auto unit = _unit.lock();
        const auto& abilities = unit->unitType.task_abilities;
        if (std::find(abilities.begin(), abilities.end(), name) == abilities.end()) continue;
      
    }
    return true;
}

void UnitManager::CheckTaskAbility(std::shared_ptr<Collidable> object, bool A_Click) {
    auto& engine = Game_Engine::Current();
    std::weak_ptr<Unit> testunit;
    std::weak_ptr<Building> testbuilding;
    ParseObject(object,testbuilding,testunit);
    for (auto& _unit : selectedUnits) {
        if (_unit.expired()) continue;
        auto unit = _unit.lock();
        const auto& abilities = unit->unitType.task_abilities;
  
        if (!object) {
            if (std::find(abilities.begin(), abilities.end(), "Move") != abilities.end())
                engine.unitManager->DelegateTask("Move",
                    std::make_pair(engine.tv.ScreenToWorld(engine.GetMousePos()), A_Click));
            continue;
        }
        if (testbuilding.lock()) {
            if (testbuilding.lock()->Owner == unit->Owner) {//If it was an ally
                if (!A_Click) {
                    if (std::find(abilities.begin(), abilities.end(), "Gather") != abilities.end() && testbuilding.lock()->isMine)
                        engine.unitManager->DelegateTask("Gather",
                            std::make_pair(testbuilding, engine.tv.ScreenToWorld(engine.GetMousePos())));
                    else
                        if (std::find(abilities.begin(), abilities.end(), "Repair") != abilities.end())
                            engine.unitManager->DelegateTask("Repair",
                                std::make_pair(testbuilding, engine.tv.ScreenToWorld(engine.GetMousePos())));
                        else
                            if (std::find(abilities.begin(), abilities.end(), "Move") != abilities.end())
                                engine.unitManager->DelegateTask("Move",
                                    std::make_pair(engine.tv.ScreenToWorld(engine.GetMousePos()), A_Click));

                }

            } else
                if (std::find(abilities.begin(), abilities.end(), "Move") != abilities.end())
                    engine.unitManager->DelegateTask("Move",
                        std::make_pair(engine.tv.ScreenToWorld(engine.GetMousePos()), true));
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
            return n + (!unit.expired() && unit.lock()->bSelected);
        });
}

std::shared_ptr<Unit> UnitManager::GetUnit(const std::string& name, size_t index) {
    size_t n = 0;
    for(auto& _unit : unitList){
        if (_unit.expired())
            continue;
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
        if (_unit.expired())
            continue;
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
        if (_unit.expired())
            continue;
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
            unit->ULogic = attackstate ? Unit::Aggressive : Unit::Neutral;
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
            unit->ULogic = Unit::Neutral;
            //unit->MarchingtoTarget(Target);
        }
    }
}

std::shared_ptr<Collidable> UnitManager::FindObject(olc::vf2d Mouse) {//User
    auto& engine = Game_Engine::Current();
    for (auto& _unit : unitList) {
        if (_unit.expired())
            continue;
        auto unit = _unit.lock();
        const float& r = unit->Unit_Collision_Radius;
        const float r2 = 0; // extra collision distance
        if ((unit->Position - Mouse).mag2() < (r * r + r2 * r2)) {
            return unit;
        }
    }

    std::shared_ptr<Collidable> testBuild;
    engine.buildingManager->IterateAllBuildings([&](std::shared_ptr<Building> build) -> bool {
        const float& sz = (build->Size.x + build->Size.y) / 2.f;
        const float r2 = 0;
        if ((build->Position - Mouse).mag2() < (sz * sz + r2 * r2)) {
           testBuild = build;
            return false;
        }
        return true;
    });
   
    return testBuild;//I hole this works
}

void UnitManager::ParseObject(std::shared_ptr<Collidable> object, std::weak_ptr<Building>& _build, std::weak_ptr<Unit>& _unit) {
    _build.reset();
    _unit.reset();
    if (!object)
        return;
    if (auto unit = std::dynamic_pointer_cast<Unit>(object)) {
        _unit = unit;
        return;
    }
    if (auto build = std::dynamic_pointer_cast<Building>(object)) {
        _build = build;
        return;
    }
}


std::vector<std::shared_ptr<Collidable>> UnitManager::FindObjects(olc::vf2d pos, float SearchRadius) {
    testobjects.clear();
    auto& engine = Game_Engine::Current();
    for (auto& _unit : unitList) {
        if (_unit.expired())
            continue;
        auto unit = _unit.lock();        
        if ((unit->Position - pos).mag2() < (SearchRadius*SearchRadius)) {
             testobjects.push_back(unit);
        }
    }

    engine.buildingManager->IterateAllBuildings([&](std::shared_ptr<Building> build) -> bool {
        if ((build->Position - pos).mag2() < (SearchRadius * SearchRadius + (float)build->Size.x)) {
            testobjects.push_back(build);
        }       
        return true;
    });
    
    return testobjects;
}

void UnitManager::ParseObjects(std::vector<std::shared_ptr<Collidable>> objects, std::queue<std::weak_ptr<Building>>& _builds, std::queue<std::weak_ptr<Unit>>& _units) {
    std::queue<std::weak_ptr<Building>> ClearBuildings;
    std::queue<std::weak_ptr<Unit>> ClearUnits;
    _builds.swap(ClearBuildings);
    _units.swap(ClearUnits);
    for (int i = 0; i < objects.size(); i++) {
        std::shared_ptr<Unit> unit;
        if (unit = std::dynamic_pointer_cast<Unit>(objects[i])) {
            _units.push(std::move(unit));
        }
        std::shared_ptr<Building> build;
        if (build = std::dynamic_pointer_cast<Building>(objects[i])) {
            _builds.push(std::move(build));
        }
    }
}

std::shared_ptr<Collidable> UnitManager::SearchClosestEnemy(int owner,olc::vf2d pos, float SearchRadius) {
    auto& engine = Game_Engine::Current();
    float Smallest = SearchRadius * SearchRadius;  
    testobjects.clear();

    IterateAllUnits([&](std::shared_ptr<Unit> unit) -> bool {
        if ((unit->Position - pos).mag2() < (SearchRadius * SearchRadius) && unit->Health > 0.f ) {
            if ((unit->Position - pos).mag2() < 0.2f) return true; // continue
            if (owner == 0) {
                if(unit->Owner != 0)
                    testobjects.push_back(unit);
                return true; // continue
            } else {
                for (int i = 0; i < unit->FriendList.size(); i++) {
                    if (unit->FriendList[i] == owner || unit->Owner == 0)
                        if (abs(unit->FriendList[i]) + unit->FriendList[i] == 0)
                            testobjects.push_back(unit);
                }
            }
        }
        return true; // continue
    });

    engine.buildingManager->IterateAllBuildings([&](std::shared_ptr<Building> build) -> bool {
        if ((build->Position - pos).mag2() < (SearchRadius * SearchRadius + (float)build->Size.x) && build->Health > 0.f) {
            if ((build->Position - pos).mag2() < 0.2f) return true; // continue
            if (owner == 0) {
                if (build->Owner != 0)
                    testobjects.push_back(build);
                return true; // continue
            } else {
                for (int i = 0; i < build->FriendList.size(); i++) {
                    if (build->FriendList[i] == owner || build->Owner == 0)
                        if (abs(build->FriendList[i]) + build->FriendList[i] == 0)
                            testobjects.push_back(build);
                }
            }
        }
        return true;
    });

    if (testobjects.size()) {
        std::shared_ptr<Collidable> closest;
        closest = testobjects[0];
        for (auto& o : testobjects) {
            
            float test = (o->Position - pos).mag2();
            if (test < Smallest) {
                closest = o;
                Smallest = test;
            }
        }
        return closest;
    }
    else
        return {};    
}

std::shared_ptr<Unit> UnitManager::This_shared_pointer(olc::vf2d pos) {
    std::shared_ptr<Unit> thisUnit;
    for (auto& _unit : unitList) {  
        if (_unit.expired())
            continue;
        auto unit = _unit.lock();

        if (unit->Position == pos) {            
            thisUnit = unit;
        }
    }
    return thisUnit;
}

bool UnitManager::FindHomeBase(std::shared_ptr<Unit>& unit) {
    auto& engine = Game_Engine::Current();

    engine.buildingManager->IterateAllBuildings([&](std::shared_ptr<Building> build) -> bool {
        if  (build->Owner == unit->Owner) {
            if (build->name == "Castle") {            
                unit->HomeBase = build;
                return false;
            }
        }
        return true;
    });
    return !unit->HomeBase.expired();
}