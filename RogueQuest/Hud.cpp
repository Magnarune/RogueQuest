#include "Hud.h"
#include "Engine.h"

Hud::Hud() {
}


Hud::~Hud() {   
    potBuilding.reset();
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
    //This should be it's own Import but I dont feel it's smart to clutter this with yet another importer
    // Can be moved later See Creator
    //---------------Map Day Night Cycle -----//
    loadImage("day1",  "Assets/Gui/DayNight/Visible #139.png");
    loadImage("day2",  "Assets/Gui/DayNight/Visible #140.png");
    loadImage("day3",  "Assets/Gui/DayNight/Visible #141.png");
    loadImage("day4",  "Assets/Gui/DayNight/Visible #142.png");
    loadImage("day5",  "Assets/Gui/DayNight/Visible #143.png");
    loadImage("day6",  "Assets/Gui/DayNight/Visible #144.png");
    loadImage("day7",  "Assets/Gui/DayNight/Visible #145.png");
    loadImage("day8",  "Assets/Gui/DayNight/Visible #146.png");
    loadImage("day9",  "Assets/Gui/DayNight/Visible #147.png");
    loadImage("day10", "Assets/Gui/DayNight/Visible #148.png");
    loadImage("day11", "Assets/Gui/DayNight/Visible #149.png");
    loadImage("day12", "Assets/Gui/DayNight/Visible #150.png");
    loadImage("day13", "Assets/Gui/DayNight/Visible #151.png");
    loadImage("day14", "Assets/Gui/DayNight/Visible #128.png");
    loadImage("day15", "Assets/Gui/DayNight/Visible #129.png");
    loadImage("day16", "Assets/Gui/DayNight/Visible #130.png");
    loadImage("day17", "Assets/Gui/DayNight/Visible #131.png");
    loadImage("day18", "Assets/Gui/DayNight/Visible #132.png");
    loadImage("day19", "Assets/Gui/DayNight/Visible #133.png");
    loadImage("day20", "Assets/Gui/DayNight/Visible #134.png");
    loadImage("day21", "Assets/Gui/DayNight/Visible #135.png");
    loadImage("day22", "Assets/Gui/DayNight/Visible #136.png");
    loadImage("day23", "Assets/Gui/DayNight/Visible #137.png");
    loadImage("day24", "Assets/Gui/DayNight/Visible #138.png");

    //T------------------------------------T
    loadImage("Hud", "Assets/Gui/Hudbackround.png");
	loadImage("Plus", "Assets/Gui/Plus.png");
	loadImage("Pixel","Assets/Gui/Pixel.png");
	loadImage("Gui",  "Assets/Gui/RPG_GUI.png");
    loadImage("Box", "Assets/Gui/BoxGui.png");
    loadImage("HudBox", "Assets/Gui/Gui_boxboxs.png");
	loadImage("Background", "Assets/Gui/Options_Backround.png");
    loadImage("Icon", "Assets/Gui/Icon_backround.png");
    loadImage("mBox", "Assets/Gui/MiniBox.png");

    loadImage("Attack", "Assets/Gui/PowerUp_04.png");//Unit values
    loadImage("Move", "Assets/Gui/Move.png");//Unit values
    loadImage("AtkSpeed", "Assets/Gui/sword_icon.png");//Unit values

    loadImage("Health", "Assets/Gui/Red.png");
    loadImage("HealthBox","Assets/Gui/EmptyBar.png");
    loadImage("HealthBoxBackground", "Assets/Gui/BackRoundBar.png");//

    loadImage("flag","Assets/Gui/flag.png");//flag were units go to

    //LeaderHud
    loadImage("LeaderInfo", "Assets/Gui/LeaderIcons/ParchmentBackground.png");//Leader Info Backround
    loadImage("TopBorder", "Assets/Gui/TopBorder.png"); //top Border
    loadImage("Food", "Assets/Gui/Food.png");//food icon
    loadImage("Tree", "Assets/Gui/Tree.png");//tree icon
    loadImage("Gold", "Assets/Gui/Goldicon.png");//gold icon

    //leader "heads"
    loadImage("Gav", "Assets/Gui/LeaderIcons/Gavin.png");
    loadImage("Hunt", "Assets/Gui/LeaderIcons/Hunter.png");
    loadImage("Jack", "Assets/Gui/LeaderIcons/Jackson.png");
    loadImage("Lando", "Assets/Gui/LeaderIcons/Lando.png");
    loadImage("Luke", "Assets/Gui/LeaderIcons/Luke.png");
    
    loadImage("Units",    "Assets/Research/Icons/Make_Units_Icon.png");//Icons for buildings
    loadImage("Research", "Assets/Research/Icons/Research_Icon.png"); //Icon for Research
    loadImage("Back", "Assets/Research/Icons/No.png"); //Back Icon for Objects

    //hover unit info box
    loadImage("Sigbox", "Assets/Gui/Borderbox.png");

}

