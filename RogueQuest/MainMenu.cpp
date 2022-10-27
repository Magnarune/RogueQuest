#include"MainMenu.h"
#include"Engine.h"
MainMenu::MainMenu() {

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
    bool rval = true;
	switch (MainMenuSelect) {
	case None:
        rval=StartMenu(delta);
		break;
	case Setup_Game:
		SetupGame(delta);
		break;
	case Settings:
		SettingMenu(delta);
		break;
	case Exit_Game:
		ExitProgram();
		break;
	}
    return rval;
}

bool MainMenu::StartMenu(float delta) {
    auto& engine = Game_Engine::Current();
    float X = (float)engine.ScreenWidth() * .372917f;
    float Y = (float)engine.ScreenHeight() * 0.37777f;
   
    
    float val = Occialtions(delta, TSDN, 5.f);
    if (val < 0.2f)
        val = 0.2f;
    
    engine.DrawPartialDecal({ 0.f,0.f }, { (float)engine.ScreenWidth(),(float)engine.ScreenHeight() }, menudecals["MainMenuBackround"].get(),
        { 0.f,0.f }, { 1200.f,1500.f },olc::PixelF(val, val, val, 1.f));



    engine.DrawPartialDecal({ 0.f,0.f }, { (float)engine.ScreenWidth(),(float)engine.ScreenHeight() }, menudecals["Titleforground"].get(), { 0.f,0.f }, { 1200.f,1500.f });
   
    
    olc::vf2d pos = { (float)engine.GetMousePos().x / (float)engine.ScreenWidth() ,(float)engine.GetMousePos().y / (float)engine.ScreenHeight()};
    engine.DrawStringDecal(engine.GetMousePos() - olc::vi2d(-5, -20), std::to_string(pos.x) + "/" + std::to_string(pos.y));
   // engine.DrawStringDecal(engine.GetMousePos() - olc::vi2d( 0, 20 ), std::to_string((float)engine.ScreenWidth()) + "/" + std::to_string((float)engine.ScreenHeight()));
    
    if (engine.GetKey(olc::Key::ESCAPE).bPressed) { engine.m_nGameMode = engine.MODE_LOCAL_MAP; engine.optionsManager->SetGuiMenu("MainMenu"); }
    
    float Xa = (float)engine.ScreenWidth() * 0.329167f;
    float Ya = (float)engine.ScreenHeight() * 0.162963f;


    engine.DrawDecal({Xa, Ya + 3.f * Occialtions(delta, TSOV, 0.2f) }, menudecals["Title"].get(), { 0.15f,0.15f });

    if (MenuButton({ X,Y }, engine.GetMousePos(), { 55.5f, 10.f }, true)){
        engine.DrawPartialDecal({ X,Y }, { 55.5,10 }, menudecals["MainMenuButtons"].get(), { 15,206 }, { 300,52 });
        if (engine.GetMouse(0).bPressed) { engine.m_nGameMode = engine.MODE_LOCAL_MAP; GameStarted = true; }
    } else
        engine.DrawPartialDecal({ X,Y }, { 55.5,10 }, menudecals["MainMenuButtons"].get(), { 15,128 }, { 300,52 }); 
    engine.DrawCenteredStringDecal({ X + 24,Y + 5 }, "Start Game", olc::WHITE, {0.4f,0.4f});

    float Dy = 25;
    if (MenuButton({ X,Y + Dy }, engine.GetMousePos(), { 55.5f, 10.f }, true)) {
        engine.DrawPartialDecal({ X,Y + Dy }, { 55.5,10 }, menudecals["MainMenuButtons"].get(), { 15,206 }, { 300,52 });
        if (engine.GetMouse(0).bPressed) engine.m_nGameMode = engine.MODE_OPTIONS_MENU;
    } else
        engine.DrawPartialDecal({ X,Y + Dy }, { 55.5,10 }, menudecals["MainMenuButtons"].get(), { 15,128 }, { 300,52 });
    engine.DrawCenteredStringDecal({ X + 24,Y + Dy + 5 }, "Settings", olc::WHITE, { 0.4f,0.4f });

    if (MenuButton({ X,Y + 2*Dy }, engine.GetMousePos(), { 55.5f, 10.f }, true)) {
        engine.DrawPartialDecal({ X,Y + 2*Dy }, { 55.5,10 }, menudecals["MainMenuButtons"].get(), { 15,206 }, { 300,52 });
        if (engine.GetMouse(0).bPressed) engine.m_nGameMode = engine.MODE_OPTIONS_MENU;
    } else
        engine.DrawPartialDecal({ X,Y + 2*Dy }, { 55.5,10 }, menudecals["MainMenuButtons"].get(), { 15,128 }, { 300,52 });

    engine.DrawCenteredStringDecal({ X + 24,Y + 2*Dy + 5 }, "Network", olc::WHITE, { 0.4f,0.4f });

    if (MenuButton({ X,Y + 3 * Dy }, engine.GetMousePos(), { 55.5f, 10.f }, true)) {
        engine.DrawPartialDecal({ X,Y + 3 * Dy }, { 55.5,10 }, menudecals["MainMenuButtons"].get(), { 15,206 }, { 300,52 });
        if (engine.GetMouse(0).bPressed) engine.m_nGameMode = engine.MODE_OPTIONS_MENU;
    } else
        engine.DrawPartialDecal({ X,Y + 3 * Dy }, { 55.5,10 }, menudecals["MainMenuButtons"].get(), { 15,128 }, { 300,52 });
    engine.DrawCenteredStringDecal({ X + 24,Y + 3 * Dy + 5 }, "Credits", olc::WHITE, { 0.4f,0.4f });

    if (MenuButton({ X,Y + 4 * Dy }, engine.GetMousePos(), { 55.5f, 10.f }, true)) {
        engine.DrawPartialDecal({ X,Y + 4 * Dy }, { 55.5,10 }, menudecals["MainMenuButtons"].get(), { 15,206 }, { 300,52 });
        if (engine.GetMouse(0).bPressed) return false;
    } else
        engine.DrawPartialDecal({ X,Y + 4 * Dy }, { 55.5,10 }, menudecals["MainMenuButtons"].get(), { 15,128 }, { 300,52 });
    engine.DrawCenteredStringDecal({ X + 24,Y + 4 * Dy + 5 }, "Exit Game", olc::WHITE, { 0.4f,0.4f });


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
    return true;
}


void  MainMenu::CreateGame() {

}

void MainMenu::ExitProgram() {

}
//                    felapsed time  speed of ossilation|internal clock|return value|     | how fast to turn around | when to go up or down in value
float MainMenu::Occialtions(float delta, float& timer, float rate){
    timer += delta;

    if (timer / rate >= 2* 3.14159265f)
        timer = 0.f;

    return  sinf(timer/rate) / (2.f) + .5f;//return normalized value from 0->1
}

void MainMenu::SetupGame(float delta) {

}

void MainMenu::SettingMenu(float delta) {

}
