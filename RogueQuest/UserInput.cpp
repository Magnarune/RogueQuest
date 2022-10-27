#include "UserInput.h"
#include "Engine.h"

UserInput::UserInput() {
}
UserInput::~UserInput() {
}

void UserInput::CameraInput(){
    auto& engine = Game_Engine::Current();
    CenterofScreen = engine.tv.ScaleToWorld(olc::vf2d(float(engine.ScreenWidth()) / 2.0f, float(engine.ScreenHeight()) / 2.0f));//Center of screen
    
    engine.Camera.vVel = { 0.f,0.f };//Set Camera velocity to 0
    if (engine.tv.GetWorldScale().x <= 6.f)//Max Zoom in
        if (engine.GetMouseWheel() > 0)    engine.tv.ZoomAtScreenPos(1.1f, engine.GetMousePos());//Middle Mouse zoom
    if (engine.tv.GetWorldScale().x >= 0.3f)//Max Zoom out
        if (engine.GetMouseWheel() < 0)    engine.tv.ZoomAtScreenPos(0.9f, engine.GetMousePos());

    if (engine.GetMouse(2).bPressed)   engine.tv.StartPan(engine.GetMousePos());//If you Click with Middle mouse you can grab screen and pan
    if (engine.GetMouse(2).bHeld)      engine.tv.UpdatePan(engine.GetMousePos());
    if (engine.GetMouse(2).bReleased)  engine.tv.EndPan(engine.GetMousePos());

    engine.Camera.vPOS = engine.tv.GetWorldOffset() + CenterofScreen;//Set camera pos.

    if (engine.tv.GetWorldTL().x < 0) 
        engine.Camera.vPOS.x += abs(engine.tv.GetWorldTL().x);//You cant look above the map into the void
    if (engine.tv.GetWorldTL().y < 0) 
        engine.Camera.vPOS.y += abs(engine.tv.GetWorldTL().y);   //You cant look above the map into the void

    if (engine.tv.GetWorldBR().x > engine.worldManager->curMap().layerSize.x * 32)
        engine.Camera.vPOS.x -= engine.tv.GetWorldBR().x - engine.worldManager->curMap().layerSize.x * 32;    //You cant look above the map into the void
    if (engine.tv.GetWorldBR().y > engine.worldManager->curMap().layerSize.y * 32) 
        engine.Camera.vPOS.y -= engine.tv.GetWorldBR().y - engine.worldManager->curMap().layerSize.y * 32;    //You cant look above the map into the void
    
    if (engine.GetLocked()) {
        if (engine.GetMousePos().x <= 2) { engine.Camera.vVel.x -= 1.0f / engine.tv.GetWorldScale().x; }//If you move the mouse to the edge of screen it pans for you
        if (engine.GetMousePos().y <= 2) { engine.Camera.vVel.y -= 1.0f / engine.tv.GetWorldScale().x; }
        if (engine.GetMousePos().x >= engine.ScreenWidth() - 3) { engine.Camera.vVel.x += 1.0f / engine.tv.GetWorldScale().x; }
        if (engine.GetMousePos().y >= engine.ScreenHeight() - 3) { engine.Camera.vVel.y += 1.0f / engine.tv.GetWorldScale().x; }
        engine.Camera.vPOS += engine.Camera.vVel * 4.0f;//speed == 4.f prob could be changed here
        engine.tv.SetWorldOffset(engine.Camera.vPOS - CenterofScreen);//SET WORLD OFFSET TO CAM POS
    }
    engine.tv.SetWorldOffset(engine.Camera.vPOS - CenterofScreen);//SET WORLD OFFSET TO CAM POS
    

}
//Top Left == pos
void UserInput::MiniMapBox() {
    auto& engine = Game_Engine::Current();
    //This right here is for if you have a mini map and want to move the screen based on the mini map box
    

    olc::vi2d mpos = engine.GetMousePos();
    olc::vf2d pos = engine.hud->minimapPosition;  
    olc::vf2d size = engine.hud->minimapSize;
    if (engine.GetMouse(1).bHeld && mpos.x > pos.x && mpos.y > pos.y && mpos.x < size.x+pos.x && mpos.y < size.y+pos.y) {
        engine.ActivityDone = true;//Don't unselect units just be cause you use minimap
        olc::vf2d MouseNorm = { ((float)mpos.x -pos.x) / size.x, ((float)mpos.y - (float)pos.y) /size.y};

        engine.Camera.vPOS = { (MouseNorm.x * (float)engine.worldManager->curMap().layerSize.x * 32.f) - float(engine.ScreenWidth() / 2.f),(MouseNorm.y * (float)engine.worldManager->curMap().layerSize.y * 32.f) - float(engine.ScreenHeight() / 2.f) };//Set Cam pos
        engine.tv.SetWorldOffset(engine.Camera.vPOS - CenterofScreen);

    }


    //if (engine.GetMouse(1).bHeld && engine.GetMousePos().x > 11 &&
    //    engine.GetMousePos().y > (float)engine.ScreenHeight() * 0.823f - 10.f
    //    && engine.GetMousePos().x < 57
    //    && engine.GetMousePos().y < (float)engine.ScreenHeight() * 0.823f + 36.f)
    //{//If your Right Click && mouse box is in mini map box
    //    olc::vf2d MouseNorm = { ((float)engine.GetMousePos().x - 6.f) / 46.f, ((float)engine.GetMousePos().y - (float)engine.ScreenHeight() * 0.823f + 15.f) / (46.f) };//Normilze full map coord
    //   
    //    engine.Camera.vPOS = { (MouseNorm.x * (float)engine.worldManager->curMap().layerSize.x * 32.f) - float(engine.ScreenWidth() / 2.f),(MouseNorm.y * (float)engine.worldManager->curMap().layerSize.y * 32.f) - float(engine.ScreenHeight() / 2.f) };//Set Cam pos
    //    engine.tv.SetWorldOffset(engine.Camera.vPOS - CenterofScreen);//Set WORLD OFFSET TO CAM
    //}
}


