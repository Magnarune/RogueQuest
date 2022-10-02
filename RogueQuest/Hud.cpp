#include "Hud.h"
#include "Engine.h"
Hud::Hud() {
 
}

Hud::~Hud() {   
    decals.clear();
}

void Hud::loadImage(const std::string& name, const std::string& path) {
    std::unique_ptr<olc::Decal> ptr;
    ptr.reset(
        new olc::Decal(
            TextureCache::GetCache().GetTexture(
                TextureCache::GetCache().CreateTexture(path)
            )
        )
    );
    decals.insert_or_assign(name, std::move(ptr));
};

void Hud::loadImage(const std::string& name, size_t tex_id) {
    std::unique_ptr<olc::Decal> ptr;
    ptr.reset(
        new olc::Decal(
            TextureCache::GetCache().GetTexture(tex_id)
        )
    );
    decals.insert_or_assign(name, std::move(ptr));
};

void Hud::ImportHudAssets() {

    loadImage("Hud", "Assets/Gui/Hudbackround.png");
	loadImage("Plus", "Assets/Gui/Plus.png");
	loadImage("Pixel","Assets/Gui/Pixel.png");
	loadImage("Gui",  "Assets/Gui/RPG_GUI.png");
    loadImage("Box", "Assets/Gui/BoxGui.png");
    loadImage("HudBox", "Assets/Gui/Gui_boxboxs.png");
	loadImage("Background", "Assets/Gui/Options_Backround.png");
    loadImage("Icon", "Assets/Gui/Icon_backround.png");
    loadImage("mBox", "Assets/Gui/MiniBox.png");

    loadImage("Attack", "Assets/Gui/PowerUp_04.png");
    loadImage("Move", "Assets/Gui/Move.png");
    loadImage("AtkSpeed", "Assets/Gui/sword_icon.png");

    loadImage("Health", "Assets/Gui/Red.png");
    loadImage("HealthBox","Assets/Gui/EmptyBar.png");
    loadImage("HealthBoxBackground", "Assets/Gui/BackRoundBar.png");

}

void Hud::Update(float delta) {
    //if("Map Has CHANGED")
        //UpdateMiniMap(gfx);


    //DrawMiniMap(gfx);
}
	


void Hud::RefreshMiniMap() {
    auto& engine = Game_Engine::Current();
    // * what in the * //
  //  MiniMap = new olc::Sprite(engine.worldManager->curMap().layerSize.x, engine.worldManager->curMap().layerSize.y);
    //engine.SetDrawTarget(MiniMap);
    for (int x = 0; x < engine.worldManager->curMap().layerSize.x; x++) {
        for (int y = 0; y < engine.worldManager->curMap().layerSize.y; y++) {
            CalculateMiniMap(x, y, engine.worldManager->curMap());
            engine.Draw(x, y, olc::RED);
        }
    }
    engine.SetDrawTarget(nullptr);
}

