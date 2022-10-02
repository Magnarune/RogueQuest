#include "HudManager.h"
//#include "Engine.h"


HudManager::HudManager() {
    auto& engine = Game_Engine::Current();
    IconY = (float)engine.ScreenHeight() * 0.825f;
}
HudManager::~HudManager() {

}

void HudManager::UnitSelected() {
	auto& engine = Game_Engine::Current();

    std::shared_ptr<Unit> unitinfo = engine.unitManager->GetUnit();
    const auto& data = engine.assetManager->GetUnitData(unitinfo->sUnitName);
    std::string _refname = unitinfo->sUnitName + "__tex";
    if (!engine.hud->decals.count(_refname)) {
        engine.hud->loadImage(_refname, data.head.tex_id);
    }
    float healthMod = unitinfo->fHealth / unitinfo->fMaxHealth;
    olc::Decal* decal = engine.hud->decals[_refname].get();

    engine.DrawPartialDecal({ 90.f, IconY- 8.f }, { 8.f,8.f }, engine.hud->decals["Attack"].get(), { 0,0 }, { 190,190 });
    engine.DrawStringDecal({ 99.f,  IconY- 4.f }, std::to_string((int)unitinfo->fAttackDamage), olc::WHITE, { 0.4f,0.4f });
                                    
    engine.DrawPartialDecal({ 90.f, IconY+ 2.f }, { 8.f,8.f }, engine.hud->decals["Move"].get(), { 0,0 }, { 32,32 });
    engine.DrawStringDecal({ 99.f,  IconY+ 6.f }, std::to_string((int)unitinfo->fMoveSpeed), olc::WHITE, { 0.4f,0.4f });
                                    
    engine.DrawPartialDecal({ 90.f, IconY+ 12.f }, { 8.f,8.f }, engine.hud->decals["AtkSpeed"].get(), { 0,0 }, { 512,512 });
    engine.DrawStringDecal({ 99.f,  IconY+ 16.f }, std::to_string((int)unitinfo->fAttackSpeed), olc::WHITE, { 0.4f,0.4f });
                                    
    engine.DrawStringDecal({ 66.f,  IconY- 6.f }, unitinfo->sUnitName, olc::BLUE, { 0.4f,0.4f });
                                    
    engine.DrawPartialDecal({ 68.f ,IconY }, { 16.f,16.f }, engine.hud->decals["Icon"].get(), { 0.f,0.f }, { 16,16 });
    engine.DrawPartialDecal({ 68.f ,IconY }, { 16.f,16.f }, decal, data.head.tl, data.head.sz);
    engine.DrawPartialDecal({ 66.5f, IconY - 1.5f }, { 18.5f,18.5f }, engine.hud->decals["Gui"].get(), { 872.f,218.f }, { 115.f,97.f });
                                    
    engine.DrawPartialDecal({ 64.f, IconY+ 17.f }, { 22.f,4.f }, engine.hud->decals["HealthBoxBackground"].get(), { 0,0 }, { 128,32 });
    engine.DrawPartialDecal({ 64.f, IconY+ 17.f }, { 22.f * healthMod,4.f }, engine.hud->decals["Health"].get(), { 0,0 }, { 128,32 });
    engine.DrawPartialDecal({ 64.f, IconY+ 17.f }, { 22.f,4.f }, engine.hud->decals["HealthBox"].get(), { 0,0 }, { 128,32 });

    engine.DrawStringDecal({ 64.f, IconY + 22.f }, std::to_string((int)unitinfo->fHealth) + "/" + std::to_string((int)unitinfo->fMaxHealth), olc::RED, { 0.4f,0.4f });
    UnitAbilities(unitinfo);
}

