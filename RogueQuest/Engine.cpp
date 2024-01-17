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
    
    MapTextureCache::InitCache();
    TextureCache::InitCache(); // initialize texture cache
    config.reset(new Config("Config.lua", true)); // config manager
    soundmanager.reset(new SoundManager);//Sound manager
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
    highlightmanagment.reset(new Highlighting);
    researchmanager.reset(new Research);
    mainmenu.reset(new MainMenu);
    cmapmanager.reset(new CollisionMap);
    //New
    nodemanager.reset(new MoveLogic);
    
    
    // Configure Controllers
    cmapmanager->ImportCollisionData();//Imports all the possible world map objects
    mainmenu->ImportMainMenuAssets();
    assetManager->LoadUnitAssets();     // Load all the Lua files
    assetManager->LoadBuildingAssets(); // Load all the Buildings files
    assetManager->LoadProjectileAssets(); // Load all Projectile files
    assetManager->LoadCursorAssets();   // Load all the Cursors files
    assetManager->LoadResearchAssets(); //Load the Research files
    researchmanager->ImportResearch(); //Load all the research data
    worldManager->ImportMapData();      // Load all the Lua Map files
    hud->ImportHudAssets();      // Load all the Hud files
    particles->ImportEffectsAssets();
    assetManager->ImportGameMusic();
    // Setup Viewport
    tv = olc::TileTransformedView({ ScreenWidth(), ScreenHeight() }, { 1,1 });   
    
    worldManager->ChangeMap("Terra");//here

    SetPixelMode(olc::Pixel::ALPHA);
    ConsoleCaptureStdOut(true);     
    Clear(olc::Pixel(100,164,44,255));
    mainmenu->SetGuiMenu("MainMenu");
    optionsManager->SetGuiMenu("MainMenu");
    ShowSystemCursor(false); 

    leaders->AddLeader(5);//add Main Menu
    for (auto& L : leaders->LeaderList)
        L->Gold = 0;
    leaders->LeaderList[1]->Gold += 1000;
    Camera.vPOS = { 30.f,30.f };
    tv.SetWorldOffset(Camera.vPOS);

    curCursor = assetManager->GetCursor("default");

    bIsLocked = config->GetValue<bool>("ScreenLocked");

    SetLocked(bIsLocked);
    cmapmanager->GenerateCollisionObject("Bussy Tree", { 3 * 190.4f,3 * 200.5f });

   // soundmanager->Play_System_Sound("Main Menu");

    return true;
}

bool Game_Engine::OnUserUpdate(float fElapsedTime) {
    bool rval = true;
    fElapsedTime = std::fmin(0.3f, fElapsedTime);

    if(_lastfocus != IsFocused()){
        _lastfocus = IsFocused();
        OnFocusUpdated(_lastfocus);
    }

    if (GetKey(olc::Key::F4).bPressed) ConsoleShow(olc::Key::ESCAPE);

    switch (m_nGameMode){
        case MODE_LOCAL_MAP:
            rval = UpdateLocalMap(fElapsedTime);
            break;
        case MODE_OPTIONS_MENU:
            rval = UpdateOptions(fElapsedTime);
            break;
        case MODE_MAIN_MENU:
            rval = UpdateMainMenu(fElapsedTime);
            break;
    }




    DrawCursor();
    return rval;
}

