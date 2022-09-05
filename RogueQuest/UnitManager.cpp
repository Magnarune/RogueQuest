#include "UnitManager.h"
#include "Engine.h"


UnitManager::UnitManager() {
    unitList.reserve(1024 * 1024 * 1024); // preserve lots of unit ptrs
}


std::shared_ptr<Unit> UnitManager::GenerateUnit(const std::string& name) {
    std::shared_ptr<Unit> unit;
    unit.reset(new Unit());

    Game_Engine& engine = Game_Engine::Current();

    const auto& data = engine.assetManager->GetUnitData(name);
    
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
    
    static std::map<std::string, Unit::GFXState> States = {
        {"Walking", Unit::Walking},
        {"Attacking", Unit::Attacking},
        {"Dead", Unit::Dead}
    };

    // create decals for each texture state
    for(auto& [ name, id ] : data.tex_ids){
        std::unique_ptr<olc::Decal> decal;
        decal.reset(new olc::Decal(TextureCache::GetCache().GetTexture(id)));

        // make sure to update this when adding new GFXStates - enums don't magically connect to a string

        unit->decals.insert_or_assign(States[name], std::move(decal));
    }
    // assign animation lengths - ehh so rudimentary :(
    for(auto& [ name, len ] : data.ani_lens){
        unit->AnimationLengths.insert_or_assign(States[name], len);
    }

    unitList.emplace_back(unit);

    return unit;
}

void UnitManager::Update(float delta) {
    // units update here
}