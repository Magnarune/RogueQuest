#include"MainMenu.h"
#include"Engine.h"
MainMenu::MainMenu() {

}

MainMenu::~MainMenu() {

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


}


void  MainMenu::CreateGame() {

}

void MainMenu::ExitProgram() {

}

void MainMenu::SetupGame(float delta) {

}

void MainMenu::SettingMenu(float delta) {

}
