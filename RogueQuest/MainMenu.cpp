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
    

}



void MainMenu::MainMenuSelection(float delta) {
	switch (MainMenuSelect) {
	case None:
		StartMenu(delta);
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
}

void MainMenu::StartMenu(float delta) {
    auto& engine = Game_Engine::Current();
    engine.DrawPartialDecal({ 0.f,0.f }, { (float)engine.ScreenWidth(),(float)engine.ScreenHeight() }, menudecals["MainMenuBackround"].get(), { 0.f,0.f }, { 1200.f,1500.f });


    if (engine.GetKey(olc::Key::ESCAPE).bPressed) { engine.m_nGameMode = engine.MODE_LOCAL_MAP; engine.optionsManager->SetGuiMenu("MainMenu"); }







}


void  MainMenu::CreateGame() {

}

void MainMenu::ExitProgram() {

}

void MainMenu::SetupGame(float delta) {

}

void MainMenu::SettingMenu(float delta) {

}
