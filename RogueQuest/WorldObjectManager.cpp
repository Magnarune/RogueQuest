#include "WorldObjectManager.h"
#include "Engine.h"
#include <ranges>

WorldManager::WorldManager(): QuadList(objectList) {
    auto& engine = Game_Engine::Current();
    
    objectList.reserve(1024 * 512);
    garbageList.reserve(100);
    mapList.reserve(10);
}

WorldManager::~WorldManager() {
   // sigSorter.clear();
    mapList.clear();
	objectList.clear();
	garbageList.clear();
}

void WorldManager::refreshObjectView() {
    auto& engine = Game_Engine::Current();
    objectListView.clear();
    objectListView.assign(objectList.begin(), objectList.end());

   /* sigSorter.clear();
    screen = { engine.tv.GetWorldTL(),engine.tv.GetWorldBR() - engine.tv.GetWorldTL() };
    for (auto& founditems : quadtreeList.search(screen)) {
        sigSorter.push_back(founditems->item);
    }
    
    std::ranges::sort(sigSorter, [](auto& a, auto& b) -> bool {
     return a.get()->drawDepth < b.get()->drawDepth;
    });  */
}

void WorldManager::addObjectToList(std::shared_ptr<WorldObject> obj, olc::utils::geom2d::rect<float> size) {
    newObjectList.emplace_back(obj); // add to new object list
}

   // newobjects.push_back({ obj, size });//hold items until new frame   
void WorldManager::Update(float delta) {//Update last frames
   
    auto& engine = Game_Engine::Current();

    // sort the object list based on draw depth
    std::ranges::sort(objectListView, [](auto& a, auto& b) -> bool {
        return a.get()->updatePriority < b.get()->updatePriority;
        });

    for (auto& object : objectListView) {
        if (object.get() == nullptr) continue;

        object.get()->Update(delta);
    }
    for (auto& object : objectListView) {
        	if (object.get() == nullptr) continue;
            object.get()->AfterUpdate(delta);
           
        }
    //this is all Update
  /*  for (auto quad = quadtreeList.begin(); quad != quadtreeList.end(); ++quad) {
        if (quad->item.get() == nullptr) continue;
        quad->item.get()->Update(delta);
      
    } 

    for (auto quad = quadtreeList.begin(); quad != quadtreeList.end(); ++quad) {
        if (quad->item.get() == nullptr) continue;
        quad->item.get()->AfterUpdate(delta);
        quad->item.get()->PosSize.pos = quad->item.get()->Position;        
        if (quad->item.get()->IhaveMoved) {
            quad->item.get()->IhaveMoved = false;
            quadtreeList.relocate(quad, quad->item.get()->PosSize);            
        }
    } */

    if (worldclock.getSeconds() > 1.0) {
        engine.leaders->FindHomeBase();
        worldclock.restart();
    }
}

void WorldManager::Draw() {
    Game_Engine& engine = Game_Engine::Current();
    std::ranges::sort(objectListView, [](auto& a, auto& b) -> bool {
        return a.get()->drawDepth < b.get()->drawDepth;
    });
    currentMap->DrawMap(&engine.tv);
    for (auto& object : objectListView) {
        if (object.get() == nullptr) continue;
        if (!Checkonscreen(object)) continue;
    }
   
    //refreshObjectView();
    //for (auto& sigs : sigSorter) {
    //    sigs->Draw(&engine.tv);
    //}
    //

}

bool WorldManager::Checkonscreen(std::shared_ptr<WorldObject> obj) {
    auto& engine = Game_Engine::Current();
    if (obj->Position.x > 32.f * (float)currentMap->topLeftTile.x &&
        obj->Position.y > 32.f * (float)currentMap->topLeftTile.y &&
        obj->Position.x < 32.f * (float)currentMap->bottomRightTile.x &&
        obj->Position.y < 32.f * (float)currentMap->bottomRightTile.y)
        return true;
    return false;
}

void WorldManager::DestroyObject(WorldObject* self) {
    auto me = std::find_if(objectList.begin(), objectList.end(), [&](const auto& obj) { return obj.get() == self; });
    assert(me != objectList.end()); // destroying an object should always find it in the list!
    garbageList.emplace_back(std::move(*me)); // move to garbage for final living space before death
}

