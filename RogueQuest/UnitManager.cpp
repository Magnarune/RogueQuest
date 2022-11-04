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
            unit->ULogic = attackstate ? Unit::Aggressive : Unit::Passive;
            unit->ActionZone.x = 3.f;
            unit->ActionZone.y = 3.f;
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
            const auto& params = std::any_cast<std::pair<std::shared_ptr<Building>, olc::vf2d>>(arguments.second);
            const std::shared_ptr<Building>& potbuilding = params.first;// Only allowed two >.<
            const olc::vf2d& target = params.second;
            unit->ActionMode = false;
            unit->TrytoBuild(potbuilding->name, target);
            unit->ActionZone = olc::vf2d(unit->buildingSize / 2.f) + olc::vf2d(12, 12);
             return true;
         },
         [&](std::shared_ptr<TaskManager::Task> task) -> bool {
             auto arguments = std::any_cast<std::pair<std::shared_ptr<Unit>,std::any>>(task->data);
             auto& unit = arguments.first;
             //
             const auto& buildingdata = engine.assetManager->GetBuildingData(unit->buildName);
             if (engine.leaders->LeaderList[unit->Owner]->Gold < buildingdata.Cost) {
                 return false;
             }
                 
             engine.leaders->LeaderList[unit->Owner]->Gold -= buildingdata.Cost;
             engine.worldManager->GenerateBuilding(unit->buildName,unit->Owner, unit->Target.value());
             engine.particles->GenerateSmoke(unit->Target.value(), unit->buildingSize, true);
             engine.particles->GenerateSmoke(unit->Target.value(), unit->buildingSize, true);
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
            unit->Target = build.lock()->Position;
            unit->ActionZone = olc::vf2d(build.lock()->Size) /2.f + olc::vf2d(12, 12);
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
            unit->ActionZone =( olc::vf2d(unit->MineTarget.lock()->Size )/2.f  + olc::vf2d(12, 12.f));
            unit->Target = build.lock()->Position ;
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
            
            if (unit->HomeBase.lock()) {
                if (unit->Gold > 0 && unit->Target != unit->HomeBase.lock()->Position) {//Change Take to Delivery
                    unit->Target = unit->HomeBase.lock()->Position;
                    unit->ActionZone = olc::vf2d(unit->HomeBase.lock()->Size) / 2.f + olc::vf2d(12.f, 12.f);
                    return true;//This is Dangerous but will work for now

                }
            } else
                return true;

            if (unit->Gold > 0) {//If I have gold and have triggered preform task
                unit->Deliver();

                unit->ActionZone = olc::vf2d(unit->MineTarget.lock()->Size) /2.f + olc::vf2d(12.f, 12.f);//Restart Gather
                unit->Target = unit->MineTarget.lock()->Position  + olc::vf2d(0.f, 5.f);
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
        taskMgr.RegisterTask("Harvest",
            { [&](std::shared_ptr<TaskManager::Task> task) -> bool {
                // required
                auto arguments = std::any_cast<std::pair<std::shared_ptr<Unit>,std::any>>(task->data);
                auto& unit = arguments.first;
                if (!FindHomeBase(unit))//If their is no home base you cant Mine
                    return false;
                // customizable parameters
                const auto& params = std::any_cast<std::pair<std::weak_ptr<CollisionMapObject>, olc::vf2d>>(arguments.second);
                const std::weak_ptr<CollisionMapObject>& Tree = params.first;
                if (Tree.expired())
                    return false;
                if (Tree.lock()->Lumber < 0.f)//If this building selected is not a mine you cant mine
                    return false;
                if(unit->MineTarget.lock())
                    unit->MineTarget.reset();
                const olc::vf2d& target = params.second;

                unit->ActionMode = false;
                unit->TreeObject = Tree;
                unit->ActionZone = (olc::vf2d(unit->TreeObject.lock()->mask.rect) / 2.f + olc::vf2d(12, 16.f));
                unit->Target = Tree.lock()->Position+olc::vf2d(0,16);
                if (unit->Lumber > 0)
                    unit->currentTask->performTask();
                return true;
             },

            [&](std::shared_ptr<TaskManager::Task> task) -> bool {
                auto arguments = std::any_cast<std::pair<std::shared_ptr<Unit>,std::any>>(task->data);
                auto& unit = arguments.first;
                if (unit->Lumber < 25) {//Gather wood
                    unit->Gather();
                }
                if (unit->HomeBase.lock()) {
                    if (unit->Lumber > 0 && unit->Target != unit->HomeBase.lock()->Position) {//Change Take to Delivery
                        unit->Target = unit->HomeBase.lock()->Position;
                        unit->ActionZone = olc::vf2d(unit->HomeBase.lock()->Size) / 2.f + olc::vf2d(12.f, 12.f);
                        return true;//This is Dangerous but will work for now

                    }
                } else
                    return true;

                if (unit->Lumber > 0) {//If I have gold and have triggered preform task
                    unit->Deliver();

                    unit->ActionZone = olc::vf2d(unit->TreeObject.lock()->mask.rect) / 2.f + olc::vf2d(12.f, 12.f);//Restart Gather
                    unit->Target = unit->TreeObject.lock()->Position + olc::vf2d(0, 16) + olc::vf2d(0.f, 5.f);
                }


                return true;
             },
             [&](std::shared_ptr<TaskManager::Task> task) -> bool { // check if task is finished
                 // required
                 auto arguments = std::any_cast<std::pair<std::shared_ptr<Unit>,std::any>>(task->data);
                 auto& unit = arguments.first;
                 if (unit->TreeObject.expired() || unit->TreeObject.lock()->Lumber < 0) {
                     return true;
                }
                 return false;
             }
             , 0, olc::Key::H }); // metadata , hotkey

        taskMgr.RegisterTask("AttackTarget",
            { [&](std::shared_ptr<TaskManager::Task> task) -> bool {//Initiate Task
                auto arguments = std::any_cast<std::pair<std::shared_ptr<Unit>,std::any>>(task->data);
                auto& unit = arguments.first;           // customizable parameters
                const auto& params = std::any_cast<std::pair<std::weak_ptr<Building>, std::weak_ptr<Unit>>>(arguments.second);
                const std::weak_ptr<Building>& HBuild = params.first;
                const std::weak_ptr<Unit>& HUnit = params.second;
                unit->attackTarget = true;
                unit->ActionMode = true;
                if (HBuild.lock()) {
                    unit->targetBuilding = HBuild;
                    unit->Target = HBuild.lock()->Position; //center of building
                    unit->ActionZone.x = unit->fAttackRange + (float)HBuild.lock()->Size.x / 2.f;
                    unit->ActionZone.y = unit->fAttackRange + (float)HBuild.lock()->Size.y / 2.f;
                }
                if (HUnit.lock()) {
                    unit->targetUnit = HUnit;
                    unit->Target = olc::vf2d(HUnit.lock()->Position.x,HUnit.lock()->Position.y);
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
                } else {
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
                  if (!unit->targetBuilding.lock() && !unit->targetUnit.lock()) {
                      unit->attackTarget = true;
                      return true;
                  }
                  if (unit->targetBuilding.lock() && unit->targetBuilding.lock()->Health < 0.f) {
                      unit->attackTarget = true;
                      unit->targetBuilding.reset();
                      unit->targetUnit.reset();
                      return true;
                  }
                  if (unit->targetUnit.lock() && unit->targetUnit.lock()->Health < 0.f) {
                      unit->attackTarget = true;
                      unit->targetUnit.reset();
                      unit->targetBuilding.reset();
                      return true;
                  }
                  return false;
              }
              , 0, olc::Key::P });
}
// internal do not touch
void UnitManager::addNewUnit(std::weak_ptr<Unit> unit) {
    unitList.emplace_back(unit);
}

void UnitManager::Update(float delta) {
    Game_Engine& engine = Game_Engine::Current();
    engine.leaders->FoodMaintenance(unitList); 
  
}

void UnitManager::CollectGarbage() {//edited    

    std::erase_if(unitList, [](const auto& unit) { return unit.expired(); });

}

// Task delegation
void UnitManager::DelegateTask(const std::string& name, const std::any& data) {
	for (auto& _unit : selectedUnits) {
		if (_unit.expired()) continue;
		auto unit = _unit.lock();
        const auto& abilities = unit->unitType.task_abilities;
        if (name == "AttackTarget") {
            if (std::find(abilities.begin(), abilities.end(), "Attack") == abilities.end()) continue;
        }else
            if (std::find(abilities.begin(), abilities.end(), name) == abilities.end()) continue;
		if (unit->bFriendly) {
            unit->taskQueue.push(taskMgr.PrepareTask(name, std::pair<std::shared_ptr<Unit>, std::any>{unit, data}));
		}
	}
}
//Step One 
void UnitManager::ConditionedDelegateTask(std::shared_ptr<Unit> unit, const std::string& name, const std::any& data) {
    unit->taskQueue.push(taskMgr.PrepareTask(name, std::pair<std::shared_ptr<Unit>, std::any>{unit, data}));       
}

void UnitManager::CheckTaskAbility(std::shared_ptr<Collidable> object, bool A_Click) {
    numberofselectedunits = 0;
    Columnoffset = 0;
    auto& engine = Game_Engine::Current();
    std::weak_ptr<Unit> testunit;
    std::weak_ptr<Building> testbuilding;
    std::weak_ptr<CollisionMapObject> testTree;
    ParseObject(object,testbuilding,testunit, testTree);
    for (auto& _unit : selectedUnits) {
        if (_unit.expired()) continue;
        numberofselectedunits += 1;
        auto unit = _unit.lock();
        const auto& abilities = unit->unitType.task_abilities;
        if (!object) {
            if (std::find(abilities.begin(), abilities.end(), "Move") != abilities.end())
                engine.unitManager->ConditionedDelegateTask(unit, "Move",
                    std::make_pair(olc::vf2d(engine.tv.ScreenToWorld(olc::vf2d(engine.GetMousePos()))) + ArrangeSelectedUnits((int)selectedUnits.size(), numberofselectedunits), A_Click));
            continue;
        }
        if (testbuilding.lock()) {
            //If it was an ally
            if (!A_Click) {
                if (testbuilding.lock()->Owner == unit->Owner) {
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
                } else {
                    engine.unitManager->DelegateTask("AttackTarget",
                        std::make_pair(testbuilding, testunit));
                }
            } else
            {
                engine.unitManager->DelegateTask("AttackTarget",
                    std::make_pair(testbuilding, testunit));
            }

        }
        if (testunit.lock()) {

            if (!A_Click) {
                if (testunit.lock()->Owner == unit->Owner) {
                    if (std::find(abilities.begin(), abilities.end(), "Move") != abilities.end() && !A_Click)
                        engine.unitManager->DelegateTask("Move",
                            std::make_pair(engine.tv.ScreenToWorld(engine.GetMousePos()), true));
                } else {
                    engine.unitManager->DelegateTask("AttackTarget",
                        std::make_pair(testbuilding, testunit));
                }

            } else
            {
                engine.unitManager->DelegateTask("AttackTarget",
                    std::make_pair(testbuilding, testunit));
            }
        }
        if (testTree.lock()) {
            if (std::find(abilities.begin(), abilities.end(), "Harvest") != abilities.end())
                engine.unitManager->DelegateTask("Harvest",
                    std::make_pair(testTree, engine.tv.ScreenToWorld(engine.GetMousePos())));

            if (std::find(abilities.begin(), abilities.end(), "Move") != abilities.end())
                engine.unitManager->DelegateTask("Move",
                    std::make_pair(engine.tv.ScreenToWorld(engine.GetMousePos()), A_Click));

        }
        else
            if (std::find(abilities.begin(), abilities.end(), "Move") != abilities.end())
            engine.unitManager->DelegateTask("Move",
                std::make_pair(engine.tv.ScreenToWorld(engine.GetMousePos()), A_Click));
    }
}

olc::vf2d UnitManager::ArrangeSelectedUnits(int Size, int iterator) {
    iterator -= 1;
    float unitoffset = 20.f;
    int ColumnSize = 4;
    int ColumnSizeOdd = 5;
    int Sizeadder=-1;
    if (Size == 1) {
        return { 0.f,0.f };
    }
    for (int i = 0; i < Size; i++) {
        if ((i + 1) % 3 == 0)
            Sizeadder +=1;
    }
    if (Size > 1) {     

        if (Size % 2 == 0) {//Even
            if ((iterator) % ColumnSize == 0) {
                if ((iterator) != 0)
                    Columnoffset += 1;
            }
            if (Size == 2) {
                if (iterator == 0) {//
                    return { 0.f,   -unitoffset / 2.f };
                } else
                {
                    return { 0.f,  unitoffset / 2.f };//size/2
                }
            }
            if ((iterator - Columnoffset * ColumnSize) < ColumnSize / 2) {//
                return { -unitoffset * Columnoffset,   -unitoffset / 2.f + -unitoffset * ((iterator - Columnoffset*ColumnSize )) };
            }
            else
            {
                return { -unitoffset * Columnoffset ,  unitoffset / 2.f + unitoffset * ((iterator  - Columnoffset * ColumnSize - ColumnSize/2))  };//size/2
            }
        }
        else{
            if ((iterator) % ColumnSizeOdd == 0) {
                if ((iterator) != 0)
                    Columnoffset += 1;
            }
            if(Size == 3)
                if (iterator - Columnoffset * ColumnSizeOdd < ((ColumnSizeOdd + 1) / 2) -1) {
                return { -unitoffset * Columnoffset,   -unitoffset * ((iterator - Columnoffset * ColumnSizeOdd)) };
                } else {
                    return { -unitoffset * Columnoffset ,  unitoffset * ((iterator + 2 - Columnoffset * ColumnSizeOdd - (ColumnSizeOdd + 1) / 2)) };//size/2
                }

            if (iterator - Columnoffset * ColumnSizeOdd < ((ColumnSizeOdd + 1) / 2)+ Sizeadder) {
                return { -unitoffset * Columnoffset,   -unitoffset * ((iterator - Columnoffset * ColumnSizeOdd)) };
            } else {
                return { -unitoffset * Columnoffset ,  unitoffset * ((iterator + 1  - Columnoffset * ColumnSizeOdd - (ColumnSizeOdd +1) / 2)) };//size/2
            }
        }
    }

    return {0.f,0.f};
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
            unit->Position.y < std::max(Final.y, Initial.y)) 
        {
           unit->bSelected = true;
           selectedUnits.push_back(_unit);
        }
    }
}

