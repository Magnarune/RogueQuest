#include "Engine.h"
#include <set>
#include <functional>
#include <string>
std::string StringifyObject(sol::object val) {
    std::set<const void*> tableMap;
    std::function<std::string(sol::object)> Stringify = [&](sol::object value) -> std::string {

        switch(value.get_type()){
            case sol::type::userdata: return "<userdata>";
            case sol::type::boolean: return std::to_string(value.as<bool>());
            case sol::type::string: return value.as<std::string>();
            case sol::type::function: return "<function>";
            case sol::type::number:{
                double val = value.as<double>();
                return fmod(val, 1.0f) == 0.0f ? std::to_string(int64_t(val)) : std::to_string(val);
            }
            case sol::type::table:{
                if(tableMap.count(value.pointer())){
                    return "...";
                }
                tableMap.emplace(value.pointer());
                std::string copy = "{ ";
                sol::table table = value.as<sol::table>();
                for(auto it = table.begin(); it != table.end(); ++it){
                    std::pair<std::string, std::string> fix, fix2;
                    if((*it).first.get_type() == sol::type::table){
                            fix.first = "[";
                            fix.second = "]";
                    }
                    if((*it).second.get_type() == sol::type::string){
                            fix2.first = "\"";
                            fix2.second = "\"";
                    }
                    copy += (it != table.begin() ? ", " : "")
                        + fix.first + Stringify((*it).first) + fix.second + " = "
                        + fix2.first + Stringify((*it).second) + fix2.second;
                }
                copy += "}";

                return copy;
            }
            default: return "nil";
        }
    };

    return Stringify(val); // begin recursion
}
Game_Engine::Game_Engine()
{
    sAppName = "Simple RTS GAME";
}
bool Game_Engine::OnUserCreate()
{
    //debug sprites
    Demo = new olc::Sprite("Sword.png");
    DemoDecal = new olc::Decal(Demo);
    Erzaspr = new olc::Sprite("Erza.png");
    ErzaDcl = new olc::Decal(Erzaspr);
    Flagspr = new olc::Sprite("flag.png");
    FlagDcl = new olc::Decal(Flagspr);
  // Backroundspr = new olc::Sprite("map.png");
    //Backrounddcl = new olc::Decal(Backroundspr);
    //debug sprites
    tv = olc::TileTransformedView({ ScreenWidth(), ScreenHeight() }, { 1,1 });
   // object.vPOS = { ScreenWidth() / 2.0f, ScreenHeight() / 2.0f };
    //Rain
    Rain = new olc::Sprite("rain.png");
    RainDcl = new olc::Decal(Rain);
    for (int i = 0; i < 100; i++)
    {
       olc::vf2d drop = { (float)(rand() % ScreenWidth()), (float)(rand() % ScreenHeight()) };
       RainDrops.push_back({ 1.0f, drop });

    }
    //Rain end
    lua.open_libraries(sol::lib::base);
    lua.open_libraries(sol::lib::math);
    lua.open_libraries(sol::lib::package);
    lua.open_libraries(sol::lib::string);

    lua["cout"] = [](sol::object value){
        std::cout << StringifyObject(value) << "\n";
    };
    sol::load_result script = lua.load_file("\MapData.lua");
    sol::protected_function_result rcode = script(); // run script
    if(!rcode.valid()){ // check if script ran okay
        sol::error e = rcode;
        std::cout << "error: " << e.what() << "\n"; // display error
    } else {
        mapData = rcode;
    }
    olc::vi2d vTL = tv.GetTopLeftTile().max({ 0,0 });
    olc::vi2d vBR = tv.GetBottomRightTile().min({ mapData["width"],mapData["height"] });    
    tileSets = mapData["tilesets"];
   
    for (int i = 0; i < tileSets.size(); i++)
    {
        sol::table tileSet = tileSets[i + 1];  
        vTileSet.push_back(StringifyObject(tileSet["filename"]));
        vFirstGid.push_back(tileSet["firstgid"]);
       // olc::Sprite* gfx = new olc::Sprite;
        olc::Renderable* Rfx = new olc::Renderable;
        Rfx->Load(vTileSet[i]);
       // olc::Decal* Dfx = new olc::Decal(gfx);
        renderGraphics.emplace_back(Rfx);
    }
    /*
    for(int i=0; i < tileSets.size(); ++i){ //Load all the tile sprites
        sol::table tileSet = tileSets[i+1];        
        olc::Sprite* gfx = new olc::Sprite;
        if (gfx->LoadFromFile(StringifyObject(tileSet["filename"])) != olc::OK) {
            std::cout << "Error loading: " << StringifyObject(tileSet["filename"]) << "\n";
            ; // no memory leak this time
            continue;
        }
        renderGraphics.emplace_back(gfx);
    } 
    */
    
    vTileSize = { mapData["tilewidth"], mapData["tileheight"] };//32x32, 16x16, 64x64 etc
    mapLayers = mapData["layers"];//All the layers and their data
    vLayerData.resize(mapLayers.size());
    for (int i = 0; i < mapLayers.size(); i++)
    {
        layer = mapLayers[i + 1];
        layerSize = { layer["width"], layer["height"] };
        vLayerData[i].resize(layerSize.x * layerSize.y);
        data = layer["data"];
        
        for (int j = 0; j < layerSize.x * layerSize.y; j++)
        {
            if (j >= layerSize.x * layerSize.y) break;
            int l = layer["data"][j + 1];
            vLayerData[i][j] = l; //layer["data"][j + 1];
        }
    }
    
    SetPixelMode(olc::Pixel::ALPHA);
    /*
    CreateLayer(); // console layer
    EnableLayer(1, true);
    SetDrawTarget(nullptr);
    SetPixelMode(olc::Pixel::ALPHA);
    Clear(olc::BLANK);
    tv.GetTopLeftTile().max({ 0,0 });
    tv.GetBottomRightTile().min({ 32,30 });
    for(int i = 0; i < mapLayers.size(); ++i){ //maplayers.size tells you the amount of layers .size()
        sol::table layer = mapLayers[i+1];//lua starts at 1 and look at first layer loop draw then draw second layer
        sol::table data = layer["data"];//RAW Layer Data [ number 11,22, 7, 1, ect...]
        olc::vi2d offset = {layer["offsetx"], layer["offsety"]};//The X and Y offset in the sprite Sheet Usally 0;
        olc::vi2d layerSize = {layer["width"], layer["height"]};//How Wide and Tall the layer is     
        for(int j=0; j < data.size(); ++j){//total size of layer Data
            if(j >= layerSize.x * layerSize.y) break;
            int tile = data[j+1];
            if(tile == 0) continue;
            olc::vi2d pos {j % layerSize.x, j / layerSize.x}; // truncate tile pos so it can be passed in 1D
           tv.DrawPartialSprite(offset + pos * vTileSize, GetTileSet(tile).gfx, GetTile(tile) * vTileSize, vTileSize);           
        }
    }
 */
    ConsoleCaptureStdOut(true);
    return true;
}
bool Game_Engine::OnUserUpdate(float fElapsedTime)
{    
    if (GetKey(olc::Key::F1).bPressed)
    {
        ConsoleShow(olc::Key::ESCAPE);
    }
    switch (m_nGameMode) 
    {
        case MODE_LOCAL_MAP:
            return UpdateLocalMap(fElapsedTime);
    }
    return true;
}
bool Game_Engine::UpdateLocalMap(float fElapsedTime)
{    
    olc::vi2d Topleft = tv.GetWorldTL();
    olc::vi2d BOTRight = tv.GetWorldBR();
    TopleftTile = { (int)(Topleft.x / vTileSize.x),(int)(Topleft.y / vTileSize.y) };   
    BottomeRightTile = {(int)ceil(BOTRight.x / vTileSize.x),(int)ceil(BOTRight.y / vTileSize.y) };
    if (TimeofDay >= 255)
        TimeofDay = 255.0f;
    if (TimeofDay < 50)
        TimeofDay = 50.0f;
    if (TopleftTile.x < 0)
        TopleftTile.x = 0;
    if (TopleftTile.y < 0)
        TopleftTile.y = 0;
    if (BottomeRightTile.x >= 32)
        BottomeRightTile.x = 32;
    if (BottomeRightTile.y >= 30)
        BottomeRightTile.y = 30;  
    Clear(olc::Pixel(100,164,44,255));  
    //tv.DrawDecal({ 0,0 }, Backrounddcl);    
    for (int i = 0; i < mapLayers.size(); i++)
    {        
        for (int y = TopleftTile.y; y <= BottomeRightTile.y; y++){
            for (int x = TopleftTile.x; x <= BottomeRightTile.x; x++){
                int Pos = x  + y * layerSize.x;
                if (Pos >= layerSize.x * layerSize.y) break;
                int tile = vLayerData[i][Pos];
                if (tile == 0) continue;                                  
                pos = {  x,  y };           
               
               
                tv.DrawPartialDecal(offset + pos * vTileSize, GetTileSet(tile).Rfx->Decal(), GetTile(tile) * vTileSize, vTileSize,{1.0,1.0}, olc::Pixel((int)TimeofDay, (int)TimeofDay, (int)TimeofDay,255));
                
            }
        }
    }    
   //tv.DrawStringDecal(object.vPOS + olc::vf2d( { 0.0f, 20.0f }), std::to_string((BottomeRightTile.x - TopleftTile.x + 1) * (BottomeRightTile.y - TopleftTile.y + 1)));
    object.vVel = { 0.0f,0.0f };
    if (GetKey(olc::Key::TAB).bReleased) bFollowObject = !bFollowObject;
    if (GetKey(olc::Key::UP).bHeld) object.vVel +=   {0.0f, -20.0f};
    if (GetKey(olc::Key::DOWN).bHeld) object.vVel += {0.0f, 20.0f};
    if (GetKey(olc::Key::LEFT).bHeld) object.vVel += {-20.0f, 0.0f};
    if (GetKey(olc::Key::RIGHT).bHeld) object.vVel +={20.0f, 0.0f};
    if (object.vVel.mag2() > 0)
        object.vVel = object.vVel.norm() * (GetKey(olc::Key::SHIFT).bHeld ? 80.0f : 40.0f);
    object.vPOS +=  object.vVel * fElapsedTime;
    if (bFollowObject)
    {
        tv.SetWorldOffset(object.vPOS + olc::vf2d( { 8.0f, 8.0f }) - tv.ScaleToWorld(olc::vf2d(ScreenWidth() / 2.0f, ScreenHeight() / 2.0f)));
    }
    //tv.DrawCircle(object.vPOS, object.fRadius);
    static Clock profile;
    profile.restart();
    //tv.DrawDecal(object.vPOS, ErzaDcl, { 0.2f,0.2f}, olc::Pixel((int)TimeofDay, (int)TimeofDay, (int)TimeofDay, 255));
    double time = profile.getMilliseconds();
   // tv.Draw(object.vPOS);
    //if (object.vVel.mag2() > 0)
     //   tv.DrawLine(object.vPOS, object.vPOS + object.vVel.norm() * object.fRadius, olc::BLUE);
    fAngle += 2*fElapsedTime;//rotateSwords    
    if (GetMouseWheel() > 0) tv.ZoomAtScreenPos(2.0f, GetMousePos());
    if (GetMouseWheel() < 0) tv.ZoomAtScreenPos(0.5f, GetMousePos());
    if (GetMouse(2).bPressed) tv.StartPan(GetMousePos());
    if (GetMouse(2).bHeld) tv.UpdatePan(GetMousePos());
    if (GetMouse(2).bReleased) tv.EndPan(GetMousePos());
     if (!IsConsoleShowing())
     {
        if (IsFocused())
        {
            if (GetKey(olc::Key::UP).bHeld || GetKey(olc::Key::W).bHeld)
            {
                // POS.y -= 0.1;
                if (GetKey(olc::Key::UP).bHeld || GetKey(olc::Key::W).bHeld && GetKey(olc::Key::SHIFT).bHeld)
                {
                }
            }
            if (GetKey(olc::Key::DOWN).bHeld || GetKey(olc::Key::S).bHeld)
            {
                // POS.y += 0.1;
                if (GetKey(olc::Key::DOWN).bHeld || GetKey(olc::Key::S).bHeld && GetKey(olc::Key::SHIFT).bHeld)
                {

                }
            }
            if (GetKey(olc::Key::LEFT).bHeld || GetKey(olc::Key::A).bHeld)
            {
                //POS.x -= 0.1;
                if (GetKey(olc::Key::LEFT).bHeld || GetKey(olc::Key::A).bHeld && GetKey(olc::Key::SHIFT).bHeld)
                {

                }
            }
            if (GetKey(olc::Key::RIGHT).bHeld || GetKey(olc::Key::D).bHeld)
            {
                // POS.x += 0.1;
                if (GetKey(olc::Key::RIGHT).bHeld || GetKey(olc::Key::D).bHeld && GetKey(olc::Key::SHIFT).bHeld)
                {

                }
            }
            if (Clicked == false)
            {
                Initial.x = GetMouseX();
                Initial.y = GetMouseY();
            }
            if (GetMouse(0).bHeld)
            {
                Clicked = true;
                olc::vf2d FinalWH = GetMousePos() - Initial;
                 //tv.DrawRect(Initial, FinalWH);
                 //tv.Draw
                 //DrawString(100, 100, "Width= " + std::to_string((int)FinalWH.x) + " H= " + std::to_string((int)FinalWH.y));
                for (auto& ball : Balls)
                {
                    if (ball.POSITION.x > Initial.x && ball.POSITION.y > Initial.y && ball.POSITION.x < (FinalWH.x + Initial.x) && ball.POSITION.y < (FinalWH.y + Initial.y))
                    {
                        ball.Selected = true;
                    }
                }
            }
            else
            {
                for (auto& ball : Balls)
                {
                    //ball.Selected = false;
                }
                Clicked = false;
            }
            if (GetMouse(1).bHeld)
            {
                for (auto& ball : Balls)
                {
                    if (ball.Selected == true)
                    {
                        ball.Targeted = true;
                        ball.Target = GetMousePos() - ball.POSITION;
                        tv.DrawDecal(GetMousePos(), FlagDcl, { 0.2f,0.2f });
                        //ball.VELOCITY = { ball.Target.x ,ball.Target.y};
                    }
                }
            }
            if (GetKey(olc::SPACE).bPressed)
            {
                for (auto& ball : Balls)
                {
                    if (ball.Selected == false)
                    {
                        //ball.Selected = true;
                    }
                    else if (ball.Selected == true)
                    {
                        ball.Selected = false;
                        ball.VELOCITY = { 0.0f,0.0f };
                        //  float fAngle = float(rand()) / float(RAND_MAX) * 2.0f * 3.14159f - 0.3f;
                         // float fSpeed = float(rand()) / float(RAND_MAX) * 100.0f + 20.0f;
                         // ball.VELOCITY = olc::vf2d(fSpeed * cosf(fAngle), fSpeed * sinf(fAngle));
                    }

                }
            }
        }
     }
     if (GetKey(olc::TAB).bReleased)
     {
         bFollowObject = true;
     }
     else
     {
         //bFollowObject = false;
     }
    if (GetKey(olc::L).bReleased)
    {
    }
    /*

    CameraPos = object.vPOS;//Cam Position = Object position
    vVisabletiles = { ScreenWidth() / 32,ScreenHeight() / 32 };
   // fTileOffset =  { CameraPos.x - (float)vVisabletiles.x, CameraPos.y - (float)vVisabletiles.y};
    if (fTileOffset.x < 0) fTileOffset.x = 0;if (fTileOffset.y < 0) fTileOffset.y = 0;
    if (fTileOffset.x > 32 - vVisabletiles.x) fTileOffset.x = 32 - vVisabletiles.x;
    if (fTileOffset.y > 32 - vVisabletiles.y) fTileOffset.y = 32 - vVisabletiles.y;
    */
    /*    
    vVisabletiles = { ScreenWidth() / vTileSize.x,ScreenHeight() / vTileSize.y };
    SpriteOffset = { (POS.x - (float)(vVisabletiles.x) / 2.0f), (POS.y - (float)(vVisabletiles.y) / 2.0f) };
    fTileOffset = { (SpriteOffset.x - (int)SpriteOffset.x) * vTileSize.x, (SpriteOffset.y - (int)SpriteOffset.y) * vTileSize.y };

    for (int x = -1; x < vVisabletiles.x + 1; x++)
    {
        for (int y = -1; y < vVisabletiles.y + 1; y++)
        {
            int idx =(x + SpriteOffset.x, y + SpriteOffset.y); //<-Index on sprite sheet        
            int sx = idx % 10;
            int sy = idx / 10;
            olc::vi2d S = { idx % 10, idx / 10 };
            olc::vi2d Tiles = { x * vTileSize.x - fTileOffset.x, y * vTileSize.y - fTileOffset.y };
            DrawPartialSprite(Tiles, GetTileSet(idx).gfx, S * vTileSize, vTileSize);
        }
    }
    */   
    //SetDrawTarget(1); 
    if (DayTime == true)
    {
        TimeofDay += 15 * fElapsedTime;
        FTIME += fElapsedTime;
        if (FTIME > 30)
        {
            FTIME = 0.0f;
            DayTime = false;
        }
    }
    else
    {
        TimeofDay -= 15 * fElapsedTime;
    }
    if (TimeofDay < 50)
    {
        UpdateRain(fElapsedTime);
        FTIME += fElapsedTime;
        if (FTIME > 20)
        {            
            FTIME = 0;
            DayTime = true;
        }
    }   
   // DrawString(object.vPOS, std::to_string(TopleftTile.x));
    //UpdateAndDraw(fElapsedTime); //<-- Rain
    //SetDrawTarget(2);
    return true;    
};
bool Game_Engine::OnUserDestroy()
{
    return true; // true for successful close
}
void Game_Engine::UpdateRect(olc::vi2d Initial, olc::vi2d Current)
{   
    bool Clicked = true;
    tv.DrawRect(Initial, Current);
}
olc::vi2d Game_Engine::GetTile(int id) {
    const TileSet tileset = GetTileSet(id);
    id -= tileset.gid;
    olc::vi2d tileCount = { tileset.Rfx->Sprite()->width / vTileSize.x,
                           tileset.Rfx->Sprite()->height / vTileSize.y};
    return olc::vi2d { id % tileCount.x, id / tileCount.x };
}
Game_Engine::TileSet Game_Engine::GetTileSet(int id) {
    for(int i=1; i <= tileSets.size(); ++i){
        //sol::table tileset = tileSets[i];
        int gid = vFirstGid[i - 1];     // tileset["firstgid"];
        //if (id > gid && id < vFirstGid[i]) return TileSet{ renderGraphics[i - 1], vFirstGid[i-1]};       
        if(id < gid) return TileSet {renderGraphics[i-2], tileSets[i-1]["firstgid"]};
    }
    return TileSet {renderGraphics.back(), tileSets[tileSets.size()]["firstgid"]};
}