void Hud::DrawMiniMap() {
    //MiniMap Border
    auto& engine = Game_Engine::Current();//46x46 Rect
    engine.DrawPartialDecal({ 6.9f ,(float)engine.ScreenHeight() * 0.825f -15.1f }, { 54.8f,55.3f }, decals["Gui"].get(), { 872.f,218.f }, { 115.f,97.f });
 /*engine.DrawLineDecal({ 11,(float)engine.ScreenHeight() * 0.823f - 10 }, { 57,(float)engine.ScreenHeight() * 0.823f - 10 });
 engine.DrawLineDecal({ 11,(float)engine.ScreenHeight() * 0.823f - 10 }, { 11,(float)engine.ScreenHeight() * 0.823f + 36 });
 engine.DrawLineDecal({ 11,(float)engine.ScreenHeight() * 0.823f + 36 }, { 57,(float)engine.ScreenHeight() * 0.823f + 36 });
 engine.DrawLineDecal({ 57,(float)engine.ScreenHeight() * 0.823f - 10 }, { 57,(float)engine.ScreenHeight() * 0.823f + 36 });*/
  
    //ZOOMBOX
 /*engine.DrawPartialDecal({ 11.f + 1.4375f * (float)engine.worldManager->curMap().topleftTile.x,(float)engine.ScreenHeight() * 0.823f - 10.0f + float(engine.worldManager->curMap().topleftTile.y) * 1.5333f },
     { 11.f + 1.4375f * (float)engine.worldManager->curMap().topleftTile.x - 11.f + 1.4375f * (float)engine.worldManager->curMap().topleftTile.x ,
     (float)engine.ScreenHeight() * 0.823f - 10.0f + float(engine.worldManager->curMap().bottomRightTile.y) - (float)engine.ScreenHeight() * 0.823f - 10.0f + float(engine.worldManager->curMap().topleftTile.y) * 1.5333f }, decals["mBox"].get(), { 0,0 }, { 104,104 });*/

    float CalculateX = 46.f / engine.worldManager->curMap().layerSize.x;
    float CalculateY = 46.f / engine.worldManager->curMap().layerSize.y;

    engine.DrawLineDecal({ 11.f + CalculateX * (float)engine.worldManager->curMap().topleftTile.x,(float)engine.ScreenHeight() * 0.823f - 10.0f + float(engine.worldManager->curMap().topleftTile.y) * CalculateY },
        { 11.f + CalculateX * (float)engine.worldManager->curMap().topleftTile.x,(float)engine.ScreenHeight() * 0.823f - 10.0f + float(engine.worldManager->curMap().bottomRightTile.y) * CalculateY });
    engine.DrawLineDecal({ 11.f + CalculateX * (float)engine.worldManager->curMap().topleftTile.x,(float)engine.ScreenHeight() * 0.823f - 10.0f + float(engine.worldManager->curMap().topleftTile.y) * CalculateY },
        { 11.f + CalculateX * float(engine.worldManager->curMap().bottomRightTile.x) ,(float)engine.ScreenHeight() * 0.823f - 10.0f + (float)engine.worldManager->curMap().topleftTile.y * CalculateY });

    engine.DrawLineDecal({ 11.f + CalculateX * float(engine.worldManager->curMap().bottomRightTile.x),(float)engine.ScreenHeight() * 0.823f - 10.0f + float(engine.worldManager->curMap().bottomRightTile.y) * CalculateY },
        { 11.f + CalculateX * float(engine.worldManager->curMap().bottomRightTile.x),(float)engine.ScreenHeight() * 0.823f - 10.0f + (float)engine.worldManager->curMap().topleftTile.y * CalculateY });
    engine.DrawLineDecal({ 11.f + CalculateX * float(engine.worldManager->curMap().bottomRightTile.x),(float)engine.ScreenHeight() * 0.823f - 10.0f + float(engine.worldManager->curMap().bottomRightTile.y) * CalculateY },
        { 11.f + CalculateX * (float)engine.worldManager->curMap().topleftTile.x ,    (float)engine.ScreenHeight() * 0.823f - 10.0f + float(engine.worldManager->curMap().bottomRightTile.y) * CalculateY });
                //Camera position in minimap

    // TO DO: This is really bad - even still (was worse)
    //      Fix by pre-rendering to canvas then using that canvas as the minimap
    //   < Don't worry about this until later >

    engine.unitManager->IterateAllUnits([&](auto unit){
        const auto& pos = unit->Position;
        engine.DrawPartialDecal({ 11.f + CalculateX * pos.x / 32.f ,((float)engine.ScreenHeight() * 0.823f - 10.0f + pos.y / 32.f * CalculateY)}, { CalculateX ,CalculateY },
                                    decals["Pixel"].get(), {0.f,0.f}, {1.f,1.f}, olc::GREY);
        return true;
    });

    // engine.DrawPartialDecal({ 11.f + 1.4375f * engine.Camera.vPOS.x / 32.f ,((float)engine.ScreenHeight() * 0.823f - 10.0f + engine.Camera.vPOS.y / 32.f * 1.5333f) }, { 1.4375f,1.5333f }, decals["Pixel"].get(), { 0.f,0.f }, { 1.f,1.f }, olc::RED);

}

