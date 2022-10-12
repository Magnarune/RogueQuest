#include "HudManager.h"
#include "Engine.h"



HudManager::HudManager() {
    auto& engine = Game_Engine::Current();
    IconY = (float)engine.ScreenHeight() * 0.825f;
}
HudManager::~HudManager() {

}

void HudManager::UnitSelected() {
	auto& engine = Game_Engine::Current();

    std::shared_ptr<Unit> unitinfo = engine.unitManager->GetUnit();
    if (unitinfo) {
        const auto& data = engine.assetManager->GetUnitData(unitinfo->sUnitName);
        std::string _refname = unitinfo->sUnitName + "__tex";
        if (!engine.hud->decals.count(_refname)) {
            engine.hud->loadImage(_refname, data.head.tex_id);
        }
        float healthMod = unitinfo->Health / unitinfo->fMaxHealth;
        olc::Decal* decal = engine.hud->decals[_refname].get();

        engine.DrawPartialDecal({ 90.f, IconY - 8.f }, { 8.f,8.f }, engine.hud->decals["Attack"].get(), { 0,0 }, { 190,190 });
        engine.DrawStringDecal({ 99.f,  IconY - 4.f }, std::to_string((int)unitinfo->fAttackDamage), olc::WHITE, { 0.4f,0.4f });

        engine.DrawPartialDecal({ 90.f, IconY + 2.f }, { 8.f,8.f }, engine.hud->decals["Move"].get(), { 0,0 }, { 32,32 });
        engine.DrawStringDecal({ 99.f,  IconY + 6.f }, std::to_string((int)unitinfo->fMoveSpeed), olc::WHITE, { 0.4f,0.4f });

        engine.DrawPartialDecal({ 90.f, IconY + 12.f }, { 8.f,8.f }, engine.hud->decals["AtkSpeed"].get(), { 0,0 }, { 512,512 });
        engine.DrawStringDecal({ 99.f,  IconY + 16.f }, std::to_string((int)unitinfo->fAttackSpeed), olc::WHITE, { 0.4f,0.4f });

        engine.DrawStringDecal({ 66.f,  IconY - 6.f }, unitinfo->sUnitName, olc::BLUE, { 0.4f,0.4f });

        engine.DrawPartialDecal({ 68.f ,IconY }, { 16.f,16.f }, engine.hud->decals["Icon"].get(), { 0.f,0.f }, { 16,16 });
        engine.DrawPartialDecal({ 68.f ,IconY }, { 16.f,16.f }, decal, data.head.tl, data.head.sz);
        engine.DrawPartialDecal({ 66.5f, IconY - 1.5f }, { 18.5f,18.5f }, engine.hud->decals["Gui"].get(), { 872.f,218.f }, { 115.f,97.f });

        engine.DrawPartialDecal({ 64.f, IconY + 17.f }, { 22.f,4.f }, engine.hud->decals["HealthBoxBackground"].get(), { 0,0 }, { 128,32 });
        engine.DrawPartialDecal({ 64.f, IconY + 17.f }, { 22.f * healthMod,4.f }, engine.hud->decals["Health"].get(), { 0,0 }, { 128,32 });
        engine.DrawPartialDecal({ 64.f, IconY + 17.f }, { 22.f,4.f }, engine.hud->decals["HealthBox"].get(), { 0,0 }, { 128,32 });

        engine.DrawStringDecal({ 64.f, IconY + 22.f }, std::to_string((int)unitinfo->Health) + "/" + std::to_string((int)unitinfo->fMaxHealth), olc::RED, { 0.4f,0.4f });
        UnitAbilities(unitinfo);
    }
}

void HudManager::UnitsSelected(size_t ucount) {
	auto& engine = Game_Engine::Current();
    //draw backround box
    engine.DrawStringDecal({ 63.f,(float)engine.ScreenHeight() * 0.825f - 6 }, "Units: " + std::to_string(ucount), olc::WHITE, { 0.4f,0.4f });
    olc::vf2d sz = ucount <= 10 ? olc::vf2d(96, 16) : (ucount <= 20 ? olc::vf2d(96, 24) : olc::vf2d(96, 32));
    engine.DrawPartialDecal({ 58.f,(float)engine.ScreenHeight() * 0.825f - 3 }, sz, engine.hud->decals["Hud"].get(), { 0,64 }, { 96,32 });
    
    
    std::map<std::string, int> _types;
    engine.unitManager->IterateSelectedUnits([&](std::shared_ptr<Unit> unit) {
        if (!_types.count(unit->sUnitName)) _types.insert_or_assign(unit->sUnitName, 0);
        ++_types[unit->sUnitName];
        return true;



        });

  
        int i = 0;
        const size_t max_units = 30;
    for (const auto& [name, count] : _types) {
        const auto& data = engine.assetManager->GetUnitData(name);
        std::string _refname = name + "__tex";
        if (!engine.hud->decals.count(_refname)) {
            engine.hud->loadImage(_refname, data.head.tex_id);
        }
        olc::Decal* decal = engine.hud->decals[_refname].get();

        for (int r = 0; r < count; ++r) {
            float yoffset = i < 10 ? 0 : (i < 20 ? 8.f : 16.f);
            engine.DrawPartialDecal({ 63.f + 8 * (i % 10) ,(float)engine.ScreenHeight() * 0.825f + yoffset }, { 8.f,8.f }, engine.hud->decals["Icon"].get(), { 0.f,0.f }, { 16,16 });
            engine.DrawPartialDecal({ 63.f + 8 * (i % 10),(float)engine.ScreenHeight() * 0.825f + yoffset }, { 8.f,8.f }, decal, data.head.tl, data.head.sz);
            engine.DrawPartialDecal({ 63.f + 8 * (i % 10),(float)engine.ScreenHeight() * 0.825f + yoffset }, { 8.f,8.f }, engine.hud->decals["Gui"].get(), { 872.f,218.f }, { 115.f,97.f });
            if (++i == max_units) break;
        }
        if (ucount > max_units) {
            engine.DrawDecal({ 63.f + 80,(float)engine.ScreenHeight() * 0.825f + 18.f }, engine.hud->decals["Plus"].get(), { 0.025f,0.025f });
        }
    }
}