void Hud::Update(float delta) {
    if(minimapRefreshTimer.getSeconds() > 1.0) {
        RefreshMiniMap();
        minimapRefreshTimer.restart();
    }
}

void Hud::PreRenderMiniMap(const Map &cur_map) {
    auto& engine = Game_Engine::Current();
    auto& curMap = engine.worldManager->curMap();

    minimapCanvas.reset(new olc::Sprite(curMap.layerSize.x,curMap.layerSize.y));

    minimapTileCanvas.reset(new olc::Sprite(minimapCanvas->width, minimapCanvas->height));
    minimapDecal.reset(new olc::Decal(minimapCanvas.get()));
    
    engine.SetPixelMode(olc::Pixel::ALPHA);
    engine.SetDrawTarget(minimapTileCanvas.get());
    engine.Clear(olc::BLANK);
    for(int i = 0; i < curMap.mapLayers; i++) {
        for (int x = 0; x < curMap.layerSize.x; x++) {
            for (int y = 0; y < curMap.layerSize.y; y++) {
                float alpha = 1.f - float(i) / float(curMap.mapLayers); // less alpha for higher layers
                
                int tile = curMap.layerData[i][x + y * curMap.layerSize.x]; // get tile
                if(tile == 0) continue;

                olc::vi2d tspc = curMap.GetTile(tile) * curMap.tileSize; // get tileset pixel coord
                const olc::Sprite& tspr = *curMap.GetTileSet(tile).decal->sprite;

                olc::Pixel col = tspr.GetPixel(tspc + curMap.tileSize / 2); // get pixel color

                engine.Draw(x, y, col);
            }
        }
    }
    engine.SetDrawTarget(nullptr);
}

void Hud::RefreshMiniMap() {
    auto& engine = Game_Engine::Current();
    auto& curMap = engine.worldManager->curMap();

    if(!minimapCanvas) return;
    
    // draw background tileset
    (*minimapCanvas) = *minimapTileCanvas->Duplicate(); // refresh tilemap to canvas

    engine.SetDrawTarget(minimapCanvas.get()); // draw units

    // draw units
    engine.unitManager->IterateAllUnits([&](auto unit){
        const auto& pos = unit->Position;
        auto tilepos = olc::vi2d(pos / olc::vf2d(curMap.tileSize));

        engine.FillRect(tilepos - olc::vi2d(5,5), {10,10}, 0xFF2050F2);
        return true;
    });

    // draw camera
    engine.FillRect(curMap.topLeftTile, curMap.bottomRightTile - curMap.topLeftTile, 0x40FFFFFF);
    engine.DrawRect(curMap.topLeftTile, curMap.bottomRightTile - curMap.topLeftTile);

    engine.SetDrawTarget(nullptr);

    minimapDecal->Update();
}

