#include "Engine.h"
#include <set>
#include <functional>
#include <vector>

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
Game_Engine::Game_Engine() {
    sAppName = "Simple RTS GAME";
    self = this;
}
Game_Engine::~Game_Engine() {
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
    */
   
    Flag->Load("flag.png");

    // Setup Controllers    
    TextureCache::InitCache(); // initialize texture cache
    worldManager.reset(new WorldManager); // create the world manager
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
    

    
    { // To do: Consider putting all this code into an appropriate Lua Map Loading Class
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
    }
    
    SetPixelMode(olc::Pixel::ALPHA);
    ConsoleCaptureStdOut(true);
    Clear(olc::Pixel(100,164,44,255)); // once needed

    return true;
}

bool Game_Engine::OnUserUpdate(float fElapsedTime) {
    fElapsedTime = std::fmin(1.f, fElapsedTime);

    if (GetKey(olc::Key::F1).bPressed) ConsoleShow(olc::Key::ESCAPE);

    switch (m_nGameMode){
        case MODE_LOCAL_MAP:
            return UpdateLocalMap(fElapsedTime);
    }

    return true;
}

bool Game_Engine::UpdateLocalMap(float fElapsedTime) {
    GetUserInput(fElapsedTime);

    if(!IsConsoleShowing()){
        worldManager->Update(fElapsedTime);
        unitManager->Update(); // isn't used for anything atm
    }

    DrawMap(); // draw map background
    worldManager->Draw(); // draw game objects

    UpdateTimeofDay(fElapsedTime);


    worldManager->CollectGarbage(); // clearing garbage will break references
    return true;
};