bool UnitManager::CheckBuildObstruction(std::shared_ptr<Building> potBuilding) {
    auto& engine = Game_Engine::Current();
    bool GoodtoPlaceBuilding = true;
    engine.buildingManager->IterateAllBuildings([&](std::shared_ptr<Building> build) -> bool {
   /*     if (build->Position.x > potBuilding->Position.x + (float)potBuilding->Size.x &&
            build->Position.y > potBuilding->Position.y + (float)potBuilding->Size.y &&
            build->Position.x + (float)build->Size.x > potBuilding->Position.x &&
            build->Position.y + (float)build->Size.y > potBuilding->Position.y) {
            GoodtoPlaceBuilding = false;
            return false;
        }*/
        if ((potBuilding->Position.x - potBuilding->Size.x < build->Position.x - potBuilding->Size.x + (float)build->Size.x && potBuilding->Position.x - potBuilding->Size.x + (float)potBuilding->Size.x>  build->Position.x - potBuilding->Size.x &&
            potBuilding->Position.y - potBuilding->Size.y< build->Position.y - potBuilding->Size.y + (float)build->Size.y && potBuilding->Position.y - potBuilding->Size.y + (float)potBuilding->Size.y>  build->Position.y - potBuilding->Size.y)) {
            GoodtoPlaceBuilding = false;
            return false;
        }
            
        return true;
        });
    engine.DrawStringDecal({30.f,30.f}, std::to_string(GoodtoPlaceBuilding));
    return GoodtoPlaceBuilding;
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
        const auto& meta = build->textureMetadata.at(build->Graphic_State);
        if (Mouse.x > build->Position.x - (float)meta.draw_origin.x  && Mouse.y > build->Position.y - (float)meta.draw_origin.y  &&
            Mouse.x < build->Position.x + (float)meta.draw_origin.x  &&
            Mouse.y < build->Position.y + (float)meta.draw_origin.y ) {
           testBuild = build;
            return false;
        }
        return true;
    });
    if (testBuild)
        return testBuild;
    std::shared_ptr<CollisionMapObject> testTree;
    engine.worldManager->IterateObjectsQT(olc::utils::geom2d::rect<float>(engine.tv.GetWorldTL(), engine.tv.GetWorldVisibleArea()), [&](std::shared_ptr<WorldObject> _Tree) -> bool {
        auto tree = std::dynamic_pointer_cast<Collidable>(_Tree);
        const auto& meta = tree->mask;
        if (auto ISTREE = std::dynamic_pointer_cast<CollisionMapObject>(tree)) {
            if (Mouse.x > tree->Position.x -(float)meta.origin.x && Mouse.y > tree->Position.y - (float)meta.origin.y &&
                Mouse.x < tree->Position.x + (float)meta.rect.x &&
                Mouse.y < tree->Position.y + (float)meta.rect.y) {
                testTree = ISTREE;
                return false;
            }
        }
        return true;
        });
 
    return testTree;
}

void UnitManager::ParseObject(std::shared_ptr<Collidable> object, std::weak_ptr<Building>& _build, std::weak_ptr<Unit>& _unit , std::weak_ptr<CollisionMapObject>& _tree) {
    _build.reset();
    _unit.reset();
    _tree.reset();
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
    if (auto tree = std::dynamic_pointer_cast<CollisionMapObject>(object)) {
        _tree = tree;
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
        if ((unit->Position - pos).mag2() < (SearchRadius * SearchRadius) && unit->Health > 0.f) {
            if ((unit->Position - pos).mag2() < 0.2f) return true; // continue

            if (owner == 0) {
                if (unit->Owner != 0)
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
        return true;
        });

    engine.buildingManager->IterateAllBuildings([&](std::shared_ptr<Building> build) -> bool {
        if ((build->Position - pos).mag2() < (SearchRadius * SearchRadius + (float)build->Size.x) && build->Health > 0.f) {
            if ((build->Position - pos).mag2() < 0.2f) return true; // continue
            if (owner == 0) {
                if (build->Owner != 0)
                    testobjects.push_back(build);
                return true; // continue
            } else {
                //Bugged here fix later
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