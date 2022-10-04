#include "UserInputManager.h"
#include "Engine.h"
UserInputManager::UserInputManager() {
}

UserInputManager::~UserInputManager() {
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

        (Initial - Final).mag2() > 16 ?
            engine.unitManager->SelectUnits(Initial, Final) : engine.unitManager->SelectUnit(Final);

        if (!engine.ActivityDone)
            engine.buildingManager->DeselectBuildings();
        else
            engine.ActivityDone = false;

        engine.buildingManager->SelectBuilding(Final);
    }
    if (engine.GetMouse(1).bPressed) {
        if (!engine.GetKey(olc::SHIFT).bHeld)
            engine.unitManager->StopUnits();
        // this is what delegating a task will look similar to
        engine.unitManager->DelegateTask("Move",
            std::make_pair(engine.tv.ScreenToWorld(engine.GetMousePos()), engine.GetKey(olc::Key::A).bHeld));

        //engine.unitManager->MoveUnits(engine.tv.ScreenToWorld(engine.GetMousePos()), engine.GetKey(olc::Key::A).bHeld);
        engine.buildingManager->SentUnitslocation(engine.tv.ScreenToWorld(engine.GetMousePos()));
    }
    if (engine.GetKey(olc::S).bPressed)
        engine.unitManager->StopUnits();

}














void UserInputManager::GetBuildModeUserInput() {
    auto& engine = Game_Engine::Current();
    if (engine.GetMouse(0).bPressed) {
        std::shared_ptr<Unit> unit = engine.unitManager->GetUnit();
        engine.hud->BuildMode = false;
        engine.unitManager->StopUnits();
        //  unit->ConstructBuilding(engine.hud->Object, engine.tv.ScreenToWorld(engine.GetMousePos()));
    }

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