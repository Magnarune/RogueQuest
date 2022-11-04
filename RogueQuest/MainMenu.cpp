#include"MainMenu.h"
#include"Engine.h"
MainMenu::MainMenu() {
    menus = {
        {"MainMenu", &mainMenu},//Main Menu
        {"SetupMenu", &setupMenu},//SetupGame Menu
        {"CreditsMenu", &creditsMenu},//Credits Menu
        {"NetworkMenu", &networkMenu}//Network Menu

    };

    auto& engine = Game_Engine::Current();
    float X = (float)engine.ScreenWidth() * .372917f;//magic numbers
    float Y = (float)engine.ScreenHeight() * 0.37777f;
    float Dy = 25;
    /*Main Menu*/
    mainMenu["btnSetup"] = Option(Option::Button, "Setup Game", [&](Option& opt) {SetGuiMenu("SetupMenu"); }, olc::vf2d(X, Y));
    mainMenu["btnSettings"] = Option(Option::Button, "Settings", [&](Option& opt) {engine.m_nGameMode = engine.MODE_OPTIONS_MENU; }, olc::vf2d(X, Y + Dy));
    mainMenu["btnNetwork"] = Option(Option::Button, "Network", [&](Option& opt) {/*not added*/}, olc::vf2d(X, Y + 2 * Dy));
    mainMenu["btnCredits"] = Option(Option::Button, "Credits", [&](Option& opt) {/*not added*/}, olc::vf2d(X, Y + 3 * Dy));
    mainMenu["btnExit"] = Option(Option::Button, "Exit Game", [&](Option& opt) { ExitGame = true; }, olc::vf2d(X, Y + 4 * Dy));
    //Setup Game Numbers
    float SX = (float)engine.ScreenWidth() * .172917f;//magic numbers
    float SY = (float)engine.ScreenHeight() * 0.17777f;
    /*Setup Game*/
    setupMenu["blInitiategame"] = Option(Option::Checkbox, "Initiate Game", [&](Option& opt) { InitiateGame = !InitiateGame; }, olc::vf2d(SX, SY), InitiateGame);
    setupMenu["btnStart"] = Option(Option::Button, "Play Game", [&](Option& opt) {GameStarted = true; CreateGame(); engine.m_nGameMode = engine.MODE_LOCAL_MAP; }, olc::vf2d(X, Y + 4 * Dy));
   
   
        setupMenu["btnGav"] = Option(Option::BigBoxButton, "Gavin", [&](Option& opt) {
            engine.leaders->LeaderList[1]->LeaderName = "Gavin";
            engine.leaders->LeaderList[1]->LeaderHead = engine.hud->decals["Gav"].get();
            }, olc::vf2d(X/2.f +0*25, Y/2.f ));
        setupMenu["btnHunt" ] = Option(Option::BigBoxButton, "Hunter", [&](Option& opt) {
            engine.leaders->LeaderList[1]->LeaderName = "Hunter";
            engine.leaders->LeaderList[1]->LeaderHead = engine.hud->decals["Hunt"].get();
            }, olc::vf2d(X / 2.f + 1 * 25, Y / 2.f));
        setupMenu["btnJack"] = Option(Option::BigBoxButton, "Jackson", [&](Option& opt) {
            engine.leaders->LeaderList[1]->LeaderName = "Jackson";
            engine.leaders->LeaderList[1]->LeaderHead = engine.hud->decals["Jack"].get();
            }, olc::vf2d(X / 2.f + 2 * 25, Y / 2.f));
        setupMenu["btnLando"] = Option(Option::BigBoxButton, "Landon", [&](Option& opt) {
            engine.leaders->LeaderList[1]->LeaderName = "Landon";
            engine.leaders->LeaderList[1]->LeaderHead = engine.hud->decals["Lando"].get();
            }, olc::vf2d(X / 2.f + 3 * 25, Y / 2.f));
        setupMenu["btnLuke"] = Option(Option::BigBoxButton, "Luke", [&](Option& opt) {
            engine.leaders->LeaderList[1]->LeaderName = "Luke";
            engine.leaders->LeaderList[1]->LeaderHead = engine.hud->decals["Luke"].get();
            }, olc::vf2d(X / 2.f + 4 * 25, Y / 2.f));
   
}
MainMenu::~MainMenu() {

}
void MainMenu::loadImage(const std::string& name, const std::string& path) {
    std::unique_ptr<olc::Decal> ptr;
    ptr.reset(
        new olc::Decal(
            TextureCache::GetCache().GetTexture(
                TextureCache::GetCache().CreateTexture(path)
            )
        )
    );
    menudecals.insert_or_assign(name, std::move(ptr));
};
void MainMenu::loadImage(const std::string& name, size_t tex_id) {
    std::unique_ptr<olc::Decal> ptr;
    ptr.reset(
        new olc::Decal(
            TextureCache::GetCache().GetTexture(tex_id)
        )
    );
    menudecals.insert_or_assign(name, std::move(ptr));
};
void MainMenu::ImportMainMenuAssets() {
    loadImage("MainMenuBackround", "Assets/MainMenu/Backround.png");
    loadImage("MainMenuButtons", "Assets/MainMenu/MainMenuButtons.png");
    loadImage("Title", "Assets/MainMenu/Title_Card.png");//1296 126
    loadImage("Titleforground", "Assets/MainMenu/Backround_windo_and_grass.png");  
}
bool MainMenu::MenuButton(olc::vf2d pos, olc::vi2d Mouse, olc::vf2d Size, bool hover){
    auto& engine = Game_Engine::Current();
    if (hover) {
        if (Mouse.x > pos.x && Mouse.y > pos.y &&
            Mouse.x < pos.x + Size.x && Mouse.y < pos.y + Size.y) {
            return true;

        }
        return false;
    }

    if (engine.GetMouse(0).bPressed &&
        Mouse.x > pos.x && Mouse.y > pos.y &&
        Mouse.x < pos.x + Size.x && Mouse.y < pos.y + Size.y) {
        return true;
    }
    return false;
}

