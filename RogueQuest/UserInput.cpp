#include "UserInput.h"
#include "Engine.h"

UserInput::UserInput() {
}
UserInput::~UserInput() {
}

void UserInput::GetUserInput() {
	auto& engine = Game_Engine::Current();
    engine.Camera.vVel = { 0.f,0.f };
    if (engine.tv.GetWorldScale().x <= 6.f)//Max Zoom in
        if (engine.GetMouseWheel() > 0)    engine.tv.ZoomAtScreenPos(1.1f, engine.GetMousePos());
    if (engine.tv.GetWorldScale().x >= 0.3f)//Max Zoom out
        if (engine.GetMouseWheel() < 0)    engine.tv.ZoomAtScreenPos(0.9f, engine.GetMousePos());


    if (engine.GetMouse(2).bPressed)   engine.tv.StartPan(engine.GetMousePos());
    if (engine.GetMouse(2).bHeld)      engine.tv.UpdatePan(engine.GetMousePos());
    if (engine.GetMouse(2).bReleased)  engine.tv.EndPan(engine.GetMousePos());


    engine.Camera.vPOS = engine.tv.GetWorldOffset() + engine.tv.ScaleToWorld(olc::vf2d(float(engine.ScreenWidth()) / 2.0f, float(engine.ScreenHeight()) / 2.0f));

    if (engine.tv.GetWorldTL().x < 0) {
        engine.Camera.vPOS.x += abs(engine.tv.GetWorldTL().x);
    }
    if (engine.tv.GetWorldTL().y < 0) {
        engine.Camera.vPOS.y += abs(engine.tv.GetWorldTL().y);
    }
    if (engine.tv.GetWorldBR().x > engine.worldManager->curMap().layerSize.x * 32) {
        engine.Camera.vPOS.x -= engine.tv.GetWorldBR().x - engine.worldManager->curMap().layerSize.x * 32;
    }
    if (engine.tv.GetWorldBR().y > engine.worldManager->curMap().layerSize.y * 32) {
        engine.Camera.vPOS.y -= engine.tv.GetWorldBR().y - engine.worldManager->curMap().layerSize.y * 32;
    }

    if (engine.GetKey(olc::ESCAPE).bPressed) {
        engine.SetLocked(!engine.GetLocked());
    }

    if (engine.GetLocked()) {
        if (engine.GetMousePos().x <= 2) { engine.Camera.vVel.x -= 1.0f / engine.tv.GetWorldScale().x; }
        if (engine.GetMousePos().y <= 2) { engine.Camera.vVel.y -= 1.0f / engine.tv.GetWorldScale().x; }
        if (engine.GetMousePos().x >= engine.ScreenWidth() - 3) { engine.Camera.vVel.x += 1.0f / engine.tv.GetWorldScale().x; }
        if (engine.GetMousePos().y >= engine.ScreenHeight() - 3) { engine.Camera.vVel.y += 1.0f / engine.tv.GetWorldScale().x; }
        engine.Camera.vPOS += engine.Camera.vVel * 4.0f;

        engine.tv.SetWorldOffset(engine.Camera.vPOS - engine.tv.ScaleToWorld(olc::vf2d(float(engine.ScreenWidth()) / 2.0f, float(engine.ScreenHeight()) / 2.0f)));
    }

    engine.tv.SetWorldOffset(engine.Camera.vPOS - engine.tv.ScaleToWorld(olc::vf2d(float(engine.ScreenWidth()) / 2.0f, float(engine.ScreenHeight()) / 2.0f)));
    if (engine.GetMouse(1).bHeld && engine.GetMousePos().x > 11 && engine.GetMousePos().y > (float)engine.ScreenHeight() * 0.823f - 10.f && engine.GetMousePos().x < 57 && engine.GetMousePos().y < (float)engine.ScreenHeight() * 0.823f + 36.f) {
        olc::vf2d MouseNorm = { ((float)engine.GetMousePos().x - 6.f) / 46.f, ((float)engine.GetMousePos().y - (float)engine.ScreenHeight() * 0.823f + 15.f) / (46.f) };

        engine.Camera.vPOS = { (MouseNorm.x * (float)engine.worldManager->curMap().layerSize.x * 32.f) - float(engine.ScreenWidth() / 2.f),(MouseNorm.y * (float)engine.worldManager->curMap().layerSize.y * 32.f) - float(engine.ScreenHeight() / 2.f) };
        engine.tv.SetWorldOffset(engine.Camera.vPOS - engine.tv.ScaleToWorld(olc::vf2d(float(engine.ScreenWidth()) / 2.0f, float(engine.ScreenHeight()) / 2.0f)));
    }
    if (!engine.IsConsoleShowing()) {
        if (engine.IsFocused()) {
            if (engine.GetMouse(0).bPressed){
                Clicked = true;
                Initial = engine.tv.ScreenToWorld(engine.GetMousePos());
            }
            if (engine.GetMouse(0).bHeld){
                Final = engine.tv.ScreenToWorld(engine.GetMousePos());
            } else {
                Clicked = false;
            }
            if (engine.GetMouse(0).bReleased){
                if(!engine.GetKey(olc::SHIFT).bHeld) // if not holding shift key (otherwise add to selection)
                    engine.unitManager->DeselectUnits(); // deselect units before new selection
                (Initial - Final).mag2() > 16 ? // if larger than 4 px is selected
                    engine.unitManager->SelectUnits(Initial, Final) : engine.unitManager->SelectUnit(Final);
            }
            if (engine.GetMouse(1).bPressed){
                if(!engine.GetKey(olc::SHIFT).bHeld)
                    engine.unitManager->StopUnits();
                engine.unitManager->MoveUnits(engine.tv.ScreenToWorld(engine.GetMousePos()), engine.GetKey(olc::Key::A).bHeld);
            }
            if (engine.GetKey(olc::S).bPressed)
                engine.unitManager->StopUnits();
        }
    }
}

void UserInput::DrawUserInput() {
    auto& engine = Game_Engine::Current();
    if (!engine.IsConsoleShowing()) {
        if (Clicked){
            olc::Pixel color = olc::RED;
            engine.tv.DrawLineDecal(Final,   { Initial.x,Final.y }, color);//Draw Rectangle
            engine.tv.DrawLineDecal(Initial, { Initial.x,Final.y }, color);
            engine.tv.DrawLineDecal(Initial, { Final.x,Initial.y }, color);
            engine.tv.DrawLineDecal(Final,   { Final.x,Initial.y }, color);
        }
    }
}