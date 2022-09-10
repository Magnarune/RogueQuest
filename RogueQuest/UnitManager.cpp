#include "UnitManager.h"
#include "Engine.h"


UnitManager::UnitManager() {
    unitList.reserve(1024 * 1024); // preserve lots of unit ptrs
}


std::shared_ptr<Unit> UnitManager::GenerateUnit(const std::string& name, olc::vf2d pos) {
    Game_Engine& engine = Game_Engine::Current();

    if(!engine.assetManager->UnitExists(name)) return nullptr;

    const auto& data = engine.assetManager->GetUnitData(name);

    // Make Unit
    std::shared_ptr<Unit> unit;
    unit.reset(new Unit());

    unit->vUnitPosition = pos;

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

    unitList.emplace_back(unit);

    return unit;
}

size_t UnitManager::GetUnitCount(const std::string& name) {//got it
    return std::accumulate(unitList.begin(), unitList.end(), 0ULL,
        [&](size_t n, const auto& unit) -> size_t {
            return n + (unit->sUnitName == name);
        });
}

std::shared_ptr<Unit> UnitManager::GetUnit(const std::string& name, size_t index) {
    size_t n = 0;
    for(auto& unit : unitList){
        if(unit->sUnitName == name && n++ == index){
            return unit;
        }
    }
    return nullptr; // couldn't find unit
}
void UnitManager::SelectUnits(olc::vf2d Initial, olc::vf2d Final) {    
    Game_Engine& engine = Game_Engine::Current();
    engine.tv.DrawLineDecal(Final,   { Initial.x,Final.y }, olc::RED);//Draw Rect
    engine.tv.DrawLineDecal(Initial, { Initial.x,Final.y }, olc::RED);
    engine.tv.DrawLineDecal(Initial, { Final.x,Initial.y }, olc::RED);
    engine.tv.DrawLineDecal(Final,   { Final.x,Initial.y }, olc::RED);
  //  engine.tv.DrawLineDecal(Initial, Final);
    for (auto& unit : unitList)
    {
        if (unit->vUnitPosition.x  > std::min(Initial.x, Final.x) && 
            unit->vUnitPosition.y  > std::min(Initial.y, Final.y) &&
            unit->vUnitPosition.x  < std::max(Final.x, Initial.x) &&
            unit->vUnitPosition.y  < std::max(Final.y, Initial.y))
        {
           unit->bSelected = true;
        }
        else { unit->bSelected = false; };        
    }


}
void UnitManager::MoveUnits(olc::vf2d Target)
{
    for (auto& unit : unitList) {
        if (unit->bSelected == true)
        {
            unit->MarchingtoTarget(Target);
        }
    }
}
void UnitManager::Update(float delta) {
    // units update here
    for(auto& unit : unitList){
        unit->CheckCollision(unitList);
        unit->UpdateUnit(delta);
    }
}

void UnitManager::Draw() {
    Game_Engine& engine = Game_Engine::Current();

    for(auto& unit : unitList){
        unit->DrawUnit(&engine.tv);
    }

}