bool MainMenu::MainMenuSelection(float delta) {
    auto& engine = Game_Engine::Current();
    if (engine.GetKey(olc::Key::ESCAPE).bPressed) {
        engine.m_nGameMode = engine.MODE_LOCAL_MAP;
        engine.optionsManager->SetGuiMenu("MainMenu");
    }
    float val = Occialtions(delta, TSDN, 5.f);
    if (val < 0.2f)
        val = 0.2f;
    float Xa = (float)engine.ScreenWidth() * 0.329167f;
    float Ya = (float)engine.ScreenHeight() * 0.162963f;
    engine.DrawDecal({ Xa, Ya + 3.f * Occialtions(delta, TSOV, 0.2f) }, menudecals["Title"].get(), { 0.15f,0.15f });
    engine.DrawPartialDecal({ 0.f,0.f }, { (float)engine.ScreenWidth(),(float)engine.ScreenHeight() }, menudecals["MainMenuBackround"].get(),
        { 0.f,0.f }, { 1200.f,1500.f }, olc::PixelF(val, val, val, 1.f));
    engine.DrawPartialDecal({ 0.f,0.f }, { (float)engine.ScreenWidth(),(float)engine.ScreenHeight() }, menudecals["Titleforground"].get(), { 0.f,0.f }, { 1200.f,1500.f });
    timer += delta;
    if (timer > 1.f) {
        timer = 0.f;
        if (!down)
            occilate += 1.f;
        else
            occilate -= 1.f;

        if (occilate < 0.f)
            down = false;
        if (occilate > 3.f)
            down = true;
    }
    DrawGui(*currentMenu);
    return !ExitGame;
}

