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
    loadImage("MiniMap", "Assets/Gui/Minimap.png");

    loadImage("flag","Assets/Gui/flag.png");

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
    if(engine.worldManager->currentMap->name == "Terra")
        engine.DrawPartialDecal({ 6.9f ,(float)engine.ScreenHeight() * 0.825f - 15.1f }, { 54.8f,55.3f }, decals["MiniMap"].get(), { 0.f,0.f }, { 497.f,497.f });
    engine.DrawPartialDecal({ 6.9f ,(float)engine.ScreenHeight() * 0.825f -15.1f }, { 54.8f,55.3f }, decals["Gui"].get(), { 872.f,218.f }, { 115.f,97.f });
  
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

    engine.DrawPartialDecal({ 1.f,(float)engine.ScreenHeight() * 0.75f }, { (float)engine.ScreenWidth(),(float)engine.ScreenHeight() * 0.75f }, decals["Hud"].get(), { 0,96 }, { 96,96 });
    engine.DrawDecal({ 152.f,177.f }, decals["HudBox"].get(),{0.66f,0.63f});
    engine.DrawPartialDecal({ 59.f,(float)engine.ScreenHeight() * 0.75f }, { 96,60.f }, decals["Hud"].get(), { 224,223 }, { 96,97 });


    //Options Icon top left
    if (engine.GetMousePos().x < 55.5 && engine.GetMousePos().y < 10) {
        engine.DrawPartialDecal({ 1.f,0.f }, { 55.5,10 }, decals["Gui"].get(), { 15,206 }, { 300,52 });
        if (engine.GetMouse(0).bPressed)
            engine.m_nGameMode = engine.MODE_OPTIONS_MENU;
    } else {
        engine.DrawPartialDecal({ 1.f,0.f }, { 55.5,10 }, decals["Gui"].get(), { 15,128 }, { 300,52 });
    }
    engine.DrawStringDecal({ 9.f,3.f }, "Options", olc::WHITE, { 0.5,0.5 });
    //

    
    if (ucount == 1) //If you select one unit
        engine.hudManager->UnitSelected();
    if (ucount > 1) 
        engine.hudManager->UnitsSelected(ucount);
   if (bcount == 1)
       engine.hudManager->BuildingSelected();
   if (bcount > 1)
       engine.hudManager->BuildingsSelected(bcount);
   
    if (BuildMode)
        DrawBuild();
}

void Hud::DrawLeaderHud() {
    auto& engine = Game_Engine::Current();
    
    float SWitdh = (float)engine.ScreenWidth();

    
    engine.DrawPartialDecal({ 0.f,0.f }, olc::vf2d({ SWitdh, 10.f }), decals["Background"].get(), { 0.f,0.f }, { 512.f,40.f });

    engine.DrawPartialDecal(olc::vf2d(SWitdh / 1.5f + 2.f, 1.f), olc::vf2d(13.f, 9.f), decals["mBox"].get(), {0.f,0.f}, {104.f,104.f});
    engine.DrawPartialDecal(olc::vf2d(SWitdh / 1.5f + 22.f, 1.f), olc::vf2d(13.f, 9.f), decals["mBox"].get(), {0.f,0.f}, {104.f,104.f});
    engine.DrawPartialDecal(olc::vf2d(SWitdh / 1.5f + 37.f, 1.f), olc::vf2d(13.f, 9.f), decals["mBox"].get(), {0.f,0.f}, {104.f,104.f});

    engine.DrawPartialDecal(olc::vf2d(SWitdh / 1.5f + 2.f, 0.5f), olc::vf2d(13.f, 10.f), decals["HealthBoxBackground"].get(), {0.f,0.f}, {128.f,32.f});
    engine.DrawPartialDecal(olc::vf2d(SWitdh / 1.5f + 22.f, 0.5f), olc::vf2d(13.f, 10.f), decals["HealthBoxBackground"].get(), {0.f,0.f}, {128.f,32.f});
    engine.DrawPartialDecal(olc::vf2d(SWitdh / 1.5f + 37.f, 0.5f), olc::vf2d(13.f, 10.f), decals["HealthBoxBackground"].get(), {0.f,0.f}, {128.f,32.f});

    engine.DrawStringDecal(olc::vf2d(SWitdh / 1.5f + 10.f, 4.f), std::to_string(engine.leaders->LeaderList[1]->Gold), olc::YELLOW, {0.4f,0.4f});
    engine.DrawStringDecal(olc::vf2d(SWitdh / 1.5f + 28.f, 4.f), std::to_string(engine.leaders->LeaderList[1]->Lumber), olc::GREEN, {0.4f,0.4f});
    engine.DrawStringDecal(olc::vf2d(SWitdh / 1.5f + 44.f, 4.f), std::to_string(engine.leaders->LeaderList[1]->Food ),olc::CYAN, {0.4f,0.4f});


}

void Hud::CalculateMiniMap(int x, int y, const Map &cur_map) {
    auto& engine = Game_Engine::Current();
    /* int y = 0;
 for (int i = 0; i < engine.worldManager->curMap().layerSize.x * engine.worldManager->curMap().layerSize.y; i++) {
     if (i > 0 && i % engine.worldManager->curMap().layerSize.x == 0) {
         y++;
     }
     float X = 11.f + i % engine.worldManager->curMap().layerSize.x;

     engine.DrawPartialDecal({ 11.f + 1.4375f * (i % engine.worldManager->curMap().layerSize.x),((float)engine.ScreenHeight() * 0.823f - 10.0f + y * 1.5333f) }, { 1.4375f,1.5333f }, decals["Pixel"].get(), { 0.f,0.f }, { 1.f,1.f }, olc::GREEN);
 }*/
}

void Hud::DrawBuild() {
    auto& engine = Game_Engine::Current();

    const auto& data = engine.assetManager->GetBuildingData(Object);
    std::string _refname = Object + "__tex";
    if (!engine.hud->decals.count(_refname)) {
        engine.hud->loadImage(_refname, data.icon.tex_id);
    }
    olc::Decal* decal = engine.hud->decals[_refname].get();
    olc::vf2d Center = olc::vf2d(engine.GetMousePos()) - olc::vf2d((float)data.icon.sz.x , (float)data.icon.sz.y );
    engine.DrawPartialDecal( ((Center)), {64,64}, decal, {0,0}, data.icon.fsz, olc::PixelF(0.f, 255.f, 0.f, 0.7f));
}