void WorldManager::CollectGarbage() {
    auto& engine = Game_Engine::Current();

    size_t len = garbageList.size();
    if (len) { // if there is garbage, we need to clean the missing links
        size_t pos = 1;

        auto itr = objectList.begin();
        if (std::find_if(objectList.begin(), objectList.end(), [](const auto& obj) { return obj.get() != nullptr; }) == objectList.end()) {
            objectList.clear();
        } else {
            do {
                if (!!itr->get()) continue; // keep valid objectList
                do { // keep swapping until there's no more invalid object here
                    if (objectList.begin() == objectList.end() - pos || itr == objectList.end() - pos) break;
                    std::swap(*itr, *(objectList.end() - pos)); // move garbage to end of list
                    ++pos; // increase found garbage
                } while (!itr->get());

            } while (itr + 1 != objectList.end() && pos <= len && ++itr != objectList.end() - len); // continue looking for missing link garbage

            objectList.resize(objectList.size() - len); // slice off the head
            // for(int i=0; i < len; ++i) objectList.pop_back(); // slice off the garbage - resize instead?
        }
        garbageList.clear();

        engine.unitManager->CollectGarbage(); // cleanup the unit manager garbage
        engine.buildingManager->CollectGarbage(); //cleanup the building manager garbage

        refreshObjectView(); // remember to refresh the object view when modifying the list
   }
}

void WorldManager::PopulateNewObjects() {
    for (auto& obj : newObjectList) objectList.emplace_back(std::move(obj));
    newObjectList.clear();
    refreshObjectView();
}
    //for(auto& obj : newobjects)//add the vector of new items to quad tree
    //    quadtreeList.insert(obj.first, obj.second);
    //newobjects.clear();

   // refreshObjectView();
std::shared_ptr<WorldObject> WorldManager::FindObject(size_t index) {
    return objectList.size() > index ? objectList.at(index) : nullptr;
}

bool WorldManager::IterateObjects(std::function<bool(std::shared_ptr<WorldObject>)> cb) {

    for(auto& object : objectList) {
        if(object == nullptr) continue;
        if(!cb(object)) break;
    }
    return true; // iterate completed successfully
}

//bool WorldManager::IterateObjectQT(std::function<bool(std::shared_ptr<WorldObject>)> cb, olc::utils::geom2d::rect<float> searchsize) {
//    auto object = quadtreeList.search(searchsize);
//    for (auto& obj :object) {    
//        if (obj->item == nullptr) continue;
//        if (!cb(obj->item)) break;
//    }
//    return true; // iterate completed successfully
//}

/* Factory Function */

std::shared_ptr<Unit> WorldManager::GenerateUnit(const std::string& name, int owner, olc::vf2d pos) {
    Game_Engine& engine = Game_Engine::Current();

    if(!engine.assetManager->UnitExists(name)) return nullptr;
    const auto& data = engine.assetManager->GetUnitData(name);
    // Make Unit
    std::shared_ptr<Unit> unit;
    unit.reset(new Unit(data)); // pass in primary unit type
    if (pos == olc::vf2d(0.f, 0.f))
        pos = { engine.worldManager->curMap().layerSize.x * 32 / 2.f, engine.worldManager->curMap().layerSize.y * 32 / 2.f };

    unit->predPosition = unit->Position = pos;

    // Update Internal Values Of New Unit
    unit->sUnitName = data.lua_data["Name"]; //This is in top of .lua
    //Load Sprite Order
    for (int i = 0; i < 4; i++)
        unit->Direction.push_back(data.lua_data["SpriteOrder"][i+1]);
    unit->Target = std::nullopt;
       
    // Load Parameters
    unit->Unit_Collision_Radius = data.lua_data["Parameters"]["CollisionRadius"];
    unit->bIsRanged = data.lua_data["Parameters"]["Ranged"];
   
    unit->food = data.lua_data["Parameters"]["Food"];
    // Load Stats
    unit->Health = data.lua_data["Stats"]["Health"]; unit->fMaxHealth = data.lua_data["Stats"]["MaxHealth"];
    unit->fMana = data.lua_data["Stats"]["Mana"];     unit->fMaxMana = data.lua_data["Stats"]["MaxMana"];
    unit->fAmmo = data.lua_data["Stats"]["Ammo"];     unit->fMaxAmmo = data.lua_data["Stats"]["MaxAmmo"];
    unit->fMoveSpeed = data.lua_data["Stats"]["MoveSpeed"];
    unit->fAttackRange = data.lua_data["Stats"]["AttackRange"];
    unit->fAttackDamage = data.lua_data["Stats"]["AttackDamage"];
    unit->fAttackSpeed = data.lua_data["Stats"]["AttackSpeed"];
    unit->fSpellCooldown = data.lua_data["Stats"]["SpellCooldown"];
    unit->fKnockBackResist = data.lua_data["Stats"]["KnockBackResist"];   
    unit->Owner = owner;
    unit->FriendList = InitializeObject(owner);
    //Bonuses
    unit->fAttackDamage += engine.leaders->LeaderList[owner]->Bonus.meleedamage;
    unit->fMoveSpeed += engine.leaders->LeaderList[owner]->Bonus.movespeed;
    unit->fMaxHealth += engine.leaders->LeaderList[owner]->Bonus.health;
    unit->Health = unit->fMaxHealth;
    // make sure to update this when adding new GFXStates - enums don't magically connect to a string
    static std::map<std::string, Unit::GFXState> States = {
        {"Walking", Unit::Walking},
        {"Attacking", Unit::Attacking},
        {"Dead", Unit::Dead},
        {"Build", Unit::Build}
    };

    // create decals for each texture state
    for(auto& [ name, meta ] : data.texture_metadata){
        const Unit::GFXState& state = States[name]; // local state ref
        // load a decal texture and add to decal map
        std::unique_ptr<olc::Decal> decal;
        decal.reset(new olc::Decal(TextureCache::GetCache().GetTexture(meta.tex_id)));
        unit->decals.insert_or_assign(state, std::move(decal));
        // copy texture metadata    
        unit->textureMetadata.insert_or_assign(state, meta);
    }
    unit->bFriendly = true;  
    
    unit->SetMask(Collidable::Mask(unit->Unit_Collision_Radius));
    unit->cType = Collidable::isUnit;
    unit->updatePriority = 0.1f; // highest priority

    engine.unitManager->addNewUnit(unit);
    unit->PosSize = { unit->Position,olc::vf2d(unit->Unit_Collision_Radius,unit->Unit_Collision_Radius)};
    addObjectToList(unit,unit->PosSize);
    return unit;
}