void Hud::DrawMiniMap() {
    //MiniMap Border
    auto& engine = Game_Engine::Current();
    auto& curMap = engine.worldManager->curMap();

    if(!minimapDecal) return;

    minimapSize=olc::vf2d(70.f, 70.f); // draw size
    frameSize=olc::vf2d(5.f,6.f); // minimap frame size
    minimapScale=olc::vf2d(minimapSize / olc::vf2d(float(minimapCanvas->width), float(minimapCanvas->height))); // draw scale based on image size to target size

    minimapPosition=olc::vf2d(frameSize.x, float(engine.ScreenHeight()) - minimapSize.y - frameSize.y); // screen position

    engine.DrawDecal(minimapPosition, minimapDecal.get(), minimapScale);
    engine.DrawPartialDecal(minimapPosition - frameSize - olc::vf2d(2.f,2.8f), minimapSize + olc::vf2d(4.8f, 6.2f) + frameSize * 2.f, decals["Gui"].get(), {872.f,218.f}, {115.f,97.f}, 0xFFFFFFFF);
}

void Hud::DrawHud(){
    auto& engine = Game_Engine::Current();

    const auto& ucount = engine.unitManager->GetSelectedUnitCount();
    const auto& bcount = engine.buildingManager->GetSelectedBuildingCount();
    float X = (float)engine.ScreenWidth() * 0.758f;
    float Y = (float)engine.ScreenHeight() * 0.75f;
    float Xb = (float)engine.ScreenHeight() * 0.58f;
    engine.DrawPartialDecal({ Xb,Y-2.f }, { 115.f,(float)engine.ScreenHeight() * 0.748f }, decals["Hud"].get(), { 0,96 }, { 96,96 });
    float Xa = (float)engine.ScreenWidth() * 0.35f;
    engine.DrawDecal({X,Y+1.f }, decals["HudBox"].get(),{0.66f,0.63f});
    engine.DrawPartialDecal({ Xa,(float)engine.ScreenHeight() * 0.75f }, { 96,60.f }, decals["Hud"].get(), { 224,223 }, { 96,97 });

    //Options Icon top left
    if (engine.GetMousePos().x < 55.5 && engine.GetMousePos().y < 10) {
        engine.DrawPartialDecal({ 1.f,0.f }, { 55.5,10 }, decals["Gui"].get(), { 15,206 }, { 300,52 });
        if (engine.GetMouse(0).bPressed)
            LeaderInfo = !LeaderInfo;
    } else {
        engine.DrawPartialDecal({ 1.f,0.f }, { 55.5,10 }, decals["Gui"].get(), { 15,128 }, { 300,52 });
    }
    engine.DrawCenteredStringDecal({ 26.f,5.f }, "Leaders Info", olc::WHITE, { 0.4f,0.4f });
    
    if (ucount == 1) //If you select one unit
        engine.hudManager->UnitSelected();
    if (ucount > 1) 
        engine.hudManager->UnitsSelected(ucount);
   if (bcount == 1)
       engine.hudManager->BuildingSelected();
   if (bcount > 1)
       engine.hudManager->BuildingsSelected(bcount);
   if (LeaderInfo)
       DrawLeaderInfoHud();
   
    if (BuildMode)
        DrawBuild();
}

