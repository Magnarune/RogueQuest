#include "Options.h"
#include "Hud.h"
#include "Engine.h"
Options::Options() {
    auto& engine = Game_Engine::Current();
    engine.hudManager->ImportHudAssets();
}
Options::~Options()
{
}
void Options::MenuSelect() {
    auto& engine = Game_Engine::Current();
    if (engine.GetKey(olc::Key::ESCAPE).bPressed) { engine.m_nGameMode = engine.MODE_LOCAL_MAP; Button = Main_Options; }
    engine.DrawDecal({ 0,0 }, engine.hudManager->decals["Backround"].get());//BackRound
    if (engine.GetMousePos().x < 169.5f && engine.GetMousePos().y < 13.f && engine.GetMousePos().x > 90.f) {
        engine.DrawPartialDecal({ 90.f,0.f }, { 79.5,13 }, engine.hudManager->decals["Gui"].get(), { 15,206 }, { 300,52 });
        engine.DrawStringDecal({ 102.f,4.f }, "Back to Game", olc::WHITE, { 0.5,0.5 });
        if (engine.GetMouse(0).bPressed) { engine.m_nGameMode = engine.MODE_LOCAL_MAP; Button = Main_Options; }//Back to game Button This should be a function
    }
    else {
        engine.DrawPartialDecal({ 90.f,0.f }, { 79.5,13 }, engine.hudManager->decals["Gui"].get(), { 15,128 }, { 300,52 });
        engine.DrawStringDecal({ 102.f,4.f }, "Back to Game", olc::WHITE, { 0.5,0.5 });
    }

    switch (Button){
    case Main_Options:
        DrawMainMenu();
        break;
    case Sound_Options:
        DrawSoundMenu();
        break;
    case Game_Options:
        DrawGameMenu();
        break;
    default:
        break;
    }
}

void Options::DrawMainMenu(){
    auto& engine = Game_Engine::Current();

    if (engine.GetMousePos().x < 169.5f && engine.GetMousePos().y < 47.f && engine.GetMousePos().x > 90.f && engine.GetMousePos().y > 30.f) {
        engine.DrawPartialDecal({ 90.f,30.f }, { 79.5,13 }, engine.hudManager->decals["Gui"].get(), { 15,206 }, { 300,52 });
        engine.DrawStringDecal({ 102.f,34.f }, "Sound Options", olc::WHITE, { 0.5,0.5 });
        if (engine.GetMouse(0).bPressed) Button = Sound_Options;//Back to game Button This should be a function
    }
    else {
        engine.DrawPartialDecal({ 90.f,30.f }, { 79.5,13 }, engine.hudManager->decals["Gui"].get(), { 15,128 }, { 300,52 });
        engine.DrawStringDecal({ 102.f,34.f }, "Sound Options", olc::WHITE, { 0.5,0.5 });
    }
    if (engine.GetMousePos().x < 169.5f && engine.GetMousePos().y < 75.f && engine.GetMousePos().x > 90.f && engine.GetMousePos().y > 60) {
        engine.DrawPartialDecal({ 90.f,60.f }, { 79.5,13 }, engine.hudManager->decals["Gui"].get(), { 15,206 }, { 300,52 });
        engine.DrawStringDecal({ 102.f,64.f }, "Game Options", olc::WHITE, { 0.5,0.5 });
        if (engine.GetMouse(0).bPressed) Button =Game_Options;//Back to game Button This should be a function
    }
    else {
        engine.DrawPartialDecal({ 90.f,60.f }, { 79.5,13 }, engine.hudManager->decals["Gui"].get(), { 15,128 }, { 300,52 });
        engine.DrawStringDecal({ 102.f,64.f }, "Game Options", olc::WHITE, { 0.5,0.5 });
    }
    
}

void Options::DrawSoundMenu(){
    auto& engine = Game_Engine::Current();
   engine.DrawStringDecal({ (float)engine.ScreenWidth() / 4.f, (float)engine.ScreenHeight() / 2.f }, "Not Added Yet!");
}

void Options::DrawGameMenu(){
    auto& engine = Game_Engine::Current();

    engine.DrawStringDecal({ 6.f,21.f }, "Auto-Enemies?", olc::WHITE, { 0.4f,0.4f });
    engine.DrawPartialDecal({ 9.f,30.f }, { 5,5 }, engine.hudManager->decals["Gui"].get(), { 152,17 }, { 17,17 });
    if (engine.GetMousePos().x > 9 && engine.GetMousePos().y > 30 && engine.GetMousePos().x < 14 && engine.GetMousePos().y < 35) {
        if (engine.GetMouse(0).bPressed)   engine.bevil = !engine.bevil;
    }
    if (engine.bevil)
        engine.DrawPartialDecal({ 9.f,30.f }, { 5,5 }, engine.hudManager->decals["Gui"].get(), { 152,61 }, { 17,17 });
}