std::shared_ptr<Building> WorldManager::GenerateBuilding(const std::string& name, int owner, olc::vf2d pos) {
    Game_Engine& engine = Game_Engine::Current();
    auto to_vi2d = [](sol::table obj) -> olc::vi2d {
        int32_t x = obj[1],
            y = obj[2];
        return { x, y };
    };
    if (!engine.assetManager->BuildingExists(name)) return nullptr;
    const auto& data = engine.assetManager->GetBuildingData(name);
    // make building
    std::shared_ptr<Building> build;
    build.reset(new Building());

    if (pos == olc::vf2d(0.f, 0.f))
        pos = { 10.f,10.f };
    build->predPosition = build->Position = pos;
    build->name = name;
    build->offset = to_vi2d(data.lua_data["offset"]);
    build->Size = to_vi2d(data.lua_data["Parameters"]["CollisionSize"]);
    build->buildtime = data.lua_data["Parameters"]["BuildTime"];
    build->Interactable = data.lua_data["Parameters"]["Interactable"];

    // build->health = data.lua_data["Stats"]["Health"];
    build->Health = 0.4f;
    build->maxHealth = data.lua_data["Stats"]["MaxHealth"];
    if (build->Interactable)
        build->Health = build->maxHealth;

    build->isMine = data.lua_data["Stats"]["isMine"];
    if (build->isMine == true)
        build->Gold = data.lua_data["Production"]["Gold"];
    build->AttackSpeed = data.lua_data["Stats"]["AttackSpeed"];
    build->Owner = owner;
    build->FriendList = InitializeObject(owner);

    if (sol::table UnitProduction = data.lua_data["Production"]["Units"])
        for (int i = 0; i < UnitProduction.size(); i++) {
            build->unitproduction.push_back(UnitProduction[i + 1]);
        }
    if (sol::table Attacktypes = data.lua_data["Projectiles"]) {
        for (int i = 0; i < Attacktypes.size(); i++) {
            build->AttackTypes.push_back(Attacktypes[i + 1]);
        }
        build->CanAttack = true;
    }

    if (data.lua_data["Production"]["Research"] != sol::nil) {
        sol::table Researchtypes = data.lua_data["Production"]["Research"];
        for (int i = 0; i < Researchtypes.size(); i++) {
            build->researchproduction.push_back(Researchtypes[i + 1]);
        }
    }
    // make sure to update this when adding new GFXStates - enums don't magically connect to a string
    static std::map<std::string, Building::GFXState> States = {
        {"Normal", Building::Normal},
    };
    // create decals for each texture state
    for(auto& [ name, meta ] : data.texture_metadata){
        const Building::GFXState& state = States.at(name); // local state ref
        // load a decal texture and add to decal map

        if (meta.level_offsets.size() < build->Level.size()) {
            build->Level.erase(std::prev(build->Level.end()));
        }

        std::unique_ptr<olc::Decal> decal;
        decal.reset(new olc::Decal(TextureCache::GetCache().GetTexture(meta.tex_id)));
        build->decals.insert_or_assign(state, std::move(decal));
        // copy texture metadata
        build->textureMetadata.insert_or_assign(state, meta);
    }
    build->SetMask(Collidable::Mask(olc::vf2d(build->Size)));
    build->cType = Collidable::isBuilding;
    build->updatePriority = 0.9f; // lower priority

    engine.buildingManager->addNewBuilding(build);
    build->PosSize = { build->Position,build->Size };
    addObjectToList(build, build->PosSize);
    return build;
}

