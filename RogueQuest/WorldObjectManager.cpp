#include "WorldObjectManager.h"
#include "Engine.h"

WorldManager::WorldManager() {
	objectList.reserve(1024 * 1024);
	garbageList.reserve(100);
}

WorldManager::~WorldManager() {
	objectList.clear();
	garbageList.clear();
}


void WorldManager::Update(float fElapedtime) {//Update last frames
	for (auto& object : objectList) {
		if (object == nullptr) continue;
		object->Update(fElapedtime);
	}
}

void WorldManager::Draw() {//Draw 
	Game_Engine& engine = Game_Engine::Current();
	for (auto& object : objectList) {
		if (object == nullptr) continue;
        object->Draw(&engine.tv);
	}
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

        garbageList.clear();
        engine.unitManager->CollectGarbage(); // cleanup the unit manager garbage
    }

}

std::shared_ptr<WorldObject> WorldManager::FindObject(size_t index) {
    return objectList.size() > index ? objectList.at(index) : nullptr;
}

bool WorldManager::IterateObjects(std::function<bool(std::shared_ptr<WorldObject>)> cb) {
    for(auto& object : objectList) {
        if(object == nullptr) continue;
        if(!cb(object)){
            return false; // user abort
        }
    }
    return true; // iterate completed successfully
}


/* Factory Function */

std::shared_ptr<Unit> WorldManager::GenerateUnit(const std::string& name, olc::vf2d pos) {
    Game_Engine& engine = Game_Engine::Current();

    if(!engine.assetManager->UnitExists(name)) return nullptr;
    const auto& data = engine.assetManager->GetUnitData(name);
    // Make Unit
    std::shared_ptr<Unit> unit;
    unit.reset(new Unit());
    unit->Position = pos;

    // Update Internal Values Of New Unit
    unit->sUnitName = data.lua_data["Name"]; //This is in top of .lua
    // Load Parameters
    unit->Unit_Collision_Radius = data.lua_data["Parameters"]["CollisionRadius"]; //I'm not in stats section of .lua
    // Load Stats
    unit->fHealth = data.lua_data["Stats"]["Health"]; unit->fMaxHealth = data.lua_data["Stats"]["MaxHealth"];
    unit->fMana = data.lua_data["Stats"]["Mana"];     unit->fMaxMana = data.lua_data["Stats"]["MaxMana"];
    unit->fAmmo = data.lua_data["Stats"]["Ammo"];     unit->fMaxAmmo = data.lua_data["Stats"]["MaxAmmo"];
    unit->fMoveSpeed = data.lua_data["Stats"]["MoveSpeed"];
    unit->fAttackRange = data.lua_data["Stats"]["AttackRange"];
    unit->fAttackDamage = data.lua_data["Stats"]["AttackDamage"];
    unit->fAttackSpeed = data.lua_data["Stats"]["AttackSpeed"];
    unit->fSpellCooldown = data.lua_data["Stats"]["SpellCooldown"];
    unit->fKnockBackResist = data.lua_data["Stats"]["KnockBackResist"];    
    // make sure to update this when adding new GFXStates - enums don't magically connect to a string
    static std::map<std::string, Unit::GFXState> States = {
        {"Walking", Unit::Walking},
        {"Attacking", Unit::Attacking},
        {"Dead", Unit::Dead}
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
    objectList.emplace_back(unit);
    engine.unitManager->addNewUnit(unit);
    return unit;
}

std::shared_ptr<Projectile> WorldManager::GenerateProjectile(olc::vf2d start, olc::vf2d Target) {
    std::shared_ptr<Projectile> proj;
    //Projectile.reset(new proj());
    proj->Position = start; proj->Target = Target;
    proj->Damage; proj->Velocity;

    objectList.emplace_back(proj);
    return proj;
}