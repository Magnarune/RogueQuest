#include "UserInputManager.h"
#include "Engine.h"
UserInputManager::UserInputManager() {
}

UserInputManager::~UserInputManager() {
    build.reset();
    unit.reset();
}

void UserInputManager::StandardUserInput() {
    auto& engine = Game_Engine::Current();

    if (engine.GetMouse(0).bPressed) {
        Clicked = true;
        Initial = engine.tv.ScreenToWorld(engine.GetMousePos());
    }
    if (engine.GetMouse(0).bHeld) {
        Final = engine.tv.ScreenToWorld(engine.GetMousePos());
    }
    else {
        Clicked = false;
    }
    if (engine.GetMouse(0).bReleased) {
        if (!engine.GetKey(olc::SHIFT).bHeld && !engine.ActivityDone)
            engine.unitManager->DeselectUnits();

        (Initial - Final).mag2() > 16 && !engine.ActivityDone ?
            engine.unitManager->SelectUnits(Initial, Final) : engine.unitManager->SelectUnit(Final);

        if (!engine.ActivityDone)
            engine.buildingManager->DeselectBuildings();
        if (!engine.ActivityDone)
            engine.hudManager->selection = engine.hudManager->None;

        if (!engine.ActivityDone)
            engine.buildingManager->SelectBuilding(Final);
        else
            engine.ActivityDone = false;
    }

    if (engine.GetMouse(1).bPressed) {
        if (!engine.ActivityDone) {
            if (!engine.GetKey(olc::SHIFT).bHeld)
                engine.unitManager->StopUnits();
            DecideUnitAction();
            engine.buildingManager->SentUnitslocation(engine.tv.ScreenToWorld(engine.GetMousePos()));
        } else
            engine.ActivityDone = false;
    }
    if (engine.GetKey(olc::S).bPressed)
        engine.unitManager->StopUnits();
}

void UserInputManager::GetBuildModeUserInput() {
    auto& engine = Game_Engine::Current();
    if (engine.GetMouse(0).bPressed || engine.GetKey(olc::ESCAPE).bPressed)
        engine.hud->BuildMode = false;

    if (engine.GetMouse(1).bPressed) {
        if (!engine.GetKey(olc::SHIFT).bHeld)
            engine.unitManager->StopUnits();
        engine.ActivityDone = true;
        engine.hud->BuildMode=false;
        engine.hud->potBuilding->Position = engine.tv.ScreenToWorld(engine.GetMousePos()) - olc::vf2d(engine.hud->potBuilding->Size)/2.f;
        if(engine.unitManager->CheckBuildObstruction(engine.hud->potBuilding))
            engine.unitManager->DelegateTask("Build",
                std::make_pair(engine.hud->potBuilding, engine.tv.ScreenToWorld(engine.GetMousePos())));
    }
}

void UserInputManager::DecideUnitAction() {
    auto& engine = Game_Engine::Current();   
    engine.unitManager->CheckTaskAbility(engine.unitManager->FindObject(engine.tv.ScreenToWorld(engine.GetMousePos())),engine.GetKey(olc::A).bHeld);  
}

void UserInputManager::HeroUserInput() {
    auto& engine = Game_Engine::Current();
 
}

void UserInputManager::PrimarySelected() 
{
    auto& engine = Game_Engine::Current();

  /*  if (engine.leaders->LeaderList[1]->HeroDynamic)
    {
        auto& HERO = engine.leaders->LeaderList[1]->HomeHero;
        if (engine.GetKey(olc::Key::W).bHeld) HERO.lock()->Velocity += { 0.0f, -1.0f };
        if (engine.GetKey(olc::Key::S).bHeld) HERO.lock()->Velocity += { 0.0f, +1.0f };
        if (engine.GetKey(olc::Key::A).bHeld) HERO.lock()->Velocity += { -1.0f, 0.0f };
        if (engine.GetKey(olc::Key::D).bHeld) HERO.lock()->Velocity += { +1.0f, 0.0f };

        if (HERO.lock()->Velocity.mag2() > 0)
            HERO.lock()->Velocity = HERO.lock()->Velocity.norm() * (engine.GetKey(olc::Key::SHIFT).bHeld ? 2.0f : 1.0f);

    }*/
}




bool UserInputManager::Button(olc::vf2d pos, olc::vi2d Mouse, olc::vf2d Size) {
    auto& engine = Game_Engine::Current();
    if (engine.GetMouse(0).bPressed &&
        Mouse.x > pos.x && Mouse.y > pos.y &&
        Mouse.x < pos.x + Size.x && Mouse.y < pos.y + Size.y) {
        engine.ActivityDone = true;
        return true;
    }
    else
        return false;
}
bool UserInputManager::InsideBox(olc::vf2d pos, olc::vi2d Mouse, olc::vf2d Size) {
    if (Mouse.x > pos.x && Mouse.y > pos.y &&
        Mouse.x < pos.x + Size.x &&
        Mouse.y < pos.y + Size.y) {
        return true;
    }
    return false;
}
bool UserInputManager::Hover(olc::vf2d pos, olc::vi2d Mouse, olc::vf2d Size, float timer, float felapstedtime) {
    if (Mouse.x > pos.x && Mouse.y > pos.y &&
        Mouse.x < pos.x + Size.x &&
        Mouse.y < pos.y + Size.y) {
        triggertime += felapstedtime;
    } else {
        triggertime = 0.f;
    }

    if (triggertime > timer){
        return true;
    }
    return false;
}