void Hud::DrawLeaderInfoHud() {
    auto& engine = Game_Engine::Current();
    olc::vf2d Start(0.f,10.f);
    engine.DrawDecal(Start, decals["LeaderInfo"].get(),{0.4f,0.4f}, 0xEAFFFFFF);
    olc::vf2d Pos =olc::vf2d((float)engine.ScreenWidth() / 10.f, (float)engine.ScreenHeight() / 13.f);
        int a = 0;
    for (int i = 0; i < engine.leaders->LeaderList.size(); i++) {
        a = i;
        int sp = 13;
        olc::vf2d size = { 12.f,12.f };

        engine.DrawPartialDecal(Pos + olc::vf2d(0, sp * a), size, engine.leaders->LeaderList[i]->LeaderHead, { 0,0 }, { 256,256 });
        engine.DrawPartialDecal(Pos + olc::vf2d(sp , sp * a), size, decals["HealthBoxBackground"].get(), { 0.f,4.f }, { 128.f,24.f });
        engine.DrawPartialDecal(Pos + olc::vf2d(sp, sp * a), size, decals["mBox"].get(), { 0.f,0.f }, { 104.f,104.f });
        engine.DrawCenteredStringDecal(Pos + olc::vf2d(18, sp * a + 6), std::to_string(engine.leaders->LeaderList[i]->Gold), olc::YELLOW, olc::vf2d(0.25,0.25));
        engine.DrawPartialDecal(Pos + olc::vf2d(2*sp, sp * a), size, decals["HealthBoxBackground"].get(), { 0.f,4.f }, { 128.f,24.f });
        engine.DrawPartialDecal(Pos + olc::vf2d(2*sp, sp * a), size, decals["mBox"].get(), { 0.f,0.f }, { 104.f,104.f });
        engine.DrawCenteredStringDecal(Pos + olc::vf2d(31, sp * a + 6), std::to_string(engine.leaders->LeaderList[i]->Lumber), olc::GREEN, olc::vf2d(0.25, 0.25));
        engine.DrawPartialDecal(Pos + olc::vf2d(3*sp, sp * a), size, decals["HealthBoxBackground"].get(), { 0.f,4.f }, { 128.f,24.f });
        engine.DrawPartialDecal(Pos + olc::vf2d(3*sp, sp * a), size, decals["mBox"].get(), { 0.f,0.f }, { 104.f,104.f });
        engine.DrawCenteredStringDecal(Pos + olc::vf2d(44, sp * a + 6), std::to_string(engine.leaders->LeaderList[i]->Food), olc::RED, olc::vf2d(0.25, 0.25));
        
        engine.DrawStringDecal(Pos + olc::vf2d(4 * sp, 6 + sp * a), engine.leaders->LeaderList[i]->LeaderName, olc::BLACK, {0.51f,0.51f});
        engine.DrawStringDecal(Pos + olc::vf2d(4 * sp, 6 + sp * a), engine.leaders->LeaderList[i]->LeaderName, engine.highlightmanagment->Color[i], {0.5f,0.5f});
    }
}



struct Item {
    olc::Decal* decIcon;
    olc::vf2d iconSize, borderSize;
    int value;
    olc::Pixel color;
};