void UserInput::GetUserInput() {
    auto& engine = Game_Engine::Current();
    if (!engine.IsConsoleShowing()) {
        if (engine.IsFocused()) {
            CameraInput();
            MiniMapBox();
            if (engine.GetKey(olc::ESCAPE).bPressed)
                engine.m_nGameMode = engine.MODE_OPTIONS_MENU;
            if (engine.GetKey(olc::F1).bPressed) {
                if (!engine.leaders->LeaderList[1]->HomeBase.expired()) {
                    engine.unitManager->DeselectUnits();
                    engine.buildingManager->DeselectBuildings();
                    engine.buildingManager->SelectBuilding(engine.leaders->LeaderList[1]->HomeBase.lock()->Position);
                }
            }



            if ((engine.GetMouse(1).bPressed || engine.GetMouse(0).bPressed || engine.GetMouse(0).bReleased) && engine.GetMousePos().y > (float)engine.ScreenHeight() * 0.75f)
                engine.ActivityDone = true;


            if (engine.hud->BuildMode)
                engine.inputmanager->GetBuildModeUserInput();
            engine.inputmanager->StandardUserInput();
          
        }
    }

}

void UserInput::DrawUserInput() {
    auto& engine = Game_Engine::Current();
    if (!engine.IsConsoleShowing()) {
        if (engine.inputmanager->Clicked){
            olc::Pixel color = olc::RED;
            engine.tv.DrawLineDecal(engine.inputmanager->Final,   { engine.inputmanager->Initial.x,engine.inputmanager->Final.y }, color);
            engine.tv.DrawLineDecal(engine.inputmanager->Initial, { engine.inputmanager->Initial.x,engine.inputmanager->Final.y }, color);
            engine.tv.DrawLineDecal(engine.inputmanager->Initial, { engine.inputmanager->Final.x,engine.inputmanager->Initial.y }, color);
            engine.tv.DrawLineDecal(engine.inputmanager->Final,   { engine.inputmanager->Final.x,engine.inputmanager->Initial.y }, color);
        }
    }
}