bool Game_Engine::OnUserDestroy(){
    worldManager.reset(); // free the world manager
    unitManager.reset(); // free the unit manager
    assetManager.reset(); // free the asset manager
    TextureCache::FreeCache(); // free the texture cache database
    return true; // true for successful close
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



typedef std::function<void(std::stringstream&)> CommandFunction;

const std::map<std::string, CommandFunction> commands = {
    {"add", [](std::stringstream& ss) {
            auto& engine = Game_Engine::Current();
            
            std::string name;
            float x {}, y {}, count = 1;
            ss >> name;
            if(!ss.eof()) ss >> x;
            if(!ss.eof()) ss >> y;
            if(!ss.eof()) ss >> count;
            for (int i = 0; i < count; i++){
                auto unit = engine.worldManager->GenerateUnit(name, {x + 3*i, y});
                if (unit) {
                    unit->bFriendly = true;
                    std::cout << name << " created at " << x << ", " << y << "\n";
                }
            }
        }
    },
    {"enemy", [](std::stringstream& ss){
        auto& engine = Game_Engine::Current();

        std::string name;
        size_t n = 0;
        if(!ss.eof()) ss >> name;
        if(!ss.eof()) ss >> n;
        auto unit = engine.unitManager->GetUnit(name, n);
        if(unit) {
            unit->bFriendly = false;
            unit->ULogic = Unit::Attack;
            unit->Hunted.reset();
            std::cout << name << " #" << n << " is now an enemy\n";
        }
    }},
    {"destroy", [](std::stringstream& ss){
        auto& engine = Game_Engine::Current();

        std::string name;
        size_t n = 0;
        if(!ss.eof()) ss >> name;
        if(!ss.eof()) ss >> n;
        auto unit = engine.unitManager->GetUnit(name, n);
        if(unit) {
            unit->Destroy();
            std::cout << name << " #" << n << " destroyed\n";
        }
    }},
    {"destroyall", [](std::stringstream& ss){
        auto& engine = Game_Engine::Current();

        std::string name;
        if(!ss.eof()) ss >> name;
        for(size_t i=0; i < engine.unitManager->GetUnitCount(name); ++i){
            auto unit = engine.unitManager->GetUnit(name, i);
            if(unit) {
                unit->Destroy();
                std::cout << name << " #" << i << " destroyed\n";
            }
        }
    }},
    {"count", [](std::stringstream& ss){
        auto& engine = Game_Engine::Current();

        std::string name;
        ss >> name;
        std::cout << "Found " << engine.unitManager->GetUnitCount(name) << " " << name << " units\n";
    }},
    {"follow", [](std::stringstream& ss){
        auto& engine = Game_Engine::Current();

        std::string name;
        size_t n = 0;
        if(!ss.eof()) ss >> name;
        if(!ss.eof()) ss >> n;
        auto unit = engine.unitManager->GetUnit(name, n);
        if(unit){
            std::cout << "Now following " << name << " #" << n << "\n";
            engine.followUnit = unit;
        } else {
            std::cout << "Stopped following unit\n";
            engine.followUnit.reset();
        }
    }}
    /*
    {"", {[](std::stringstream& ss){
        auto& engine = Game_Engine::Current();

        }
    }},
    */
};

bool Game_Engine::OnConsoleCommand(const std::string& stext){

    std::stringstream ss;
    ss << stext;
    std::string c1;
    ss >> c1;

    if(commands.count(c1)){
        commands.at(c1)(ss);
    } else {
        std::cout << "Command " << c1 << " does not exist\n";
    }

    return true;
}

void Game_Engine::UpdateRain(float FelapsedTime)
{
    for (auto& Raindrops : RainDrops){
        //Raindrops.PositionX += 1.0;
        //Raindrops.PositionY += 1.0;
        //Raindrops.Position += {1.0, 1.0};
        //olc::vi2d Screen = tv.GetVisibleTiles();
        if (Raindrops.Position.x >tv.GetWorldBR().x){
            Raindrops.Position.x = tv.GetWorldTL().x;
        }
        if (Raindrops.Position.y > tv.GetWorldBR().y){
            Raindrops.Position.y = tv.GetWorldTL().y;
        }
        tv.DrawRotatedDecal( Raindrops.Position, RainDcl, 7 * 3.14159f / 4, {0,0}, { 0.01f, 0.01f },olc::Pixel((int)TimeofDay, (int)TimeofDay, (int)TimeofDay, 255));
    }
}

void Game_Engine::UpdateTimeofDay(float felapsedtime){
    TimeofDay = 255;
    if (TimeofDay >= 255)
        TimeofDay = 255.0f;
    if (TimeofDay < 50)
        TimeofDay = 50.0f;
    if (DayTime == true){
        TimeofDay += 20 * felapsedtime;
        FTIME += felapsedtime;
        if (FTIME > 30){
            FTIME = 0.0f;
            DayTime = false;
        }
    }
    else{
        TimeofDay -= 20 * felapsedtime;
    }
    if (TimeofDay < 50){
        UpdateRain(felapsedtime);
        FTIME += felapsedtime;
        if (FTIME > 20){
            FTIME = 0;
            DayTime = true;
        }
    }
}

void Game_Engine::DrawMap() {
    TopleftTile = { (int)(tv.GetWorldTL().x / vTileSize.x),(int)(tv.GetWorldTL().y / vTileSize.y) };
    BottomeRightTile = { (int)ceil(tv.GetWorldBR().x / vTileSize.x),(int)ceil(tv.GetWorldBR().y / vTileSize.y) };
    if (TopleftTile.x < 0)
        TopleftTile.x = 0;
    if (TopleftTile.y < 0)
        TopleftTile.y = 0;
    if (BottomeRightTile.x >= 32)
        BottomeRightTile.x = 32;
    if (BottomeRightTile.y >= 30)
        BottomeRightTile.y = 30;
    for (int i = 0; i < mapLayers.size(); i++) {
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
}

void Game_Engine::GetUserInput(float felapsedtime) {
    if (followUnit.expired()) {
        if (GetKey(olc::Key::UP).bHeld) Camera.vVel += {0.0f, -1.f};
        if (GetKey(olc::Key::DOWN).bHeld) Camera.vVel += {0.0f, 1.0f};
        if (GetKey(olc::Key::LEFT).bHeld) Camera.vVel += {-1.0f, 0.0f};
        if (GetKey(olc::Key::RIGHT).bHeld) Camera.vVel += {1.0f, 0.0f};
        if (Camera.vVel.mag2() > 0)
            Camera.vVel *= (GetKey(olc::Key::SHIFT).bHeld ? 300.0f : 100.0f);
        Camera.vPOS += Camera.vVel * felapsedtime;
        Camera.vVel = { 0.0f,0.0f };
    }
    else {
        auto unit = followUnit.lock();
        unit->Velocity += olc::vf2d(unit->fMoveSpeed * GetKey(olc::Key::RIGHT).bHeld - unit->fMoveSpeed * GetKey(olc::Key::LEFT).bHeld,
            unit->fMoveSpeed * GetKey(olc::Key::DOWN).bHeld - unit->fMoveSpeed * GetKey(olc::Key::UP).bHeld) * felapsedtime;
        Camera.vPOS = unit->Position;
        if (GetKey(olc::Key::T).bPressed){
            unit->fHealth -= 5;//this is bad
        }
        if (GetKey(olc::Key::SPACE ).bPressed){
            unit->fHealth += 5;
        }
        
    }
    if (GetMouseWheel() > 0)    tv.ZoomAtScreenPos(2.0f, GetMousePos());
    if (GetMouseWheel() < 0)    tv.ZoomAtScreenPos(0.5f, GetMousePos());
    if (GetMouse(2).bPressed)   tv.StartPan(GetMousePos());
    if (GetMouse(2).bHeld)      tv.UpdatePan(GetMousePos());
    if (GetMouse(2).bReleased)  tv.EndPan(GetMousePos());

    tv.SetWorldOffset(Camera.vPOS - tv.ScaleToWorld(olc::vf2d(float(ScreenWidth()) / 2.0f, float(ScreenHeight()) / 2.0f)));
    
    if (!IsConsoleShowing()){
        if (IsFocused()){
            if (Clicked == false){
                Initial.x = int(1.0f / tv.GetWorldScale().x * GetMouseX() + tv.GetWorldTL().x);
                Initial.y = int(1.0f / tv.GetWorldScale().y * GetMouseY() + tv.GetWorldTL().y);
            }
            if (GetMouse(0).bHeld){
                Clicked = true;
                olc::vf2d FinalWH = 1.0f / tv.GetWorldScale() * GetMousePos() + tv.GetWorldTL();
                unitManager->SelectUnits(Initial, FinalWH);
            } else {
                Clicked = false;
            }
            if (GetMouse(1).bPressed){
                olc::vf2d FinalWH = 1.0f / tv.GetWorldScale() * GetMousePos() + tv.GetWorldTL();               
                 unitManager->MoveUnits(FinalWH,GetKey(olc::Key::A).bHeld);
                 
                
            }
            if (GetKey(olc::Key::P).bPressed){              
                tv.GetWorldScale().x;
                tv.GetWorldScale().y;
            }
        }
    }    
}