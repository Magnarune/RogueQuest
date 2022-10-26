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
       
        engine.hud->DrawCenteredStringDecal({ 103.f,  IconY - 4.f }, std::to_string((int)unitinfo->fAttackDamage), olc::WHITE, { 0.4f,0.4f });

        engine.DrawPartialDecal({ 90.f, IconY + 2.f }, { 8.f,8.f }, engine.hud->decals["Move"].get(), { 0,0 }, { 32,32 });
        engine.hud->DrawCenteredStringDecal({ 103.f,  IconY + 6.f }, std::to_string((int)unitinfo->fMoveSpeed), olc::WHITE, { 0.4f,0.4f });

        engine.DrawPartialDecal({ 90.f, IconY + 12.f }, { 8.f,8.f }, engine.hud->decals["AtkSpeed"].get(), { 0,0 }, { 512,512 });
        engine.hud->DrawCenteredStringDecal({ 103.f,  IconY + 16.f }, std::to_string((int)unitinfo->fAttackSpeed), olc::WHITE, { 0.4f,0.4f });

        engine.hud->DrawCenteredStringDecal({ 77.f,  IconY - 4.f }, unitinfo->sUnitName, olc::BLUE, { 0.4f,0.4f });

        engine.DrawPartialDecal({ 68.f ,IconY }, { 16.f,16.f }, engine.hud->decals["Icon"].get(), { 0.f,0.f }, { 16,16 });
        engine.DrawPartialDecal({ 68.f ,IconY }, { 16.f,16.f }, decal, data.head.tl, data.head.sz);
        engine.DrawPartialDecal({ 66.5f, IconY - 1.5f }, { 18.5f,18.5f }, engine.hud->decals["Gui"].get(), { 872.f,218.f }, { 115.f,97.f });

        engine.DrawPartialDecal({ 64.f, IconY + 17.f }, { 22.f,4.f }, engine.hud->decals["HealthBoxBackground"].get(), { 0,0 }, { 128,32 });
        engine.DrawPartialDecal({ 64.f, IconY + 17.f }, { 22.f * healthMod,4.f }, engine.hud->decals["Health"].get(), { 0,0 }, { 128,32 });
        engine.DrawPartialDecal({ 64.f, IconY + 17.f }, { 22.f,4.f }, engine.hud->decals["HealthBox"].get(), { 0,0 }, { 128,32 });

        engine.hud->DrawCenteredStringDecal({ 74.f, IconY + 24.f }, std::to_string((int)unitinfo->Health) + "/" + std::to_string((int)unitinfo->fMaxHealth), olc::RED, { 0.4f,0.4f });
        UnitAbilities(unitinfo);
    }
}

void HudManager::UnitsSelected(size_t ucount) {
	auto& engine = Game_Engine::Current();
    //draw backround box
    engine.hud->DrawCenteredStringDecal({ 78.f,(float)engine.ScreenHeight() * 0.825f - 6 }, "Units: " + std::to_string(ucount), olc::WHITE, { 0.4f,0.4f });
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
    bool hover = false;
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
        bool rq = CheckRequirements(unit->Owner, data);
       
        engine.DrawPartialDecal({ 186.f + 16.f * (i % 3) , IconY - 12.f + yoffset }, { 16.f,16.f }, engine.hud->decals["Icon"].get(), { 0.f,0.f }, { 16,16 });
        engine.DrawPartialDecal({ 186.f + 16.f * (i % 3) , IconY - 12.f + yoffset }, { 16.f,16.f }, decal, {0.f,0.f}, data.icon.fsz, rq ? olc::WHITE : olc::GREY);
        engine.DrawPartialDecal({ 186.f + 16.f * (i % 3) , IconY - 12.f + yoffset }, { 16.5f,16.5f }, engine.hud->decals["Gui"].get(), { 872.f,218.f }, { 115.f,97.f });

        if (engine.inputmanager->InsideBox({ 186.f + 16.f * (i % 3) , IconY - 12.f + yoffset }, engine.GetMousePos(), { 16.f,16.f })) {
            if (engine.inputmanager->Hover({ 186.f + 16.f * (i % 3) , IconY - 12.f + yoffset }, engine.GetMousePos(), { 16.f,16.f }, 1.f, 0.1f)) {
                engine.hud->DrawDescription(data.Description, engine.GetMousePos());

            }
            hover = true;
        }

        if (engine.inputmanager->Button({ 186.f + 16.f * (i % 3), IconY - 12.f + yoffset }, engine.GetMousePos(), { 16,16 })&& rq) {
            auto to_vi2d = [](sol::table obj) -> olc::vi2d {
                int32_t x = obj[1],
                    y = obj[2];
                return { x, y };
            };//PUT ALL DATA OF THE POTENTIAL BUILDING INTO POTENTIAL BUILDING    
            engine.hud->potBuilding.reset(new Building);
            const auto& potdata = engine.assetManager->GetBuildingData(buildingName);
            engine.hud->BuildMode = true;//InputManager goes to build Mode
            engine.hud->potBuilding->name = buildingName;
            engine.hud->potBuilding->Size = to_vi2d(data.lua_data["Parameters"]["CollisionSize"]);
        }
    }
    if (!hover)
        engine.inputmanager->triggertime = 0.f;
}

