#include "Engine.h"
#include <set>
#include <functional>
#include <vector>

std::string StringifyObject(sol::object val) {
    std::set<const void*> tableMap;
    std::function<std::string(sol::object)> Stringify = [&](sol::object value) -> std::string {

        switch (value.get_type()) {
        case sol::type::userdata: return "<userdata>";
        case sol::type::boolean: return std::to_string(value.as<bool>());
        case sol::type::string: return value.as<std::string>();
        case sol::type::function: return "<function>";
        case sol::type::number: {
            double val = value.as<double>();
            return fmod(val, 1.0f) == 0.0f ? std::to_string(int64_t(val)) : std::to_string(val);
        }
        case sol::type::table: {
            if (tableMap.count(value.pointer())) {
                return "...";
            }
            tableMap.emplace(value.pointer());
            std::string copy = "{ ";
            sol::table table = value.as<sol::table>();
            for (auto it = table.begin(); it != table.end(); ++it) {
                std::pair<std::string, std::string> fix, fix2;
                if ((*it).first.get_type() == sol::type::table) {
                    fix.first = "[";
                    fix.second = "]";
                }
                if ((*it).second.get_type() == sol::type::string) {
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
    sAppName = "Simple PGE RTS GAME";
    self = this;
}
Game_Engine::~Game_Engine() {
    self = nullptr;
}
bool Game_Engine::OnUserCreate() {

    // Setup Controllers    
    TextureCache::InitCache(); // initialize texture cache
    config.reset(new Config("Config.lua",true)); // config manager
    bIsLocked = config->GetValue<bool>("ScreenLocked");
    worldManager.reset(new WorldManager); // create the world manager
    unitManager.reset(new UnitManager); // create the unit manager
    buildingManager.reset(new BuildingManager); //create the build manager
    assetManager.reset(new cAssets); // create the asset manager
    hud.reset(new Hud); // create the Hud
    hudManager.reset(new HudManager);//Creates HudManager
    optionsManager.reset(new Options);//Create options menu
    userinputs.reset(new UserInput); //Create user options    
    inputmanager.reset(new UserInputManager);
    particles.reset(new Particles);
    leaders.reset(new LeaderManager); //Handler for the Leaders in the game
    
    // Configure Controllers
    assetManager->LoadUnitAssets();     // Load all the Lua files
    assetManager->LoadBuildingAssets(); // Load all the Buildings files
    assetManager->LoadProjectileAssets(); // Load all Projectile files
    assetManager->LoadCursorAssets();   // Load all the Cursors files
    worldManager->ImportMapData();      // Load all the Lua Map files
    hud->ImportHudAssets();      // Load all the Hud files

    // Setup Viewport
    tv = olc::TileTransformedView({ ScreenWidth(), ScreenHeight() }, { 1,1 });   

    worldManager->ChangeMap("Basic");
    SetPixelMode(olc::Pixel::ALPHA);
    ConsoleCaptureStdOut(true);     
    Clear(olc::Pixel(100,164,44,255));
    optionsManager->SetGuiMenu("MainMenu");
    ShowSystemCursor(false); 

    leaders->AddLeader(5);

    curCursor = assetManager->GetCursor("default");
    SetLocked(true);
    return true;
}

bool Game_Engine::OnUserUpdate(float fElapsedTime) {
    bool rval = true;
    fElapsedTime = std::fmin(1.f, fElapsedTime);

    if(_lastfocus != IsFocused()){
        _lastfocus = IsFocused();
        OnFocusUpdated(_lastfocus);
    }

    if (GetKey(olc::Key::F1).bPressed) ConsoleShow(olc::Key::ESCAPE);

    switch (m_nGameMode){
        case MODE_LOCAL_MAP:
            rval = UpdateLocalMap(fElapsedTime);
            break;
        case MODE_OPTIONS_MENU:
            rval = UpdateOptions(fElapsedTime);
            break;
    }

    DrawCursor();
    return rval;
}

bool Game_Engine::UpdateLocalMap(float fElapsedTime) {
    userinputs->GetUserInput();
    // Game Updates
    if(!IsConsoleShowing()){
        unitManager->Update(fElapsedTime);
        worldManager->Update(fElapsedTime);
        particles->UpdateParticles(fElapsedTime);
        hud->Update(fElapsedTime); // hud update should be last for game updates (I think)
    }
    // GC
    worldManager->CollectGarbage();
    // Drawing
    worldManager->Draw();
    
    userinputs->DrawUserInput();
    hud->DrawLeaderHud();
    hud->DrawHud();
    
    hud->DrawMiniMap();

    return true;
};

bool Game_Engine::UpdateOptions(float fElapsedTime) {
    optionsManager->MenuSelect();
    return true;
}

void Game_Engine::DrawCursor() {//Assets\Gui\Cursors\cursor.png
    if(curCursor == nullptr) return;

    DrawDecal(GetMousePos(), curCursor->decal.get(), curCursor->scale);
}

void Game_Engine::OnFocusUpdated(bool focus){
    SetLocked(focus ? bWasLocked : false, false);
}

void Game_Engine::SetLocked(bool locked, bool permanent) {
    config->SaveValue<bool>("ScreenLocked", locked);

    if(permanent) bWasLocked = locked;
    if(locked && olc::platform){ // lock cursor
        HWND mHwnd = *(HWND*)olc::platform->GetWindowHandle();
        GetWindowRect(mHwnd, &my_rect);
        my_rect.top += 48L;
        my_rect.left += 16L;
        my_rect.bottom -= 16L;
        my_rect.right -= 16L;
      //  ClipCursor(&my_rect);
    } else {
        ClipCursor(nullptr);
    }
}

bool Game_Engine::OnUserDestroy(){
    worldManager.reset();
    unitManager.reset();
    buildingManager.reset();
    assetManager.reset();
    hud.reset();
    optionsManager.reset();
    userinputs.reset();
    inputmanager.reset();
    TextureCache::FreeCache();
    particles.reset();
    leaders.reset();
    return true;
}

typedef std::function<void(std::stringstream&)> CommandFunction;

const std::map<std::string, CommandFunction> commands = {
    {"add", [](std::stringstream& ss) {
            auto& engine = Game_Engine::Current();
            
            std::string name;
            float x {}, y {}, count = 1 , owner = 1;
            ss >> name;
            if(!ss.eof()) ss >> x;
            if(!ss.eof()) ss >> y;
            if(!ss.eof()) ss >> count;
            if (!ss.eof()) ss >> owner;
            if (owner < engine.leaders->LeaderList.size()) {
                for (int i = 0; i < count; i++) {
                    auto unit = engine.worldManager->GenerateUnit(name, owner, { x * 32 + 15 * i, y * 32 + i });
                    if (unit) {

                        std::cout << name << " created at " << x << ", " << y << "\n";
                    }
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
            unit->ULogic = Unit::Aggressive;
            
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
                unit->Health -= 1000.f;
                engine.particles->CreateParticles(unit->Position);            
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
    }},
    {"load", [](std::stringstream& ss) {
        auto& engine = Game_Engine::Current();

        std::string name;
        if (!ss.eof()) ss >> name;

        if(engine.worldManager->ChangeMap(name)){
            std::cout << "Map Changed\n";
        }
    }},
    {"build", [](std::stringstream& ss) {
        auto& engine = Game_Engine::Current();
        std::string name;
        float x{}, y{};
        ss >> name;
        if (!ss.eof()) ss >> x;
        if (!ss.eof()) ss >> y;

        if (engine.worldManager->GenerateBuilding(name,1, {x*32.f,y*32.f})) {
            std::cout << "Building Made\n";
        }
    }},
    { "Crush", [](std::stringstream& ss) {
    auto& engine = Game_Engine::Current();
    std::string name;
    float x{}, y{};
    ss >> name;
    if (!ss.eof()) ss >> x;
    if (!ss.eof()) ss >> y;
    auto Building = engine.buildingManager->GetBuilding(name, x);
    if (Building) {
        Building->Destroy();
        std::cout << "Building Crushed\n";
    }
    }},

    /*
    {"", [](std::stringstream& ss){
        auto& engine = Game_Engine::Current();

    }},
    */
};

bool Game_Engine::OnConsoleCommand(const std::string& stext) {
    std::stringstream ss;
    ss << stext;
    std::string c1;
    ss >> c1;
    if(commands.count(c1)){
        std::cout << config->GetValue<std::string>("Squelch") << " ";
        commands.at(c1)(ss);
    } else {
        std::cout << "Command " << c1 << " does not exist\n";
    }
    return true;
}




/*
    struct VirtualObjectType {
        std::vector<std::vector<int>> matrix;
        bool destroyable;
    };

    std::vector<VirtualObjectType> VirtualCollisionObjectDatabase;
    // loaded from lua

    VirtualObjectTypes = [
        {
            matrix = [
                [11,11,122,11],
                [11,19,11,11],
                [11,121,10,11],
                [11,11,11,19]
            ],
            objectName = "TreeThing",
            destroyable = true,
        },
        {
            matrix = [
                [14,1,85,11],
                [10,15,18,20],
                [11,121,11,11],
                [10,10,11,19]
            ],
            objectName = "Something",
            destroyable = false
        },
    ]


    // LOADING MAP:
    Iterate World {
        tile -> 
            Iterate -> VirtualCollisionObjectDatabase {
                VCO -> Check if matches first tile and creates matched tile box

            }
    }


    struct VirtualObject {
        position {x,y}
        size {width, height}
        destructable true|false

        layer
        tilePosition {tx, ty}
        eraseOnDestroy true|false

        onErase() { < clear the tilePosition + size > }
    }

    Map::VirtualObjects [VirtualObject, VirtualObject, VirtualObject ...]


    CollisionCheck -> {
        Iterate spatial grid -> (automatic) {
            collide with VirtualObject...
            logic can allow for removing VirtualObject
        }
    }

*/