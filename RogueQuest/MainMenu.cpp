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
    setupMenu["blInitiategame"] = Option(Option::Checkbox, "Initiate Game", [&](Option& opt) { InitiateGame = !InitiateGame; }, olc::vf2d(SX, SY),InitiateGame);
    setupMenu["btnStart"] = Option(Option::Button, "Play Game", [&](Option& opt) {GameStarted=true; CreateGame(); engine.m_nGameMode = engine.MODE_LOCAL_MAP; }, olc::vf2d(X, Y + 4 * Dy));
    
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
    //olc::vf2d pos = { (float)engine.GetMousePos().x / (float)engine.ScreenWidth() ,(float)engine.GetMousePos().y / (float)engine.ScreenHeight()};
    //engine.DrawStringDecal(engine.GetMousePos() - olc::vi2d(-5, -20), std::to_string(pos.x) + "/" + std::to_string(pos.y));
   // engine.DrawStringDecal(engine.GetMousePos() - olc::vi2d( 0, 20 ), std::to_string((float)engine.ScreenWidth()) + "/" + std::to_string((float)engine.ScreenHeight()));
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
            {Option::Button,{55.5f,10.f}}
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
        }
        // hmm .. temporarily .. but should be moved
        if (engine.GetMouse(0).bPressed && hover) {
            localchange(); // change any value if necessary
            option.callback(option);
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