void HudManager::BuildingSelected() {
	auto& engine = Game_Engine::Current();

    std::shared_ptr<Building> build = engine.buildingManager->GetBuilding();
    const auto& data = engine.assetManager->GetBuildingData(build->name);
    std::string _refname = build->name + "__tex";
    if (!engine.hud->decals.count(_refname)) {
        engine.hud->loadImage(_refname, data.icon.tex_id);
    }

    float healthMod = build->Health / build->maxHealth;
    float productionMod=0.f;
    if(build->building)
         productionMod = build->m_fTimer / build->productiontime;    
    
    olc::Decal* decal = engine.hud->decals[_refname].get();

    engine.hud->DrawCenteredStringDecal({ 77.f,   IconY - 4.f }, build->name, olc::BLUE, { 0.4f,0.4f });
    engine.DrawPartialDecal({ 68.f  ,IconY }, { 16.f,16.f }, engine.hud->decals["Icon"].get(), { 0.f,0.f }, { 16,16 });
    engine.DrawPartialDecal({ 68.f  ,IconY }, olc::vf2d(data.icon.sz) / 2.f, decal, { 0.f,0.f }, data.icon.fsz);
    engine.DrawPartialDecal({ 66.5f, IconY - 1.5f }, { 18.5f,18.5f }, engine.hud->decals["Gui"].get(), { 872.f,218.f }, { 115.f,97.f });

   
    engine.DrawPartialDecal({ 64.f, IconY + 17.f }, { 22.f,4.f }, engine.hud->decals["HealthBoxBackground"].get(), { 0,0 }, { 128,32 });
    engine.DrawPartialDecal({ 64.f, IconY + 17.f }, { 22.f * healthMod,4.f }, engine.hud->decals["Health"].get(), { 0,0 }, { 128,32 });
    engine.DrawPartialDecal({ 64.f, IconY + 17.f }, { 22.f,4.f }, engine.hud->decals["HealthBox"].get(), { 0,0 }, { 128,32 });
    
    if (build->sentUnitPos != olc::vf2d(0.f, 0.f))
        engine.tv.DrawPartialDecal(build->sentUnitPos - olc::vf2d(0.f,14.f), olc::vf2d(8.f, 14.f), engine.hud->decals["flag"].get(), {0.f,0.f}, {113.f,218.f});
    
    if (build->building) {
        if (build->production == build->isUnit) {
            const auto& data = engine.assetManager->GetUnitData(build->unitproduced);
            std::string _refname = build->unitproduced + "__tex";
            if (!engine.hud->decals.count(_refname)) {
                engine.hud->loadImage(_refname, data.head.tex_id);
            }
            
            olc::Decal* decal = engine.hud->decals[_refname].get();
            
            engine.hud->DrawCenteredStringDecal({ 109.f,   IconY - 11.f }, "Producing: " + build->unitproduced, olc::BLUE, { 0.4f,0.4f });
            engine.DrawPartialDecal({ 92.f, IconY - 9.f }, { 44.f,4.f }, engine.hud->decals["HealthBoxBackground"].get(), { 0,0.f }, { 128,32 });
            engine.DrawPartialDecal({ 92.f, IconY - 9.f }, { 44.f * productionMod,4.f }, engine.hud->decals["Health"].get(), { 0.f,0.f }, { 128,32 }, olc::YELLOW);
            engine.DrawPartialDecal({ 92.f, IconY - 9.f }, { 44.f,4.f }, engine.hud->decals["HealthBox"].get(), { 0,0 }, { 128,32 });
            
            engine.DrawPartialDecal({ 104.f , IconY - 4.f }, { 16.f,16.f }, engine.hud->decals["Icon"].get(), { 0.f,0.f }, { 16,16 });
            engine.DrawPartialDecal({ 104.f , IconY - 4.f }, { 16.f,16.f }, decal, data.head.tl, data.head.sz);
            engine.DrawPartialDecal({ 104.f , IconY - 4.f }, { 16.5f,16.5f }, engine.hud->decals["Gui"].get(), { 872.f,218.f }, { 115.f,97.f });
        } else {
            if (build->production == build->isResearch) {
                const auto& data = engine.assetManager->GetResearchData(build->researchproduced);
                std::string _refname = build->researchproduced + "__tex";
                if (!engine.hud->decals.count(_refname)) {
                    engine.hud->loadImage(_refname, data.icon.tex_id);
                }
                olc::Decal* decal = engine.hud->decals[_refname].get();
                engine.hud->DrawCenteredStringDecal({ 109.f,   IconY - 11.f }, "Researching: " + build->researchproduced, olc::BLUE, { 0.4f,0.4f });
                engine.DrawPartialDecal({ 92.f, IconY - 9.f }, { 44.f,4.f }, engine.hud->decals["HealthBoxBackground"].get(), { 0,0.f }, { 128,32 });
                engine.DrawPartialDecal({ 92.f, IconY - 9.f }, { 44.f * productionMod,4.f }, engine.hud->decals["Health"].get(), { 0.f,0.f }, { 128,32 }, olc::BLUE);
                engine.DrawPartialDecal({ 92.f, IconY - 9.f }, { 44.f,4.f }, engine.hud->decals["HealthBox"].get(), { 0,0 }, { 128,32 });

                engine.DrawPartialDecal({ 104.f , IconY - 4.f }, { 16.f,16.f }, engine.hud->decals["Icon"].get(), { 0.f,0.f }, { 16,16 });
                engine.DrawPartialDecal({ 104.f , IconY - 4.f }, { 16.f,16.f }, decal, {0.f,0.f}, data.icon.fsz);
                engine.DrawPartialDecal({ 104.f , IconY - 4.f }, { 16.5f,16.5f }, engine.hud->decals["Gui"].get(), { 872.f,218.f }, { 115.f,97.f });

            }
        }
        
        if (engine.inputmanager->Button({ 104.f , IconY - 4.f }, engine.GetMousePos(), { 16.f,16.f }))
            build->UnProduceObject();


        if (build->productionQue.size() < 10) {
            engine.DrawPartialDecal({ 104.f , IconY + 15.f }, { 8.f,8.f }, engine.hud->decals["Icon"].get(), { 0.f,0.f }, { 16,16 });
            engine.hud->DrawCenteredStringDecal({ 108.7f, IconY + 20.f }, std::to_string(build->productionQue.size() + 1), olc::GREEN, { 0.8f,0.8f });
            engine.DrawPartialDecal({ 104.f , IconY + 15.f }, { 8.5f,8.5f }, engine.hud->decals["Gui"].get(), { 872.f,218.f }, { 115.f,97.f });
        } else {
            if (build->productionQue.size() < 100) {
                engine.DrawPartialDecal({ 104.f , IconY + 15.f }, { 16.f,8.f }, engine.hud->decals["Icon"].get(), { 0.f,0.f }, { 16,16 });
                engine.hud->DrawCenteredStringDecal({ 112.f, IconY + 20.f }, std::to_string(build->productionQue.size()+1), olc::GREEN, { 0.8f,0.8f });
                engine.DrawPartialDecal({ 104.f , IconY + 15.f }, { 16.5f,8.5f }, engine.hud->decals["Gui"].get(), { 872.f,218.f }, { 115.f,97.f });
            } else {
                engine.DrawPartialDecal({ 101.f , IconY + 15.f }, { 23.f,8.f }, engine.hud->decals["Icon"].get(), { 0.f,0.f }, { 16,16 });
                engine.hud->DrawCenteredStringDecal({ 112.f, IconY + 20.f }, std::to_string(build->productionQue.size()+1), olc::GREEN, { 0.8f,0.8f });
                engine.DrawPartialDecal({ 101.f , IconY + 15.f }, { 23.5f,8.5f }, engine.hud->decals["Gui"].get(), { 872.f,218.f }, { 115.f,97.f });
            }

        }
    }
    BuildingAbilities(build);
}

