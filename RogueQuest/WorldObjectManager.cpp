#include "WorldObjectManager.h"
#include "Engine.h"
#include <ranges>

WorldManager::WorldManager() {
    auto& engine = Game_Engine::Current();

    garbageList.reserve(100);
    mapList.reserve(10);
}

WorldManager::~WorldManager() {
   // sigSorter.clear();
    mapList.clear();
	objectList.clear();
	garbageList.clear();
}

void WorldManager::PopulateNewObjects() {
    for (auto& obj : newObjectList) {
        auto item = objectList.insert(obj, getObjectQuadTreeArea(obj));
        obj->_ithome = item; // update local pointer to item home
    }
    newObjectList.clear();
    refreshObjectView();
}

void WorldManager::RelocateObject(std::shared_ptr<WorldObject> self) {
    
    objectList.relocate(self->_ithome, getObjectQuadTreeArea(self));
}

void WorldManager::refreshObjectView() {
    auto& engine = Game_Engine::Current();
    objectListView.clear();
    for (auto& item : objectList) {
        objectListView.emplace_back(item.item);
    }
}

void WorldManager::addObjectToList(std::shared_ptr<WorldObject> obj) {
    newObjectList.emplace_back(obj);
}

olc::utils::geom2d::rect<float> WorldManager::getObjectQuadTreeArea(const std::shared_ptr<WorldObject>& obj) const {
    if (auto col = std::dynamic_pointer_cast<Collidable>(obj)) {
	    switch (col->mask.type) {
            case Collidable::Mask::MASK_CIRCLE:
                return olc::utils::geom2d::rect<float>(
                    obj->Position - col->mask.origin - olc::vf2d(col->mask.radius, col->mask.radius),
                    olc::vf2d(col->mask.radius, col->mask.radius) * 2.f
                );
            case Collidable::Mask::MASK_RECTANGLE:
                return olc::utils::geom2d::rect<float>(
                    obj->Position - col->mask.origin,
                    col->mask.rect
                );
	    }
    }
    return olc::utils::geom2d::rect<float>( obj->Position, olc::vf2d(1.f, 1.f));
}

/*
void WorldManager::refreshQuadtreeView() {
    auto& engine = Game_Engine::Current();   
   
    auto mapsize = olc::vf2d(currentMap->layerSize) * olc::vf2d(currentMap->tileSize);
	objectQuadList.clear();

    for(auto obj : objectList) {
        if(auto obj = std::dynamic_pointer_cast<Collidable>(obj)){
            olc::utils::geom2d::rect<float> pos;

            switch(col->mask.type){
                case Collidable::Mask::MASK_CIRCLE:
                    pos.pos = col->Position - col->mask.origin - olc::vf2d(col->mask.radius,col->mask.radius);
                    pos.size = olc::vf2d(col->mask.radius,col->mask.radius) * 2.f;
                break;
                case Collidable::Mask::MASK_RECTANGLE:
                    pos.pos = col->Position - col->mask.origin;
                    pos.size = col->mask.rect;
                break;
                case Collidable::Mask::MASK_NONE:
                    pos.pos = col->Position;
                    pos.size = olc::vf2d(1.f, 1.f);
                break;
            }
            objectQuadList.insert(obj.get(), pos);
        }
    }
}
*/


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


    std::vector<std::reference_wrapper<std::shared_ptr<WorldObject>>> drawList;
    IterateObjectsQT(
        olc::utils::geom2d::rect<float>(engine.tv.GetWorldTL(), engine.tv.GetWorldVisibleArea()),
        [&](std::shared_ptr<WorldObject>& obj){
            drawList.emplace_back(obj);
            return true;
        }
    );

    std::ranges::sort(drawList, [](auto& a, auto& b) -> bool {
        return a.get()->drawDepth < b.get()->drawDepth;
    });

    for (auto& object : drawList) {
        object.get()->Draw(&engine.tv);
    }
}

void WorldManager::DestroyObject(WorldObject* self) {
    garbageList.emplace_back(self->_ithome->item);
}

void WorldManager::DestroyObject(const std::shared_ptr<WorldObject>& self) {
    DestroyObject(self.get());
}