void Hud::DrawLeaderHud() {
    auto& engine = Game_Engine::Current();


    { // border
        olc::vf2d topBarSize(float(engine.ScreenWidth()), 8.f);

        engine.DrawPartialDecal({ 0.f,0.f }, topBarSize, decals["Background"].get(), { 0.f,0.f }, { 512.f,40.f });
        engine.DrawPartialDecal({0.f, topBarSize.y - 1.f}, {topBarSize.x, 3.f}, decals["TopBorder"].get(), {0.f,0.f}, {90.f,16.f});
        //DayNight Info
        engine.DrawPartialDecal(olc::vf2d((float)engine.ScreenWidth() / 2.f, 0.f), { 25.f,15.f }, 
            decals["day" + std::to_string(engine.worldManager->currentMap->PhaseofDay)].get(), { 0.f,0.f }, { 112,73 });

     // economy list
        float kerning = 1.5f;
        olc::vf2d listPosition(topBarSize.x - 4.f, 1.f);
        olc::vf2d frameSize(4.f,0.f);

        float ysize = (topBarSize.y - listPosition.y) * 0.94f;
        std::array<Item, 4> itemList {{
            {decals["Food"].get(), {ysize, ysize}, {topBarSize.y * 1.42f, ysize}, engine.leaders->LeaderList[1]->Food, olc::RED },
            {decals["Tree"].get(), {ysize, ysize}, {ysize * 1.42f, ysize}, engine.leaders->LeaderList[1]->Lumber, olc::GREEN },
            {decals["Gold"].get(), {ysize, ysize}, {ysize * 1.42f, ysize}, engine.leaders->LeaderList[1]->Gold, olc::YELLOW },
            {decals["flag"].get(), {ysize, ysize}, {ysize * 1.42f, ysize}, (int)engine.unitManager->GetSelectedUnitCount(), olc::BLUE },
        }};

        olc::Decal* decBorder = decals["mBox"].get();
        olc::Decal* decBackground = decals["HealthBoxBackground"].get();

        int i = 1;
        for(const Item& item : itemList) {
            const std::string& valueStr = std::to_string(item.value);
            olc::vf2d pos = listPosition;
            float itemWidth = (item.iconSize.x + item.borderSize.x + frameSize.x * 2.f);

            pos.x -= i * itemWidth * kerning;

            olc::vf2d valuePos(pos);
            olc::vf2d iconPos(valuePos);
            iconPos.x += frameSize.x + item.borderSize.x + 2.f;

            olc::vf2d borderPos(valuePos - frameSize);
            olc::vf2d textSize = olc::vf2d(engine.GetTextSize(valueStr));
            
            olc::vf2d tscale((item.borderSize + frameSize - olc::vf2d(2.f,2.f)) / textSize);
            float textScale = std::max(std::min(std::min(tscale.x, tscale.y), 0.75f), 0.25f);

            engine.DrawPartialDecal(borderPos, item.borderSize + frameSize * 2.f, decBackground, {0.f,4.f}, {128.f,24.f});
            engine.DrawPartialDecal(borderPos, item.borderSize + frameSize * 2.f, decBorder, {0.f,0.f}, {104.f,104.f});
            engine.DrawPartialDecal(iconPos, item.iconSize, item.decIcon, { 0.f,0.f }, olc::vf2d(float(item.decIcon->sprite->width), float(item.decIcon->sprite->height)));
            engine.DrawCenteredStringDecal(valuePos + item.borderSize / 2.f, valueStr, item.color, olc::vf2d(textScale,textScale));

            ++i;
        }
    }
}

void Hud::DrawBuild() {
    auto& engine = Game_Engine::Current();

    const auto& data = engine.assetManager->GetBuildingData(potBuilding->name);
    engine.hud->potBuilding->Position = engine.tv.ScreenToWorld(engine.GetMousePos()) - olc::vf2d(engine.hud->potBuilding->Size) / 2.f;
    std::string _refname = potBuilding->name + "__tex";
    if (!engine.hud->decals.count(_refname)) {
        engine.hud->loadImage(_refname, data.icon.tex_id);
    }
    olc::Decal* decal = engine.hud->decals[_refname].get();
    olc::vf2d Center = olc::vf2d(engine.tv.ScreenToWorld(engine.GetMousePos())) - olc::vf2d((float)data.icon.sz.x, (float)data.icon.sz.y );
    engine.tv.DrawPartialDecal( ((Center)), olc::vf2d(engine.hud->potBuilding->Size), decal, {0,0}, data.icon.fsz, engine.unitManager->CheckBuildObstruction(potBuilding) ? olc::PixelF(0.f, 255.f, 0.f, 0.7f) : olc::PixelF(1.f, 0.f, 0.f, 0.7f));
}

void Hud::DrawDescription(const std::string& str, olc::vi2d Mouse) {//Your making the Description
    auto& engine = Game_Engine::Current();
    float x = 30;
    int width = engine.GetTextSize(str).x;
    int targetWidth = std::clamp(width, 30, 50);
    
    std::string drawStr = WrapText(str, targetWidth, false, { 0.4f,0.4f });
    int h = engine.GetTextSize(drawStr).y;
    float y = (float)h * 0.4f;
    engine.DrawPartialDecal({ (float)Mouse.x-2-(float)targetWidth ,(float)Mouse.y -4- y }, {(float)targetWidth+8, y + 8}, decals["Icon"].get(), { 0.f,0.f }, { 117.f,99.f },olc::Pixel(255,255,255,180));
    DrawDescriptionBorder(decals["Sigbox"].get(), { (float)Mouse.x - 2 - (float)targetWidth ,(float)Mouse.y - 4 - y }, { (float)targetWidth + 8, y + 8 }, {0.2f,0.2f});
    engine.DrawStringDecal({ (float)Mouse.x + 3 - (float)targetWidth,(float)Mouse.y- y}, drawStr, olc::WHITE, { 0.4f,0.4f });
}