void HudManager::BuildingAbilities(std::shared_ptr<Building> building) {
    auto& engine = Game_Engine::Current();
    switch (selection) {
    case None:
        if (building->unitproduction.size() > 0) {
            engine.DrawPartialDecal({ 186.f + 16.f  , IconY - 12.f + 32.f }, { 16.f,16.f }, engine.hud->decals["Icon"].get(), { 0.f,0.f }, { 16,16 });
            engine.DrawPartialDecal({ 186.f + 16.f  , IconY - 12.f + 32.f }, { 16.f,16.f }, engine.hud->decals["Units"].get(), { 0.f,0.f }, { 64,64 });
            engine.DrawPartialDecal({ 186.f + 16.f  , IconY - 12.f + 32.f }, { 16.5f,16.5f }, engine.hud->decals["Gui"].get(), { 872.f,218.f }, { 115.f,97.f });
            if (engine.inputmanager->Button({ 186.f + 16.f  , IconY - 12.f + 32.f }, engine.GetMousePos(), { 16,16 }))
                selection = Producable;
        }      
        
        if (building->researchproduction.size() > 0) {
            engine.DrawPartialDecal({ 186.f + 32.f  , IconY - 12.f + 32.f }, { 16.f,16.f }, engine.hud->decals["Icon"].get(), { 0.f,0.f }, { 16,16 });
            engine.DrawPartialDecal({ 186.f + 32.f  , IconY - 12.f + 32.f }, { 16.f,16.f }, engine.hud->decals["Research"].get(), { 0.f,0.f }, { 64,64 });
            engine.DrawPartialDecal({ 186.f + 32.f  , IconY - 12.f + 32.f }, { 16.5f,16.5f }, engine.hud->decals["Gui"].get(), { 872.f,218.f }, { 115.f,97.f });
            if (engine.inputmanager->Button({ 186.f + 32.f  , IconY - 12.f + 32.f }, engine.GetMousePos(), { 16,16 }))
                selection = Research;
        }
        break;
    case Research:
        ResearchAbilites(building);
        break;
    case Producable:
        ProductionAbilites(building);
        break;
    }
}