void HudManager::UnitAbilities(std::shared_ptr<Unit> unit) {
    auto& engine = Game_Engine::Current();

    const sol::table& data = unit->unitType.lua_data; // this is totally fine
    if(data["Parameters"]["Abilities"] == sol::nil ||
       data["Parameters"]["Abilities"]["Buildables"] == sol::nil ) return;

    const sol::table& buildables = data["Parameters"]["Abilities"]["Buildables"]; // we'll see later when we try to compile
    for (int i = 0; i < buildables.size(); i++) {
        std::string buildingName = buildables[i+1];
        if(!engine.assetManager->BuildingExists(buildingName)){
            std::cerr << "Invalid Building in Unit Ability: " << buildingName << "\n"; // oof this is going to print a lot
            continue;
        }

        const auto& data = engine.assetManager->GetBuildingData(buildingName);
        std::string _refname = buildingName + "__tex";
        if (!engine.hud->decals.count(_refname)) {
            engine.hud->loadImage(_refname, data.icon.tex_id);
        }
        olc::Decal* decal = engine.hud->decals[_refname].get();
        float yoffset = i < 3 ? 0 : (i < 20 ? 16.f : 32.f);

        // um .. what the *** are these doing here this isn't a draw function
        engine.DrawPartialDecal({ 186.f + 16.f * (i % 3) , IconY - 12.f + yoffset }, { 16.f,16.f }, engine.hud->decals["Icon"].get(), { 0.f,0.f }, { 16,16 });
        engine.DrawPartialDecal({ 186.f + 16.f * (i % 3) , IconY - 12.f + yoffset }, { 16.f,16.f }, decal, {0.f,0.f}, data.icon.fsz);
        engine.DrawPartialDecal({ 186.f + 16.f * (i % 3) , IconY - 12.f + yoffset }, { 16.5f,16.5f }, engine.hud->decals["Gui"].get(), { 872.f,218.f }, { 115.f,97.f });

        if (engine.inputmanager->Button({ 186.f + 16.f * (i % 3), IconY - 12.f + yoffset }, engine.GetMousePos(), { 16,16 })) {
            engine.hud->BuildMode = true;
            engine.hud->Object = buildingName;
        }
    }
}