void WorldManager::CollectGarbage() {
    auto& engine = Game_Engine::Current();

    if (garbageList.size()) { // if there is garbage, we need to clean the missing links
		
        for(auto& obj : garbageList){
            objectList.remove(obj->_ithome);
        }
        garbageList.clear();

        engine.unitManager->CollectGarbage(); // cleanup the unit manager garbage
        engine.buildingManager->CollectGarbage(); //cleanup the building manager garbage

        refreshObjectView(); // remember to refresh the object view when modifying the list
   }
}

bool WorldManager::IterateObjects(std::function<bool(std::shared_ptr<WorldObject>&)> cb) {
    for(auto& obj : objectList) {
        if(obj.item == nullptr) continue;
        if(!cb(obj.item)) break;
    }
    return true; // iterate completed successfully
}

bool WorldManager::IterateObjectsQT(olc::utils::geom2d::rect<float> searchArea, std::function<bool(std::shared_ptr<WorldObject>&)> cb) {
    auto found = objectList.search(searchArea); // position of object{} and size{} of search area
    for (auto& _obj : found) { // returns a list if found objects
        auto& obj = _obj->item;

        if (obj == nullptr) continue;
        if (!cb(obj)) break;//obj->item is the items obj->pitem is the pointer to the container
    }
    return true;
}

/* Factory Function */

std::shared_ptr<Unit> WorldManager::GenerateUnit(const std::string& name, int owner, olc::vf2d pos) {
    Game_Engine& engine = Game_Engine::Current();
    if(!engine.assetManager->UnitExists(name)) return nullptr;
    const auto& data = engine.assetManager->GetUnitData(name);
    // Make Unit
    
    std::shared_ptr<Unit> unit;
    unit.reset(new Unit()); // pass in primary unit type
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
    addObjectToList(unit);
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
    addObjectToList(build);
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
    //proj->projType = name;
    
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
    addObjectToList(proj);
    return proj;
}

std::shared_ptr<Projectile> WorldManager::GenerateProjectile(std::shared_ptr<Unit> Hero, float angle) {
    auto& engine = Game_Engine::Current();
    auto to_vi2d = [](sol::table obj) -> olc::vi2d {
        int32_t x = obj[1],
            y = obj[2];
        return { x, y };
    };
    const auto& Herodata = engine.assetManager->GetHeroData(Hero->sUnitName);
    if (!engine.assetManager->ProjectileExists(Herodata.projectileName)) return nullptr;
    const auto& data = engine.assetManager->GetProjectileData(Herodata.projectileName);
   

    std::shared_ptr<Projectile> proj;
    proj.reset(new Projectile());
    proj->predPosition = proj->Position = Hero->Position;

    proj->noTarget = true;
    proj->Damage = data.lua_data["Stats"]["Damage"];
    proj->PSpeed = data.lua_data["Stats"]["Speed"];
    proj->Spinning = data.lua_data["Stats"]["Spin"];
    proj->Damage += Hero->fAttackDamage;

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
    proj->SetMask(Collidable::Mask(olc::vf2d({5.f,5.f}))); // TO DO: Figure out what the mask will be / and how to implement it
    proj->cType = Collidable::isProjectile;
    proj->updatePriority = 0.5f; // mid priority
    addObjectToList(proj);
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
    olc::utils::geom2d::rect<float> worldMapArea = { {0.f,0.f}, olc::vf2d(currentMap->layerSize) * olc::vf2d(currentMap->tileSize)};

    engine.hud->PreRenderMiniMap(*currentMap);

    engine.cmapmanager->CLayerdata.clear();
    engine.cmapmanager->CLayerdata.resize(currentMap->layerSize.x * currentMap->layerSize.y);
    int last_layer = int(currentMap->layerData.size()-1);
    for (int i = 0; i < currentMap->layerData[last_layer].size(); i++) {
        engine.cmapmanager->CLayerdata[i] = currentMap->layerData[1][i];
    }

	objectList.resize(worldMapArea); // clear quadtree and reset to map size
    engine.cmapmanager->PlaceMapObjects();//Gulp


    return true;
}

std::vector<int> WorldManager::InitializeObject(int owner) {
    auto& engine = Game_Engine::Current();
    return engine.leaders->LeaderList[owner]->Allies[owner];
}