std::string Hud::WrapText(const std::string& str, int width, bool proportional, olc::vd2d scale) {
    auto& engine = Game_Engine::Current();
    std::string newStr, oldStr(str);
    while (true) {
        std::string word;
        if (str.find(" ") == std::string::npos) {
            word = oldStr;
        } else {
            word = oldStr.substr(0, str.find(" "));
        }
        olc::vi2d newSize = olc::vd2d(proportional ? engine.GetTextSizeProp(newStr + (newStr.size() > 0 ? " " : "") + word) :
            engine.GetTextSize(newStr + (newStr.size() > 0 ? " " : "") + word)) * scale;

        if (newSize.x > width) {
            newStr += "\n" + word;
        } else {
            newStr += (newStr.size() > 0 ? " " : "") + word;
        }
        if (oldStr.find(" ") == std::string::npos) {
            break;
        } else {
            oldStr.erase(0, oldStr.find(" ") + 1);
        }
    }
    return newStr;
}

void Hud::DrawDescriptionBorder(olc::Decal* box, const olc::vf2d& pos, const olc::vf2d& size , const olc::vf2d& borderScale){
    auto& engine = Game_Engine::Current();
    olc::vd2d scaledCorner = { 13 * (float)borderScale.x,13 * (float)borderScale.y };
  engine.DrawPartialDecal(pos, scaledCorner, box, { 0,0 }, { 13,13 }); //Upper-Left Corner
    olc::vd2d ULOffset = { 13 * (float)borderScale.x,0 };
    engine.DrawPartialDecal(pos + ULOffset, { (float)size.x - 26 * (float)borderScale.x,13 * (float)borderScale.y }, box, { 13,0 }, { 91,13 }); //Top
    olc::vd2d UROffset = { size.x - 13 * (float)borderScale.x,0 };
    engine.DrawPartialDecal(pos + UROffset, scaledCorner, box, { 104,0 }, { 13,13 }); //Upper-Right Corner
    olc::vd2d ULBottomOffset = { 0,13 * (float)borderScale.y };
    engine.DrawPartialDecal(pos + ULBottomOffset, { 13 * (float)borderScale.x,(float)size.y - 25 * (float)borderScale.y }, box, { 0,13 }, { 13,73 });//Left
    olc::vd2d BLOffset = { 0,size.y - 13 * (float)borderScale.y };
    engine.DrawPartialDecal(pos + BLOffset, scaledCorner, box, { 0,86 }, { 13,13 }); //Bottom-Left Corner
    olc::vd2d BLLeftOffset = { 13 * (float)borderScale.x,size.y - (13 + 0.05) * (float)borderScale.y };
    engine.DrawPartialDecal(pos + BLLeftOffset, { (float)size.x - 26 * (float)borderScale.x,13 * (float)borderScale.y }, box, { 13,86 }, { 91,13 }); //Bottom
    olc::vd2d BROffset = { size.x - 13 * (float)borderScale.x,size.y - 13 * (float)borderScale.y };
    engine.DrawPartialDecal(pos + BROffset, scaledCorner, box, { 104,86 }, { 13,13 }); //Bottom-Right Corner
    olc::vd2d TROffset = { size.x - 13 * (float)borderScale.x,13 * (float)borderScale.y };
    engine.DrawPartialDecal(pos + TROffset, { 13 * (float)borderScale.x,(float)size.y - 25 * (float)borderScale.y }, box, { 104,13 }, { 13,73 });//Right
}