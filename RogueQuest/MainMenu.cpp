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
    float X = 90.f;
    float Y = 83.f;
   
    
    
    
    engine.DrawPartialDecal({ 0.f,0.f }, { (float)engine.ScreenWidth(),(float)engine.ScreenHeight() }, menudecals["MainMenuBackround"].get(),
        { 0.f,0.f }, { 1200.f,1500.f },olc::PixelF(Occialtions(delta, 0.4f,TSDN,TSOV,5.f, TS), Occialtions(delta, 0.4f, TSDN, TSOV, 5.f, TS), Occialtions(delta, 0.4f, TSDN, TSOV, 5.f, TS), 1.f));



    engine.DrawPartialDecal({ 0.f,0.f }, { (float)engine.ScreenWidth(),(float)engine.ScreenHeight() }, menudecals["Titleforground"].get(), { 0.f,0.f }, { 1200.f,1500.f });
    
    
    
    if (engine.GetKey(olc::Key::ESCAPE).bPressed) { engine.m_nGameMode = engine.MODE_LOCAL_MAP; engine.optionsManager->SetGuiMenu("MainMenu"); }
    



    engine.DrawDecal({ 40.f,23.f +occilate }, menudecals["Title"].get(), { 0.15f,0.15f });

    if (MenuButton({ X,Y }, engine.GetMousePos(), { 55.5f, 10.f }, true)){
        engine.DrawPartialDecal({ X,Y }, { 55.5,10 }, menudecals["MainMenuButtons"].get(), { 15,206 }, { 300,52 });
        if (engine.GetMouse(0).bPressed) engine.m_nGameMode = engine.MODE_LOCAL_MAP;
    } else
        engine.DrawPartialDecal({ X,Y }, { 55.5,10 }, menudecals["MainMenuButtons"].get(), { 15,128 }, { 300,52 }); 
    engine.hud->DrawCenteredStringDecal({ X + 24,Y + 5 }, "Start Game", olc::WHITE, {0.4f,0.4f});

    float Dy = 25;
    if (MenuButton({ X,Y + Dy }, engine.GetMousePos(), { 55.5f, 10.f }, true)) {
        engine.DrawPartialDecal({ X,Y + Dy }, { 55.5,10 }, menudecals["MainMenuButtons"].get(), { 15,206 }, { 300,52 });
        if (engine.GetMouse(0).bPressed) engine.m_nGameMode = engine.MODE_OPTIONS_MENU;
    } else
        engine.DrawPartialDecal({ X,Y + Dy }, { 55.5,10 }, menudecals["MainMenuButtons"].get(), { 15,128 }, { 300,52 });
    engine.hud->DrawCenteredStringDecal({ X + 24,Y + Dy + 5 }, "Settings", olc::WHITE, { 0.4f,0.4f });

    if (MenuButton({ X,Y + 2*Dy }, engine.GetMousePos(), { 55.5f, 10.f }, true)) {
        engine.DrawPartialDecal({ X,Y + 2*Dy }, { 55.5,10 }, menudecals["MainMenuButtons"].get(), { 15,206 }, { 300,52 });
        if (engine.GetMouse(0).bPressed) engine.m_nGameMode = engine.MODE_OPTIONS_MENU;
    } else
        engine.DrawPartialDecal({ X,Y + 2*Dy }, { 55.5,10 }, menudecals["MainMenuButtons"].get(), { 15,128 }, { 300,52 });

    engine.hud->DrawCenteredStringDecal({ X + 24,Y + 2*Dy + 5 }, "Network", olc::WHITE, { 0.4f,0.4f });

    if (MenuButton({ X,Y + 3 * Dy }, engine.GetMousePos(), { 55.5f, 10.f }, true)) {
        engine.DrawPartialDecal({ X,Y + 3 * Dy }, { 55.5,10 }, menudecals["MainMenuButtons"].get(), { 15,206 }, { 300,52 });
        if (engine.GetMouse(0).bPressed) engine.m_nGameMode = engine.MODE_OPTIONS_MENU;
    } else
        engine.DrawPartialDecal({ X,Y + 3 * Dy }, { 55.5,10 }, menudecals["MainMenuButtons"].get(), { 15,128 }, { 300,52 });
    engine.hud->DrawCenteredStringDecal({ X + 24,Y + 3 * Dy + 5 }, "Credits", olc::WHITE, { 0.4f,0.4f });

    if (MenuButton({ X,Y + 4 * Dy }, engine.GetMousePos(), { 55.5f, 10.f }, true)) {
        engine.DrawPartialDecal({ X,Y + 4 * Dy }, { 55.5,10 }, menudecals["MainMenuButtons"].get(), { 15,206 }, { 300,52 });
        if (engine.GetMouse(0).bPressed) return false;
    } else
        engine.DrawPartialDecal({ X,Y + 4 * Dy }, { 55.5,10 }, menudecals["MainMenuButtons"].get(), { 15,128 }, { 300,52 });
    engine.hud->DrawCenteredStringDecal({ X + 24,Y + 4 * Dy + 5 }, "Exit Game", olc::WHITE, { 0.4f,0.4f });


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
float MainMenu::Occialtions(float delta, float speed, float& timer, float& occilationvalue, float turnaround, bool& swap) {
    timer += delta;
    if (timer >= speed) {
        timer = 0.f;
        if (timer >= turnaround) {
            occilationvalue = turnaround;
            swap = true;
        }

        if (swap)
            occilationvalue -= speed;

        if (timer <= 0.f) {
            swap = false;
            occilationvalue = 0.f;
        }
        if (!swap)
            occilationvalue += speed;
    }

    return  (1.f - occilationvalue / turnaround);//return normalized value from 0->1
}

void MainMenu::SetupGame(float delta) {

}

void MainMenu::SettingMenu(float delta) {

}