void HudManager::ResearchAbilites(std::shared_ptr<Building> building) {
    auto& engine = Game_Engine::Current();
    bool hover = false;
    for (int i = 0; i < building->researchproduction.size(); i++) {
        bool test = false;
        for (auto& research : Hide_Research)
            if (building->building && building->researchproduction[i] == research)
                test = true;
        if (test) {
            test = false;
            continue;
        }
        if (!engine.assetManager->ResearchExists(building->researchproduction[i]))
            continue;
        const auto& data = engine.assetManager->GetResearchData(building->researchproduction[i]);
        std::string _refname = building->researchproduction[i] + "__tex";
        if (!engine.hud->decals.count(_refname)) {
            engine.hud->loadImage(_refname, data.icon.tex_id);
        }
        olc::Decal* decal = engine.hud->decals[_refname].get();
        bool rq = CheckRequirements(building->Owner, data, building->researchproduction[i]);
        float yoffset = i < 3 ? 0 : (i < 6 ? 16.f : 32.f);
        engine.DrawPartialDecal({ 186.f + 16.f * (i % 3) , IconY - 12.f + yoffset }, { 16.f,16.f }, engine.hud->decals["Icon"].get(), { 0.f,0.f }, { 16,16 });
        engine.DrawPartialDecal({ 186.f + 16.f * (i % 3) , IconY - 12.f + yoffset }, olc::vf2d(data.icon.sz) /2.f, decal, {0.f,0.f}, data.icon.fsz, rq ? olc::WHITE : olc::GREY);
        engine.DrawPartialDecal({ 186.f + 16.f * (i % 3) , IconY - 12.f + yoffset }, { 16.5f,16.5f }, engine.hud->decals["Gui"].get(), { 872.f,218.f }, { 115.f,97.f });
        

       
        if (engine.inputmanager->InsideBox({ 186.f + 16.f * (i % 3) , IconY - 12.f + yoffset }, engine.GetMousePos(), { 16.f,16.f })){
            if(engine.inputmanager->Hover({ 186.f + 16.f * (i % 3) , IconY - 12.f + yoffset }, engine.GetMousePos(), { 16.f,16.f }, 1.f, 0.1f)){
                engine.hud->DrawDescription(data.Description, engine.GetMousePos());
                      
            }
            hover = true;
        }
        if (engine.inputmanager->Button({ 186.f + 16.f * (i % 3), IconY - 12.f + yoffset }, engine.GetMousePos(), { 16,16 }) && rq) {
            Hide_Research.push_back(building->researchproduction[i]);
           int cost= engine.leaders->LeaderList[building->Owner]->CheckCost(building->researchproduction[i]);
            engine.leaders->LeaderList[building->Owner]->Gold -= engine.researchmanager->Researchables[building->researchproduction[i]]->Cost[cost];
            building->Refunds.push(engine.researchmanager->Researchables[building->researchproduction[i]]->Cost[cost]);
            building->productionQue.push(building->researchproduction[i]);
        }
    }

    if (!hover)
        engine.inputmanager->triggertime = 0.f;

    engine.DrawPartialDecal({ 186.f + 48.f  , IconY - 12.f + 32.f }, { 16.f,16.f }, engine.hud->decals["Icon"].get(), { 0.f,0.f }, { 16,16 });
    engine.DrawPartialDecal({ 186.f + 48.f  , IconY - 12.f + 32.f }, { 16.f,16.f }, engine.hud->decals["Back"].get(), { 0.f,0.f }, { 45,45 });
    engine.DrawPartialDecal({ 186.f + 48.f  , IconY - 12.f + 32.f }, { 16.5f,16.5f }, engine.hud->decals["Gui"].get(), { 872.f,218.f }, { 115.f,97.f });
    if (engine.inputmanager->Button({ 186.f + 48.f  , IconY - 12.f + 32.f }, engine.GetMousePos(), { 16,16 }))
        selection = None;

}