void MainMenu::DrawGui(OptionsList& options) {
    auto& engine = Game_Engine::Current();
    for (auto& [name, option] : options) {

        static const std::map<Option::GuiType, olc::vf2d> sizeChart{ // gui element sizes go here for now :)
            {Option::Checkbox,{8.f, 8.f}},
            {Option::Button,{55.5f,10.f}},
            {Option::BigBoxButton,{24.f,24.f}},
        };
        olc::vf2d size{ 0,0 };
        if (sizeChart.count(option.type)) size = sizeChart.at(option.type);

        olc::vf2d mpos = olc::vf2d(engine.GetMousePos());
        std::function<void()> localchange = []() {};
        bool hover = (mpos.x >= option.position.x && mpos.y >= option.position.y && mpos.x < option.position.x + size.x && mpos.y < option.position.y + size.y);

        switch (option.type) {
        case Option::Checkbox: {
            engine.DrawPartialDecal(option.position, { 5,5 }, menudecals["MainMenuButtons"].get(), { 152.f,17.f }, { 17.f,17.f });
            if (std::get<bool>(option.value))
                engine.DrawPartialDecal(option.position, { 5,5 }, menudecals["MainMenuButtons"].get(), { 152.f,61.f }, { 17.f,17.f });
            engine.DrawStringDecal(option.position - olc::vf2d({ 16.f, 12.f }), option.label, olc::WHITE, { 0.4f,0.4f });
            localchange = [&]() { option.value = !std::get<bool>(option.value); };//Where Option gets altered
            break; }
        case Option::Button: {
            olc::vf2d src = hover ? olc::vf2d(15.f, 206.f) : olc::vf2d(15.f, 128.f);
            engine.DrawPartialDecal(option.position, size, menudecals["MainMenuButtons"].get(), src, { 300,52 });
            engine.DrawStringDecal(option.position + olc::vf2d(11.f, 4.f), option.label, olc::WHITE, { 0.4f,0.4f });
            break; }
        case Option::Label: {
            engine.DrawStringDecal(option.position, option.label, olc::WHITE); // eh.. can make it configurable later
            hover = 0;
            break; }
        case Option::BigBoxButton: {
            olc::vf2d src = hover ?  olc::vf2d(96, 227): olc::vf2d(96, 128);
            size = { 24.f,24.f };
            engine.DrawPartialDecal(option.position, size, engine.hud->decals["Hud"].get(), src, { 96.f,96.f });
            engine.DrawStringDecal(option.position - olc::vf2d(1.f, 4.f), option.label, olc::WHITE, { 0.4f,0.4f });
            break; }
        }
                          
        
        // hmm .. temporarily .. but should be moved
        if (engine.GetMouse(0).bPressed && hover) {
            localchange(); // change any value if necessary
            option.callback(option);
        }
    }
    //Special Menu Draws
    float X = (float)engine.ScreenWidth() * .372917f;//magic numbers
    float Y = (float)engine.ScreenHeight() * 0.37777f;
    if (currentMenu == menus["SetupMenu"]) {
        for (int i = 0; i < 5; i++) {
            engine.DrawPartialDecal({ X / 2.f + 0 +2.f, Y / 2.f +2.f}, { 20.f,20.f }, engine.hud->decals["Gav"].get(), { 0.f,0.f }, {256,256});
            engine.DrawPartialDecal({ X / 2.f +  25 + 2.f, Y / 2.f + 2.f }, { 20.f,20.f }, engine.hud->decals["Hunt"].get(), { 0.f,0.f }, { 256,256 });
            engine.DrawPartialDecal({ X / 2.f + 2 * 25 + 2.f, Y / 2.f + 2.f }, { 20.f,20.f }, engine.hud->decals["Jack"].get(), { 0.f,0.f }, { 256,256 });
            engine.DrawPartialDecal({ X / 2.f +3 * 25 + 2.f, Y / 2.f + 2.f }, { 20.f,20.f }, engine.hud->decals["Lando"].get(), { 0.f,0.f }, { 256,256 });
            engine.DrawPartialDecal({ X / 2.f + 4 * 25 + 2.f, Y / 2.f + 2.f }, { 20.f,20.f }, engine.hud->decals["Luke"].get(), { 0.f,0.f }, { 256,256 });
        }
        if (engine.leaders->LeaderList[1]->LeaderName != "") {
            engine.DrawStringDecal({ X / 12,Y * 1.45f }, engine.leaders->LeaderList[1]->LeaderName, olc::WHITE, { 0.5f,0.5f });
            engine.DrawPartialDecal({ X / 13,Y * 1.5f }, { 96,96 }, engine.leaders->LeaderList[1]->LeaderHead, { 0,0 }, { 256,256 });
        }
    }
}

void  MainMenu::CreateGame() {
    auto& engine = Game_Engine::Current();
    if(InitiateGame)
        engine.InitiateGame();
}

//                    felapsed time  speed of ossilation|internal clock|return value|     | how fast to turn around | when to go up or down in value
float MainMenu::Occialtions(float delta, float& timer, float rate){
    timer += delta;
    if (timer / rate >= 2* 3.14159265f)
        timer = 0.f;
    return  sinf(timer/rate) / (2.f) + .5f;//return normalized value from 0->1
}

MainMenu::Option::Option(MainMenu::Option::GuiType t, const std::string& l, MainMenu::Option::CallbackType cb, olc::vf2d pos, MainMenu::Option::ValueType def) :
    type(t), label(l), callback(cb), position(pos), value(def) {}