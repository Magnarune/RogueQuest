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

Game_Engine* Game_Engine::self = nullptr;

Game_Engine::Game_Engine()
{
    sAppName = "Simple RTS GAME";
    self = this;
}

Game_Engine::~Game_Engine()
{
    self = nullptr;
}

bool Game_Engine::OnUserCreate()
{
    /*
    //debug sprites
    Demo = new olc::Sprite("Sword.png");
    DemoDecal = new olc::Decal(Demo);
    Erzaspr = new olc::Sprite("Erza.png");
    ErzaDcl = new olc::Decal(Erzaspr);
    Flagspr = new olc::Sprite("flag.png");
    FlagDcl = new olc::Decal(Flagspr);
    */

    // Setup Controllers    
    TextureCache::InitCache(); // initialize texture cache
    unitManager.reset(new UnitManager); // create the unit manager   
    assetManager.reset(new cAssets); // create the asset manager
    // Configure Controllers
    assetManager->LoadUnitAssets(); //Load all the Lua files
    //debug sprites
    tv = olc::TileTransformedView({ ScreenWidth(), ScreenHeight() }, { 1,1 });
    //object.vPOS = { ScreenWidth() / 2.0f, ScreenHeight() / 2.0f };
    
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
    sol::load_result script = lua.load_file("MapData.lua");
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
        olc::Renderable* Rfx = new olc::Renderable;
        Rfx->Load(vTileSet[i]);       
        renderGraphics.emplace_back(Rfx);
    }

    vTileSize = { mapData["tilewidth"], mapData["tileheight"] };
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
    ConsoleCaptureStdOut(true);
    Clear(olc::Pixel(100,164,44,255)); // once needed
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
    StandardTime += fElapsedTime;
    if (StandardTime >= 1.0f / 120.0f) {
        

        unitManager->Update(StandardTime); // update all the units - this is because this engine space is too messy to implement everything here

        olc::vi2d Topleft = tv.GetWorldTL();
        olc::vi2d BOTRight = tv.GetWorldBR();
        TopleftTile = { (int)(Topleft.x / vTileSize.x),(int)(Topleft.y / vTileSize.y) };
        BottomeRightTile = { (int)ceil(BOTRight.x / vTileSize.x),(int)ceil(BOTRight.y / vTileSize.y) };
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
        TimeofDay = 255.0f;//Turn off D/N cycle
        //                    ---[CAMERA]--- 
        object.vVel = { 0.0f,0.0f };
        //if (GetKey(olc::Key::TAB).bReleased) followUnit = followUnit.expired()         
        if (followUnit.expired()) {
            if (GetKey(olc::Key::UP).bHeld) object.vVel += {0.0f, -20.f};
            if (GetKey(olc::Key::DOWN).bHeld) object.vVel += {0.0f, 20.0f};
            if (GetKey(olc::Key::LEFT).bHeld) object.vVel += {-20.0f, 0.0f};
            if (GetKey(olc::Key::RIGHT).bHeld) object.vVel += {20.0f, 0.0f};
            if (object.vVel.mag2() > 0)
                object.vVel = object.vVel.norm() * (GetKey(olc::Key::SHIFT).bHeld ? 80.0f : 40.0f);
            object.vPOS += object.vVel * StandardTime;
        }
        else {
            auto unit = followUnit.lock();
            unit->vUnitVelocity += olc::vf2d(unit->fMoveSpeed * GetKey(olc::Key::RIGHT).bHeld - unit->fMoveSpeed * GetKey(olc::Key::LEFT).bHeld,
                                   unit->fMoveSpeed * GetKey(olc::Key::DOWN).bHeld - unit->fMoveSpeed * GetKey(olc::Key::UP).bHeld) * StandardTime;
            object.vPOS = unit->vUnitPosition;
        }        
        StandardTime = 0;
    }
        if (GetMouseWheel() > 0) tv.ZoomAtScreenPos(2.0f, GetMousePos());
        if (GetMouseWheel() < 0) tv.ZoomAtScreenPos(0.5f, GetMousePos());
        if (GetMouse(2).bPressed) tv.StartPan(GetMousePos());
        if (GetMouse(2).bHeld) tv.UpdatePan(GetMousePos());
        if (GetMouse(2).bReleased) tv.EndPan(GetMousePos());
        if (!IsConsoleShowing())
        {
            if (IsFocused())
            {
                if (Clicked == false)
                {
                    Initial.x = GetMouseX();
                    Initial.y = GetMouseY();
                }
                if (GetMouse(0).bHeld)
                {
                    Clicked = true;
                    olc::vf2d FinalWH = GetMousePos() - Initial;

                }
                if (GetMouse(1).bHeld)
                {

                }
            }
        }
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
        tv.SetWorldOffset(object.vPOS - tv.ScaleToWorld(olc::vf2d(ScreenWidth() / 2.0f, ScreenHeight() / 2.0f)));
        // Drawing
        for (int i = 0; i < mapLayers.size(); i++)
        {
            for (int y = TopleftTile.y; y <= BottomeRightTile.y; y++) {
                for (int x = TopleftTile.x; x <= BottomeRightTile.x; x++) {
                    int Pos = x + y * layerSize.x;
                    if (Pos >= layerSize.x * layerSize.y) break;
                    int tile = vLayerData[i][Pos];
                    if (tile == 0) continue;
                    pos = { x,  y };
                    tv.DrawPartialDecal(offset + pos * vTileSize, GetTileSet(tile).Rfx->Decal(), GetTile(tile) * vTileSize, vTileSize, { 1.0,1.0 }, olc::Pixel((int)TimeofDay, (int)TimeofDay, (int)TimeofDay, 255));
                }
            }
        }
        unitManager->Draw(); // draw units  
    return true;    
};
bool Game_Engine::OnUserDestroy()
{
    unitManager.reset(); // free the unit manager
    assetManager.reset(); // free the asset manager
    TextureCache::FreeCache(); // free the texture cache database
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
        int gid = vFirstGid[i - 1];              
        if(id < gid) return TileSet {renderGraphics[i-2], tileSets[i-1]["firstgid"]};
    }
    return TileSet {renderGraphics.back(), tileSets[tileSets.size()]["firstgid"]};
}
bool Game_Engine::OnConsoleCommand(const std::string& stext)
{
    std::stringstream ss;
    ss << stext;
    std::string c1;
    ss >> c1;
    if (c1 == "add")
    {
        std::string name;
        float x {}, y {};
        ss >> name;
        if(!ss.eof()) ss >> x;
        if(!ss.eof()) ss >> y;
        for (int i = 0; i < 100; i++)
        {
            auto unit = unitManager->GenerateUnit(name, { x,y });
            if (unit) {
                std::cout << name << " created at " << x << ", " << y << "\n";
            }
        }
    }
    if(c1 == "count"){
        std::string name;
        ss >> name;
        std::cout << "Found " << unitManager->GetUnitCount(name) << " " << name << " units\n";
    }
    if(c1 == "follow"){
        std::string name;
        size_t n = 0;
        if(!ss.eof()) ss >> name;
        if(!ss.eof()) ss >> n;
        auto unit = unitManager->GetUnit(name, n);
        if(unit){
            std::cout << "Now following " << name << " #" << n << "\n";
            followUnit = unit;
        } else {
            std::cout << "Stopped following unit\n";
            followUnit.reset();
        }
    }
    return true;
}
void Game_Engine::UpdateRain(float FelapsedTime)
{
    for (auto& Raindrops : RainDrops)
    {
        //Raindrops.PositionX += 1.0;
        //Raindrops.PositionY += 1.0;
        Raindrops.Position += {1.0, 1.0};
        //olc::vi2d Screen = tv.GetVisibleTiles();
        if (Raindrops.Position.x >tv.GetWorldBR().x)
        {
            Raindrops.Position.x = tv.GetWorldTL().x;
        }
        if (Raindrops.Position.y > tv.GetWorldBR().y)
        {
            Raindrops.Position.y = tv.GetWorldTL().y;
        }
        tv.DrawRotatedDecal( Raindrops.Position, RainDcl, 7 * 3.14159f / 4, {0,0}, { 0.01f, 0.01f },olc::Pixel((int)TimeofDay, (int)TimeofDay, (int)TimeofDay, 255));
    }
}