void Hud::DrawHud(){
    auto& engine = Game_Engine::Current();
    const auto& ucount = engine.unitManager->GetSelectedUnitCount();
    const auto& bcount = engine.buildingManager->GetSelectedBuildingCount();
    //Draw Bottom of screen    
    engine.DrawPartialDecal({ 1.f,(float)engine.ScreenHeight() * 0.75f }, { (float)engine.ScreenWidth(),(float)engine.ScreenHeight() * 0.75f }, decals["Hud"].get(), { 0,96 }, { 96,96 });
    engine.DrawDecal({ 152.f,177.f }, decals["HudBox"].get(),{0.66f,0.63f});
    engine.DrawPartialDecal({ 59.f,(float)engine.ScreenHeight() * 0.75f }, { 96,60.f }, decals["Hud"].get(), { 224,223 }, { 96,97 });
   // engine.DrawPartialDecal({ 185.5f,185.5f }, { 16,16 }, decals["Gui"].get(), { 775,168 }, { 32,32 });

    //Options Icon top left
    if (engine.GetMousePos().x < 79.5 && engine.GetMousePos().y < 13) {
        engine.DrawPartialDecal({ 1.f,0.f }, { 79.5,13 }, decals["Gui"].get(), { 15,206 }, { 300,52 });
        if (engine.GetMouse(0).bPressed)
            engine.m_nGameMode = engine.MODE_OPTIONS_MENU;
    } else {
        engine.DrawPartialDecal({ 1.f,0.f }, { 79.5,13 }, decals["Gui"].get(), { 15,128 }, { 300,52 });
    }
    engine.DrawStringDecal({ 13.f,4.f }, "Options Menu", olc::WHITE, { 0.5,0.5 });
    engine.DrawPartialDecal({ 0.f,0.f }, { 5.f,5.f }, decals["Gui"].get(), engine.GetLocked() ? olc::vf2d(21.f,53.f) : olc::vf2d(21.f,17.f), { 17.f, 17.f });
    //

    
    if (ucount == 1) {//If you select one unit
        engine.hudManager->UnitSelected();
    }
   if (bcount == 1) {
       engine.hudManager->BuildingSelected();
    }

    //Draws Box to fit all units
    if(ucount > 1){
        
        engine.DrawStringDecal({ 63.f,(float)engine.ScreenHeight() * 0.825f - 6 }, "Units: " + std::to_string(ucount), olc::WHITE, { 0.4f,0.4f });

        olc::vf2d sz = ucount <= 10 ? olc::vf2d(96,16) : (ucount <= 20 ? olc::vf2d(96,24) : olc::vf2d(96,32));

        engine.DrawPartialDecal({ 58.f,(float)engine.ScreenHeight() * 0.825f - 3 }, sz, decals["Hud"].get(), { 0,64 }, { 96,32 });
    }

    std::map<std::string, int> _types;
    engine.unitManager->IterateSelectedUnits([&](std::shared_ptr<Unit> unit){
        if(!_types.count(unit->sUnitName)) _types.insert_or_assign(unit->sUnitName, 0);
        ++_types[unit->sUnitName];
        return true;
    });
    
    ////Draw The number of units on hud
    if (ucount > 1) {
        int i = 0;
        const size_t max_units = 30;
        for (const auto& [name, count] : _types) {
            const auto& data = engine.assetManager->GetUnitData(name);
            
            
            std::string _refname = name + "__tex";
            if (!decals.count(_refname)) {
                loadImage(_refname, data.head.tex_id);
            }
            olc::Decal* decal = decals[_refname].get();

            for (int r = 0; r < count; ++r) {
                float yoffset = i < 10 ? 0 : (i < 20 ? 8.f : 16.f);
                engine.DrawPartialDecal({ 63.f + 8 * (i % 10) ,(float)engine.ScreenHeight() * 0.825f + yoffset }, { 8.f,8.f }, decals["Icon"].get(), { 0.f,0.f }, { 16,16 });
                engine.DrawPartialDecal({ 63.f + 8 * (i % 10),(float)engine.ScreenHeight() * 0.825f + yoffset }, { 8.f,8.f }, decal, data.head.tl, data.head.sz);
                engine.DrawPartialDecal({ 63.f + 8 * (i % 10),(float)engine.ScreenHeight() * 0.825f + yoffset }, { 8.f,8.f }, decals["Gui"].get(), { 872.f,218.f }, { 115.f,97.f });
                if (++i == max_units) break;
            }
            if(ucount > max_units){
                engine.DrawDecal({ 63.f + 80,(float)engine.ScreenHeight() * 0.825f + 18.f }, decals["Plus"].get(), { 0.025f,0.025f });
            }
        }
    }
   /* int y = 0;
    for (int i = 0; i < engine.worldManager->curMap().layerSize.x * engine.worldManager->curMap().layerSize.y; i++) {
        if (i > 0 && i % engine.worldManager->curMap().layerSize.x == 0) {
            y++;
        }
        float X = 11.f + i % engine.worldManager->curMap().layerSize.x;
        
        engine.DrawPartialDecal({ 11.f + 1.4375f * (i % engine.worldManager->curMap().layerSize.x),((float)engine.ScreenHeight() * 0.823f - 10.0f + y * 1.5333f) }, { 1.4375f,1.5333f }, decals["Pixel"].get(), { 0.f,0.f }, { 1.f,1.f }, olc::GREEN);
    }*/

}

void Hud::CalculateMiniMap(int x, int y, const Map &cur_map) {
    auto& engine = Game_Engine::Current();
    

}