std::shared_ptr<Projectile> WorldManager::GenerateProjectile(const std::string& name, std::shared_ptr<WorldObject> pos, std::weak_ptr<WorldObject> Target) {
    Game_Engine& engine = Game_Engine::Current();
    auto to_vi2d = [](sol::table obj) -> olc::vi2d {
        int32_t x = obj[1],
            y = obj[2];
        return { x, y };
    };
    if (!engine.assetManager->ProjectileExists(name)) return nullptr;
    const auto& data = engine.assetManager->GetProjectileData(name);
    // make projectile
    std::shared_ptr<Projectile> proj;
    proj.reset(new Projectile());
    proj->predPosition = proj->Position = pos->Position;
   
    proj->TargetObj = Target;
    proj->Damage = data.lua_data["Stats"]["Damage"];
    proj->PSpeed = data.lua_data["Stats"]["Speed"];
    proj->Spinning = data.lua_data["Stats"]["Spin"];
    //this is fucking bullshit
    std::shared_ptr<Unit> unit;
    if (unit = std::dynamic_pointer_cast<Unit>(pos)) {
        proj->Damage += unit->fAttackDamage;
    }
    // TO DO: implement this part properly
    /*
    // make sure to update this when adding new GFXStates - enums don't magically connect to a string
    static std::map<std::string, Building::GFXState> States = {
        {"Normal", Projectile::Normal},
    };
    */

    //if (name == "ThrowingAxe")
    //    proj->projType = "Axe";
  //  proj->projType = name;
    
    // create decals for each texture state
    for (auto& [name, meta] : data.texture_metadata) {
        // const Building::GFXState& state = States[name]; // local state ref
        // load a decal texture and add to decal map
        proj->projType = name;
        std::unique_ptr<olc::Decal> decal;
        decal.reset(new olc::Decal(TextureCache::GetCache().GetTexture(meta.tex_id)));
        proj->decals.insert_or_assign(name, std::move(decal)); // TO DO: Use a graphic state
        // copy texture metadata
        proj->textureMetadata.insert_or_assign(name, meta);
    }
    //proj->SetMask(Collidable::Mask(olc::vf2d(proj->Size))); // TO DO: Figure out what the mask will be / and how to implement it
    proj->cType = Collidable::isProjectile;
    proj->updatePriority = 0.5f; // mid priority
    proj->PosSize = { proj->Position,olc::vf2d(32.f,32.f) };
    addObjectToList(proj,proj->PosSize);
    return proj;
}

void WorldManager::ImportMapData() {
    // to do: iterate map folder and find maps
    const std::vector<std::string> paths = {
        "TerraProject2.lua",
        "BasicMap.lua",
        "BasicMap2.lua",
    };

    for(const auto& path : paths) {
        mapList.emplace_back(std::make_shared<Map>("Assets/Maps/" + path));
        
    }
}

bool WorldManager::ChangeMap(const std::string& name) {
    auto& engine = Game_Engine::Current();
    auto it = std::find_if(mapList.begin(), mapList.end(), [&](const auto& map){ return map->name == name; });
    if(it == mapList.end()){
        std::cout << "Map " << name << " does not exist\n";
        return false;
    }
    currentMap = *it;
    olc::utils::geom2d::rect<float> cash = { olc::vf2d( 0.f,0.f ),olc::vf2d((float)currentMap->layerSize.x, (float)currentMap->layerSize.y)};
   /*
    quadtreeList.resize(cash);*/
    engine.hud->PreRenderMiniMap(*currentMap);

    engine.cmapmanager->CLayerdata.clear();
    engine.cmapmanager->CLayerdata.resize(currentMap->layerSize.x * currentMap->layerSize.y);
    int last_layer = int(currentMap->layerData.size()-1);
    for (int i = 0; i < currentMap->layerData[last_layer].size(); i++) {
        engine.cmapmanager->CLayerdata[i] = currentMap->layerData[1][i];
    }

    engine.cmapmanager->PlaceMapObjects();//Gulp
    return true;
}

std::vector<int> WorldManager::InitializeObject(int owner) {
    auto& engine = Game_Engine::Current();
    return engine.leaders->LeaderList[owner]->Allies[owner];
}