void HudManager::UnitsSelected() {
	auto& engine = Game_Engine::Current();

}
void HudManager::UnitAbilities(std::shared_ptr<Unit> unitinfo) {
    auto& engine = Game_Engine::Current();

    for (int i = 0; i < unitinfo->Constructables.size(); i++) {
        const auto& data = engine.assetManager->GetBuildingData(unitinfo->Constructables[i]);
        std::string _refname = unitinfo->Constructables[i] + "__tex";
        if (!engine.hud->decals.count(_refname)) {
            engine.hud->loadImage(_refname, data.icon.tex_id);
        }
        olc::Decal* decal = engine.hud->decals[_refname].get();
        float yoffset = i < 3 ? 0 : (i < 20 ? 16.f : 32.f);

        engine.DrawPartialDecal({ 186.f + 16.f * (i % 3) , IconY - 12.f + yoffset }, { 16.f,16.f }, engine.hud->decals["Icon"].get(), { 0.f,0.f }, { 16,16 });
        engine.DrawPartialDecal({ 186.f + 16.f * (i % 3) , IconY - 12.f + yoffset }, { 16.f,16.f }, decal, {0.f,0.f}, data.icon.fsz);
        engine.DrawPartialDecal({ 186.f + 16.f * (i % 3) , IconY - 12.f + yoffset }, { 16.5f,16.5f }, engine.hud->decals["Gui"].get(), { 872.f,218.f }, { 115.f,97.f });

        if (Button({ 186.f + 16.f * (i % 3), IconY - 12.f + yoffset }, engine.GetMousePos(), { 16,16 })) {
            unitinfo->UnitBuild(unitinfo->Constructables[i]);
        
        }
    }
   // engine.DrawPartialDecal({ 104.f , IconY - 4.f }, { 16.f,16.f }, engine.hud->decals["Icon"].get(), { 0.f,0.f }, { 16,16 });
    //engine.DrawPartialDecal({ 104.f , IconY - 4.f }, { 16.f,16.f }, decal,{0.f,0.f}, data.icon.fsz);
    //engine.DrawPartialDecal({ 104.f , IconY - 4.f }, { 16.5f,16.5f }, engine.hud->decals["Gui"].get(), { 872.f,218.f }, { 115.f,97.f });


}