bool Game_Engine::UpdateLocalMap(float fElapsedTime) {
    fElapsedTime = std::min(fElapsedTime, 0.8f); // Cap to 800ms
    userinputs->GetUserInput();
    // Game Updates
    if(!IsConsoleShowing()){
        buildingManager->Update(fElapsedTime);
        unitManager->Update(fElapsedTime);
        worldManager->Update(fElapsedTime);
        worldManager->PopulateNewObjects();
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
    

    
   // DrawStringDecal(GetMousePos() - olc::vi2d(-5, -20), std::to_string((float)GetMousePos().x / (float)ScreenWidth()) + "/" + std::to_string((float)GetMousePos().y / (float)ScreenHeight()));
 
    return true;
};

bool Game_Engine::UpdateOptions( float fElapsedTime) {
    return optionsManager->MenuSelect();
}

bool Game_Engine::UpdateMainMenu(float fElapsedTime) {
return mainmenu->MainMenuSelection(fElapsedTime);
}

void Game_Engine::DrawCursor() {//Assets\Gui\Cursors\cursor.png
    if(curCursor == nullptr) return;
    //Draw Dev


    for (int i = 0; i < nodemanager->Move_Node.size(); i++) {
        
        tv.DrawLineDecal(nodemanager->Move_Node[i] - nodemanager->UnitW / 2.f, nodemanager->Move_Node[i] + olc::vf2d(-nodemanager->UnitW.x, nodemanager->UnitW.y) / 2.f, olc::RED);
        tv.DrawLineDecal(nodemanager->Move_Node[i] - nodemanager->UnitW / 2.f, nodemanager->Move_Node[i] + olc::vf2d(nodemanager->UnitW.x, -nodemanager->UnitW.y) / 2.f, olc::RED);
        tv.DrawLineDecal(nodemanager->Move_Node[i] +  nodemanager->UnitW / 2.f, nodemanager->Move_Node[i] + olc::vf2d(-nodemanager->UnitW.x, nodemanager->UnitW.y) / 2.f, olc::RED);
        tv.DrawLineDecal(nodemanager->Move_Node[i] +  nodemanager->UnitW / 2.f, nodemanager->Move_Node[i] + olc::vf2d(nodemanager->UnitW.x, -nodemanager->UnitW.y) / 2.f, olc::RED);

        //tv.DrawStringDecal(nodemanager->Move_Node[i], "`", olc::RED, {1.f,1.f});
    }
    tv.DrawStringDecal(nodemanager->Test_Node, "`", olc::BLACK, { 1.f,1.f });

    tv.DrawStringDecal(nodemanager->dev_X, "`", olc::BLACK, { 1.f,1.f });
    tv.DrawStringDecal(nodemanager->dev_W, "`", olc::BLACK, { 1.f,1.f });
    tv.DrawStringDecal(nodemanager->dev_Z, "`", olc::BLACK, { 1.f,1.f });
    tv.DrawStringDecal(nodemanager->dev_Y, "`", olc::BLUE, { 1.f,1.f });
    tv.DrawStringDecal(nodemanager->dev_Pos, "`", olc::CYAN);


    tv.DrawLineDecal(nodemanager->dev2_ZZ, nodemanager->dev2_WW, olc::CYAN);
    tv.DrawLineDecal(nodemanager->dev2_ZZ, nodemanager->dev2_XX, olc::CYAN);
    tv.DrawLineDecal(nodemanager->dev2_YY, nodemanager->dev2_XX, olc::CYAN);
    tv.DrawLineDecal(nodemanager->dev2_WW, nodemanager->dev2_YY, olc::CYAN);

    tv.DrawLineDecal(nodemanager->dev2_Z, nodemanager->dev2_W, olc::CYAN);
    tv.DrawLineDecal(nodemanager->dev2_Z, nodemanager->dev2_X, olc::CYAN);
    tv.DrawLineDecal(nodemanager->dev2_Y, nodemanager->dev2_X, olc::CYAN);
    tv.DrawLineDecal(nodemanager->dev2_W, nodemanager->dev2_Y, olc::CYAN);

    tv.DrawStringDecal(nodemanager->dev2_X, "l", olc::BLACK, { 1.f,1.f });
    tv.DrawStringDecal(nodemanager->dev2_W, "l", olc::BLACK, { 1.f,1.f });
    tv.DrawStringDecal(nodemanager->dev2_Z, "l", olc::BLACK, { 1.f,1.f });
    tv.DrawStringDecal(nodemanager->dev2_Y, "l", olc::BLACK, { 1.f,1.f });

    DrawDecal(GetMousePos(), curCursor->decal.get(), curCursor->scale);
}

void Game_Engine::OnFocusUpdated(bool focus){
    SetLocked(focus ? bWasLocked : false, false);
}

void Game_Engine::SetLocked(bool locked, bool permanent) {
    config->SaveValue<bool>("ScreenLocked", locked);
    bIsLocked = locked;
    if(permanent) bWasLocked = locked;
    if(locked && olc::platform){ // lock cursor
        HWND mHwnd = *(HWND*)olc::platform->GetWindowHandle();
        GetWindowRect(mHwnd, &my_rect);
        my_rect.top += 48L;
        my_rect.left += 16L;
        my_rect.bottom -= 16L;
        my_rect.right -= 16L;
        ClipCursor(&my_rect);
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
    MapTextureCache::FreeCache();
    particles.reset();
    leaders.reset();
    highlightmanagment.reset();
    config.reset();
    researchmanager.reset();
    mainmenu.reset();
    cmapmanager.reset();
    soundmanager.reset();
    nodemanager.reset();
    return true;
}

typedef std::function<void(std::stringstream&)> CommandFunction;

const std::map<std::string, CommandFunction> commands = {
    {"add", [](std::stringstream& ss) {
            auto& engine = Game_Engine::Current();
            
            std::string name;
            float x {}, y {};
            int count = 1, owner = 1;

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
                engine.particles->GenerateBlood(unit->Position);            
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
    size_t index {};
    ss >> name;
    if (!ss.eof()) ss >> index;
    auto Building = engine.buildingManager->GetBuilding(name, index);
    if (Building) {
        Building->Destroy();
        std::cout << "Building Crushed\n";
    }
    }},
    { "Plant", [](std::stringstream& ss) {
    auto& engine = Game_Engine::Current();
    std::string name;
    size_t index {};
    ss >> name;
    if (!ss.eof()) ss >> index;
    auto Building = engine.buildingManager->GetBuilding(name, index);
    if (Building) {
        Building->Destroy();
        std::cout << "Building Crushed\n";
    }
    } },
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

void Game_Engine::InitiateGame() {//For Debugging Ease
    worldManager->GenerateBuilding("GoldMine", 1, { 19 * 32.f,14 * 32.f });
    const auto& Castle = worldManager->GenerateBuilding("Castle", 1, { 36 * 32.f  ,  14 * 32.f });
    Castle->Health = Castle->maxHealth;
    Castle->curStage = 1;
     
    const auto& House = worldManager->GenerateBuilding("House", 1, { 25 * 32.f  ,  19 * 32.f });
    House->Health = House->maxHealth;
    House->curStage = 1;
    const auto& Tower =  worldManager->GenerateBuilding("StoneTower", 1, { 20 * 32.f  ,  19 * 32.f });
    Tower->Health = Tower->maxHealth;
    Tower->curStage = 2;

    for(int i=0; i< 20; i++)
        worldManager->GenerateUnit("Archer", 1, {3*i+19.f * 32.f,10 * 32.f });
    worldManager->GenerateUnit("Builder", 1, { 21 * 32.f,14 * 32.f });

    worldManager->GenerateBuilding("GoldMine", 0, { 18 * 32.f,30 * 32.f });

    const auto& Castles = worldManager->GenerateBuilding("Castle", 0, { 25 * 32.f  ,  30 * 32.f });
    Castles->Health = Castles->maxHealth;
    Castles->curStage = 1;
    const auto& Houses= worldManager->GenerateBuilding("House", 0, { 25 * 32.f  ,  35 * 32.f });
    Houses->Health = Houses->maxHealth;
    Houses->curStage = 1;
    const auto& Towers =worldManager->GenerateBuilding("StoneTower", 0, { 20 * 32.f  ,  35 * 32.f });
    Towers->Health = Towers->maxHealth;
    Towers->curStage = 2;
    worldManager->GenerateUnit("Builder", 0, { 19 * 32.f,30 * 32.f });
    for (int i = 0; i < 20; i++)
        worldManager->GenerateUnit("Archer", 0, { 3* i + 19.f * 32.f,38 * 32.f });
}

void Game_Engine::DrawCenteredStringDecal(olc::vf2d pos, const std::string& str, olc::Pixel col, olc::vf2d scale) {
    olc::vf2d textOffset = olc::vf2d(GetTextSize(str)) * scale / 2.f;
    DrawStringDecal(pos - textOffset, str, col, scale);
}