void HudManager::BuildingSelected() {
	auto& engine = Game_Engine::Current();

    std::shared_ptr<Building> build = engine.buildingManager->GetBuilding();
    const auto& data = engine.assetManager->GetBuildingData(build->name);
    std::string _refname = build->name + "__tex";
    if (!engine.hud->decals.count(_refname)) {
        engine.hud->loadImage(_refname, data.icon.tex_id);
    }
    //
    float healthMod = build->Health / build->maxHealth;
    float productionMod;
    if(build->building)
         productionMod = build->m_fTimer / build->productiontime;
    
    
    olc::Decal* decal = engine.hud->decals[_refname].get();

    engine.DrawStringDecal({ 66.f,   IconY - 6.f }, build->name, olc::BLUE, { 0.4f,0.4f });
    engine.DrawPartialDecal({ 68.f  ,IconY }, { 16.f,16.f }, engine.hud->decals["Icon"].get(), { 0.f,0.f }, { 16,16 });
    engine.DrawPartialDecal({ 68.f  ,IconY }, olc::vf2d(data.icon.sz) / 2.f, decal, { 0.f,0.f }, data.icon.fsz);
    engine.DrawPartialDecal({ 66.5f, IconY - 1.5f }, { 18.5f,18.5f }, engine.hud->decals["Gui"].get(), { 872.f,218.f }, { 115.f,97.f });

   
    engine.DrawPartialDecal({ 64.f, IconY + 17.f }, { 22.f,4.f }, engine.hud->decals["HealthBoxBackground"].get(), { 0,0 }, { 128,32 });
    engine.DrawPartialDecal({ 64.f, IconY + 17.f }, { 22.f * healthMod,4.f }, engine.hud->decals["Health"].get(), { 0,0 }, { 128,32 });
    engine.DrawPartialDecal({ 64.f, IconY + 17.f }, { 22.f,4.f }, engine.hud->decals["HealthBox"].get(), { 0,0 }, { 128,32 });

    if (build->sentUnitPos != olc::vf2d(0.f, 0.f))
        engine.tv.DrawPartialDecal(build->sentUnitPos - olc::vf2d(0.f,14.f), olc::vf2d(8.f, 14.f), engine.hud->decals["flag"].get(), {0.f,0.f}, {113.f,218.f});

    if (build->building && build->productionQue.size() > 0) { // it was already implicit
        const auto& data = engine.assetManager->GetUnitData(build->productionQue.front());
        std::string _refname = build->productionQue.front() + "__tex";
        if (!engine.hud->decals.count(_refname)) {
            engine.hud->loadImage(_refname, data.head.tex_id);
        }
        olc::Decal* decal = engine.hud->decals[_refname].get();
        engine.DrawStringDecal({ 92.f,   IconY - 13.f },"Producing: " + build->unitproduced, olc::BLUE, {0.4f,0.4f});
        engine.DrawPartialDecal({ 92.f, IconY - 9.f }, { 44.f,4.f }, engine.hud->decals["HealthBoxBackground"].get(), { 0,0.f }, { 128,32 });
        engine.DrawPartialDecal({ 92.f, IconY - 9.f }, { 44.f * productionMod,4.f }, engine.hud->decals["Health"].get(), { 0.f,0.f }, { 128,32 },olc::YELLOW);
        engine.DrawPartialDecal({ 92.f, IconY - 9.f }, { 44.f,4.f }, engine.hud->decals["HealthBox"].get(), { 0,0 }, { 128,32 });

        engine.DrawPartialDecal({ 104.f , IconY - 4.f }, { 16.f,16.f }, engine.hud->decals["Icon"].get(), { 0.f,0.f }, { 16,16 });
        engine.DrawPartialDecal({ 104.f , IconY - 4.f }, { 16.f,16.f }, decal, data.head.tl, data.head.sz);
        engine.DrawPartialDecal({ 104.f , IconY - 4.f }, { 16.5f,16.5f }, engine.hud->decals["Gui"].get(), { 872.f,218.f }, { 115.f,97.f });
        if (build->productionQue.size() < 10) {
            engine.DrawPartialDecal({ 104.f , IconY + 15.f }, { 8.f,8.f }, engine.hud->decals["Icon"].get(), { 0.f,0.f }, { 16,16 });
            engine.DrawStringDecal({ 106.f, IconY + 17.f }, std::to_string(build->productionQue.size()), olc::GREEN, { 0.8f,0.8f });
            engine.DrawPartialDecal({ 104.f , IconY + 15.f }, { 8.5f,8.5f }, engine.hud->decals["Gui"].get(), { 872.f,218.f }, { 115.f,97.f });
        } else {
            engine.DrawPartialDecal({ 104.f , IconY + 15.f }, { 16.f,8.f }, engine.hud->decals["Icon"].get(), { 0.f,0.f }, { 16,16 });
            engine.DrawStringDecal({ 106.f, IconY + 17.f }, std::to_string(build->productionQue.size()), olc::GREEN, { 0.8f,0.8f });
            engine.DrawPartialDecal({ 104.f , IconY + 15.f }, { 16.5f,8.5f }, engine.hud->decals["Gui"].get(), { 872.f,218.f }, { 115.f,97.f });

        }

    }
    BuildingAbilities(build);
}

void HudManager::BuildingAbilities(std::shared_ptr<Building> building) {
    auto& engine = Game_Engine::Current();
    for (int i = 0; i < building->unitproduction.size(); i++) {
        const auto& data = engine.assetManager->GetUnitData(building->unitproduction[i]);
        std::string _refname = building->unitproduction[i] + "__tex";
        if (!engine.hud->decals.count(_refname)) {
            engine.hud->loadImage(_refname, data.head.tex_id);
        }
        olc::Decal* decal = engine.hud->decals[_refname].get();
        float yoffset = i < 3 ? 0 : (i < 20 ? 16.f : 32.f);
        engine.DrawPartialDecal({ 186.f + 16.f * (i % 3) , IconY - 12.f + yoffset }, { 16.f,16.f }, engine.hud->decals["Icon"].get(), { 0.f,0.f }, { 16,16 });
        engine.DrawPartialDecal({ 186.f + 16.f * (i % 3) , IconY - 12.f + yoffset }, { 16.f,16.f }, decal, data.head.tl, data.head.sz);
        engine.DrawPartialDecal({ 186.f + 16.f * (i % 3) , IconY - 12.f + yoffset }, { 16.5f,16.5f }, engine.hud->decals["Gui"].get(), { 872.f,218.f }, { 115.f,97.f });

        if (engine.inputmanager->Button({ 186.f + 16.f * (i % 3), IconY - 12.f + 0 }, engine.GetMousePos(), { 16,16 })) {
            building->productionQue.push(building->unitproduction[i]); 
        }
    }
}

void HudManager::BuildingsSelected(size_t bcount) {
    auto& engine = Game_Engine::Current();

}