void HudManager::BuildingSelected() {
	auto& engine = Game_Engine::Current();

    std::shared_ptr<Building> buildinginfo = engine.buildingManager->GetBuilding();
    const auto& data = engine.assetManager->GetBuildingData(buildinginfo->name);
    std::string _refname = buildinginfo->name + "__tex";
    if (!engine.hud->decals.count(_refname)) {
        engine.hud->loadImage(_refname, data.icon.tex_id);
    }
    //
    float healthMod = buildinginfo->health / buildinginfo->maxHealth;
    float productionMod;
    if(buildinginfo->building)
         productionMod = buildinginfo->m_fTimer / buildinginfo->productiontime;
    
    
    olc::Decal* decal = engine.hud->decals[_refname].get();

    engine.DrawStringDecal({ 66.f,   IconY - 6.f }, buildinginfo->name, olc::BLUE, { 0.4f,0.4f });
    engine.DrawPartialDecal({ 68.f  ,IconY }, { 16.f,16.f }, engine.hud->decals["Icon"].get(), { 0.f,0.f }, { 16,16 });
    engine.DrawPartialDecal({ 68.f  ,IconY }, data.icon.sz / 2.f, decal, { 0.f,0.f }, data.icon.fsz);
    engine.DrawPartialDecal({ 66.5f, IconY - 1.5f }, { 18.5f,18.5f }, engine.hud->decals["Gui"].get(), { 872.f,218.f }, { 115.f,97.f });

   
    engine.DrawPartialDecal({ 64.f, IconY + 17.f }, { 22.f,4.f }, engine.hud->decals["HealthBoxBackground"].get(), { 0,0 }, { 128,32 });
    engine.DrawPartialDecal({ 64.f, IconY + 17.f }, { 22.f * healthMod,4.f }, engine.hud->decals["Health"].get(), { 0,0 }, { 128,32 });
    engine.DrawPartialDecal({ 64.f, IconY + 17.f }, { 22.f,4.f }, engine.hud->decals["HealthBox"].get(), { 0,0 }, { 128,32 });
    if (buildinginfo->building) {
        const auto& data = engine.assetManager->GetUnitData(buildinginfo->unitproduced);
        std::string _refname = buildinginfo->unitproduced + "__tex";
        if (!engine.hud->decals.count(_refname)) {
            engine.hud->loadImage(_refname, data.head.tex_id);
        }
        olc::Decal* decal = engine.hud->decals[_refname].get();
        engine.DrawStringDecal({ 92.f,   IconY - 13.f },"Producing: " + buildinginfo->unitproduced, olc::BLUE, {0.4f,0.4f});
        engine.DrawPartialDecal({ 92.f, IconY - 9.f }, { 44.f,4.f }, engine.hud->decals["HealthBoxBackground"].get(), { 0,0 }, { 128,32 });
        engine.DrawPartialDecal({ 92.f, IconY - 9.f }, { 44.f * productionMod,4.f }, engine.hud->decals["Health"].get(), { 0,0 }, { 128,32 },olc::YELLOW);
        engine.DrawPartialDecal({ 92.f, IconY - 9.f }, { 44.f,4.f }, engine.hud->decals["HealthBox"].get(), { 0,0 }, { 128,32 });

        engine.DrawPartialDecal({ 104.f , IconY - 4.f }, { 16.f,16.f }, engine.hud->decals["Icon"].get(), { 0.f,0.f }, { 16,16 });
        engine.DrawPartialDecal({ 104.f , IconY - 4.f }, { 16.f,16.f }, decal, data.head.tl, data.head.sz);
        engine.DrawPartialDecal({ 104.f , IconY - 4.f }, { 16.5f,16.5f }, engine.hud->decals["Gui"].get(), { 872.f,218.f }, { 115.f,97.f });

    }
    BuildingAbilities(buildinginfo);

    

}
void HudManager::BuildingAbilities(std::shared_ptr<Building> buildinginfo) {
    auto& engine = Game_Engine::Current();
    for (int i = 0; i < buildinginfo->unitproduction.size(); i++) {
        const auto& data = engine.assetManager->GetUnitData(buildinginfo->unitproduction[i]);
        std::string _refname = buildinginfo->unitproduction[i] + "__tex";
        if (!engine.hud->decals.count(_refname)) {
            engine.hud->loadImage(_refname, data.head.tex_id);
        }
        olc::Decal* decal = engine.hud->decals[_refname].get();
        float yoffset = i < 3 ? 0 : (i < 20 ? 16.f : 32.f);
        engine.DrawPartialDecal({ 186.f + 16.f * (i % 3) , IconY - 12.f + yoffset }, { 16.f,16.f }, engine.hud->decals["Icon"].get(), { 0.f,0.f }, { 16,16 });
        engine.DrawPartialDecal({ 186.f + 16.f * (i % 3) , IconY - 12.f + yoffset }, { 16.f,16.f }, decal, data.head.tl, data.head.sz);
        engine.DrawPartialDecal({ 186.f + 16.f * (i % 3) , IconY - 12.f + yoffset }, { 16.5f,16.5f }, engine.hud->decals["Gui"].get(), { 872.f,218.f }, { 115.f,97.f });

        if (Button({ 186.f + 16.f * (i % 3), IconY - 12.f + yoffset }, engine.GetMousePos(), { 16,16 })) {
            buildinginfo->ProduceUnit(buildinginfo->unitproduction[i]);
            //engine.worldManager->GenerateUnit(buildinginfo->unitproduction[i], buildinginfo->pos + olc::vf2d(rand() % 5 + 0.f, 32.f));
        }
    }
}

void HudManager::BuildingsSelected() {

}

bool HudManager::Button(olc::vf2d pos, olc::vi2d Mouse, olc::vf2d Size) {
    auto& engine = Game_Engine::Current();
    if (engine.GetMouse(0).bPressed &&
        Mouse.x > pos.x &&      Mouse.y > pos.y &&
        Mouse.x < pos.x + Size.x &&        Mouse.y < pos.y + Size.y) {
        engine.ActivityDone = true;
        return true;
    }
    else
        return false;

}