void HudManager::ProductionAbilites(std::shared_ptr<Building> building) {
    auto& engine = Game_Engine::Current();
    bool hover = false;
    for (int i = 0; i < building->unitproduction.size(); i++) {
        const auto& data = engine.assetManager->GetUnitData(building->unitproduction[i]);
        std::string _refname = building->unitproduction[i] + "__tex";
        if (!engine.hud->decals.count(_refname)) {
            engine.hud->loadImage(_refname, data.head.tex_id);
        }
        olc::Decal* decal = engine.hud->decals[_refname].get();

        bool rq = CheckRequirements(building->Owner, data);//this is the Bool Check

        float yoffset = i < 3 ? 0 : (i < 6 ? 16.f : 32.f);
        engine.DrawPartialDecal({ 186.f + 16.f * (i % 3) , IconY - 12.f + yoffset }, { 16.f,16.f }, engine.hud->decals["Icon"].get(), { 0.f,0.f }, { 16,16 });
        engine.DrawPartialDecal({ 186.f + 16.f * (i % 3) , IconY - 12.f + yoffset }, { 16.f,16.f }, decal, data.head.tl, data.head.sz, rq ? olc::WHITE : olc::GREY);
        engine.DrawPartialDecal({ 186.f + 16.f * (i % 3) , IconY - 12.f + yoffset }, { 16.5f,16.5f }, engine.hud->decals["Gui"].get(), { 872.f,218.f }, { 115.f,97.f });





        if (engine.inputmanager->InsideBox({ 186.f + 16.f * (i % 3) , IconY - 12.f + yoffset }, engine.GetMousePos(), { 16.f,16.f })) {
            if (engine.inputmanager->Hover({ 186.f + 16.f * (i % 3) , IconY - 12.f + yoffset }, engine.GetMousePos(), { 16.f,16.f }, 1.f, 0.1f)) {
                engine.hud->DrawDescription(data.Description, engine.GetMousePos());

            }
            hover = true;
        }








        if (engine.inputmanager->Button({ 186.f + 16.f * (i % 3), IconY - 12.f + yoffset }, engine.GetMousePos(), { 16,16 }) && rq) {
            engine.leaders->LeaderList[building->Owner]->Gold -= data.Cost;
            building->Refunds.push(data.Cost);
            building->productionQue.push(building->unitproduction[i]);
        }
    }
    if (!hover)
        engine.inputmanager->triggertime = 0.f;
    engine.DrawPartialDecal({ 186.f + 48.f  , IconY - 12.f + 32.f }, { 16.f,16.f }, engine.hud->decals["Icon"].get(), { 0.f,0.f }, { 16,16 });
    engine.DrawPartialDecal({ 186.f + 48.f  , IconY - 12.f + 32.f }, { 16.f,16.f }, engine.hud->decals["Back"].get(), { 0.f,0.f }, { 45,45 });
    engine.DrawPartialDecal({ 186.f + 48.f  , IconY - 12.f + 32.f }, { 16.5f,16.5f }, engine.hud->decals["Gui"].get(), { 872.f,218.f }, { 115.f,97.f });
    if (engine.inputmanager->Button({ 186.f + 48.f  , IconY - 12.f + 32.f }, engine.GetMousePos(), { 16,16 }))
        selection = None;
}

