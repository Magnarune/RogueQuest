#include "WorldObjectManager.h"
#include "Engine.h"
#include <ranges>

WorldManager::WorldManager() {
    objectList.reserve(1024 * 512);
    garbageList.reserve(100);
    mapList.reserve(10);
}

WorldManager::~WorldManager() {
    mapList.clear();
	objectList.clear();
	garbageList.clear();
}

void WorldManager::Update(float delta) {//Update last frames
    currentMap->UpdateMap(delta);
    for (auto& object : objectList) {
		if (object == nullptr) continue;
        object->Update(delta);
	}

    // after updates
    for (auto& object : objectList) {
		if (object == nullptr) continue;
        object->AfterUpdate(delta);
	}
}

void WorldManager::Draw() {
    Game_Engine& engine = Game_Engine::Current();

    // sort the object list based on draw depth
    std::ranges::sort(objectList, [](auto& a, auto& b) -> bool {
        return a->drawDepth < b->drawDepth;
    });

    currentMap->DrawMap(&engine.tv);
    for (auto& object : objectList) {
        if (object == nullptr) continue;
        object->Draw(&engine.tv);
    }
    engine.particles->DrawParticles();
}

void WorldManager::DestroyObject(WorldObject* self) {
    auto me = std::find_if(objectList.begin(), objectList.end(), [&](const auto& obj){ return obj.get() == self; });
    assert(me != objectList.end()); // destroying an object should always find it in the list!
    garbageList.emplace_back(std::move(*me)); // move to garbage for final living space before death
}

void WorldManager::CollectGarbage() {
    auto& engine = Game_Engine::Current();

    size_t len = garbageList.size();
    if(len){ // if there is garbage, we need to clean the missing links
        size_t pos = 1;

        auto itr = objectList.begin();
        if(std::find_if(objectList.begin(), objectList.end(), [](const auto& obj){ return obj.get() != nullptr; }) == objectList.end()){
            objectList.clear();
        } else {
            do {
                if(!!itr->get()) continue; // keep valid objectList
                do { // keep swapping until there's no more invalid object here
                    if(objectList.begin() == objectList.end() - pos || itr == objectList.end() - pos) break;
                    std::swap(*itr, *(objectList.end() - pos) ); // move garbage to end of list
                    ++pos; // increase found garbage
                } while(!itr->get());

            } while(itr + 1 != objectList.end() && pos <= len && ++itr != objectList.end() - len); // continue looking for missing link garbage

            objectList.resize(objectList.size() - len); // slice off the head
            // for(int i=0; i < len; ++i) objectList.pop_back(); // slice off the garbage - resize instead?
        }
       // if(garbageList == engine.unitManager->unitList)
        std::shared_ptr<Collidable> cppdumbstuff;
        std::shared_ptr<Unit> lunit;
        std::shared_ptr<Building> lbuild;
        for (int i = 0; i < garbageList.size(); i++) {
            if (cb && cu)
                continue;
            if (cppdumbstuff = std::dynamic_pointer_cast<Collidable>(garbageList[i])) {
                if (cppdumbstuff = std::dynamic_pointer_cast<Unit>(garbageList[i])) {
                    if(cppdumbstuff->cType == Collidable::isUnit)
                        cu = true;
                }
                if (cppdumbstuff = std::dynamic_pointer_cast<Building>(garbageList[i])) {
                    cb = true;
                }
            }
        }
        garbageList.clear();
       // if(cb)
            //engine.buildingManager->CollectGarbage(); //cleanup the building manager garbage
        //if(cu)
        cb, cu = false;
    }
            engine.unitManager->CollectGarbage(); // cleanup the unit manager garbage

}

std::shared_ptr<WorldObject> WorldManager::FindObject(size_t index) {
    return objectList.size() > index ? objectList.at(index) : nullptr;
}

bool WorldManager::IterateObjects(std::function<bool(std::shared_ptr<WorldObject>)> cb) {
    for(auto& object : objectList) {
        if(object == nullptr) continue;
        cb(object);
    }
    return true; // iterate completed successfully
}


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
    unit->Target = std::nullopt;//{0.f,0.f};
       
    // Load Parameters
    unit->Unit_Collision_Radius = data.lua_data["Parameters"]["CollisionRadius"]; //I'm not in stats section of .lua
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
    objectList.emplace_back(unit);
    engine.unitManager->addNewUnit(unit);
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
    build->name = data.lua_data["Name"];

    

    build->Size = to_vi2d(data.lua_data["Parameters"]["CollisionSize"]);
    build->buildtime = data.lua_data["Parameters"]["BuildTime"];
    build->Interactable = data.lua_data["Parameters"]["Interactable"];
    
   // build->health = data.lua_data["Stats"]["Health"];
    build->Health = 0.4f;
    build->maxHealth = data.lua_data["Stats"]["MaxHealth"];
    if (build->Interactable) 
        build->Health = build->maxHealth;      
    
    build->isMine = data.lua_data["Stats"]["isMine"];
    if(build->isMine == true)
        build->Gold = data.lua_data["Production"]["Gold"];
    build->AttackSpeed = data.lua_data["Stats"]["AttackSpeed"];
    build->Owner = owner;
    build->FriendList = InitializeObject(owner);

   if( sol::table UnitProduction = data.lua_data["Production"]["Units"])
       for (int i = 0; i < UnitProduction.size(); i++) {
           build->unitproduction.push_back(UnitProduction[i + 1]);
       }
   if (sol::table Attacktypes = data.lua_data["Projectiles"]) {
       for (int i = 0; i < Attacktypes.size(); i++) {
           build->AttackTypes.push_back(Attacktypes[i + 1]);
       }
       build->CanAttack = true;
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
    objectList.emplace_back(build);
    engine.buildingManager->addNewBuilding(build);
    return build;
}

std::shared_ptr<Projectile> WorldManager::GenerateProjectile(const std::string& name, olc::vf2d pos, std::weak_ptr<WorldObject> Target) {
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
    proj->predPosition = proj->Position = pos;
    proj->TargetObj = Target;
    proj->Damage = data.lua_data["Stats"]["Damage"];
    proj->PSpeed = data.lua_data["Stats"]["Speed"];
    proj->Spinning = data.lua_data["Stats"]["Spin"];

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

    objectList.emplace_back(proj);
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
    auto it = std::find_if(mapList.begin(), mapList.end(), [&](const auto& map){ return map->name == name; });
    if(it == mapList.end()){
        std::cout << "Map " << name << " does not exist\n";
        return false;
    }
    currentMap = *it;
    return true;
}

std::vector<int> WorldManager::InitializeObject(int owner) {
    auto& engine = Game_Engine::Current();
    return engine.leaders->LeaderList[owner]->Allies[owner];
}