void HudManager::BuildingsSelected(size_t bcount) {
    auto& engine = Game_Engine::Current();

}

bool HudManager::CheckRequirements(int Owner , cAssets::UnitType object){
    auto& engine = Game_Engine::Current();
    for (int i = 0; i < object.Requirements.size(); i++)
        if (std::find(engine.leaders->LeaderList[Owner]->ResearchUpgrades.begin(),
            engine.leaders->LeaderList[Owner]->ResearchUpgrades.end(), object.Requirements[i]) == engine.leaders->LeaderList[Owner]->ResearchUpgrades.end() && 
            std::find(engine.leaders->LeaderList[Owner]->Unlockes.begin(),
                engine.leaders->LeaderList[Owner]->Unlockes.end(), object.Requirements[i]) == engine.leaders->LeaderList[Owner]->Unlockes.end())
            return false;
    if (engine.leaders->LeaderList[Owner]->Gold < object.Cost)
        return false;
    return true;
}

bool HudManager::CheckRequirements(int Owner, cAssets::BuildingType object) {
    auto& engine = Game_Engine::Current();
    for (int i = 0; i < object.Requirements.size(); i++)
        if (std::find(engine.leaders->LeaderList[Owner]->ResearchUpgrades.begin(),
            engine.leaders->LeaderList[Owner]->ResearchUpgrades.end(), object.Requirements[i]) == engine.leaders->LeaderList[Owner]->ResearchUpgrades.end())
            return false;
    if (engine.leaders->LeaderList[Owner]->Gold < object.Cost)
        return false;
    return true;
}

bool HudManager::CheckRequirements(int Owner, cAssets::ResearchType object , std::string name) {
    auto& engine = Game_Engine::Current();

    for (int i = 0; i < object.Requirements.size(); i++)
        if (std::find(engine.leaders->LeaderList[Owner]->ResearchUpgrades.begin(),
            engine.leaders->LeaderList[Owner]->ResearchUpgrades.end(), object.Requirements[i]) == engine.leaders->LeaderList[Owner]->ResearchUpgrades.end())
            return false;

    int cost = engine.leaders->LeaderList[Owner]->CheckCost(name);
    if (engine.leaders->LeaderList[Owner]->Gold < engine.researchmanager->Researchables[name]->Cost[cost]) //if you have the gold
        return false;
    return true;
}

void HudManager::ObjectDetails() {

}