#include "Options.h"
#include "Hud.h"
#include "Engine.h"


Options::Options() {
    menus = {
        {"MainMenu", &mainMenu},
        {"SoundMenu", &soundMenu},
        {"GameMenu", &gameMenu},
        {"OptionsMenu", &optionsMenu},
    };


    auto& engine = Game_Engine::Current();
    float X = (float)engine.ScreenWidth() * 0.4125f;
    float Y = (float)engine.ScreenHeight() * .014815f;

    engine.hud->ImportHudAssets();

    optionsMenu["chkEvil"] = Option(Option::Checkbox, "Is Evil (debug)", [&](Option& opt){
        auto& engine = Game_Engine::Current();
        bool checked = std::get<bool>(opt.value);
        engine.config->SaveValue("Evil", checked);
    }, olc::vf2d(9.f,30.f), engine.config->GetValue<bool>("Evil"));

    //            name of option          option type      label/string desc
    optionsMenu["ScreenLocked"] = Option(Option::Checkbox, "Is screen Locked",
    //  lambda - callback onChange
        [&](Option& opt) {  //So after the mouse pressed 
            auto& engine = Game_Engine::Current();
            // get the current state of the option - here it's a checkbox so a boolean value is our state type
            bool checked = std::get<bool>(opt.value);
            // no when is the Option "checked" changed from old value to new
            engine.SetLocked(checked);
        },
        
        // Position on screen top left           Default option starting value
        olc::vf2d(9.f, 60.f), engine.config->GetValue<bool>("ScreenLocked"));

    mainMenu["btnExit"] = Option(Option::Button, " Exit Game", [&](Option& opt){
        // exit game
        Exit_Game = false;
    }, olc::vf2d(X, 120.f));
    

    mainMenu["btnGameOpt"] = Option(Option::Button, "Main Menu", [&](Option& opt) {
        engine.mainmenu->GameStarted = false;
        engine.m_nGameMode = engine.MODE_MAIN_MENU;

        },  { X, Y+90.f });

    mainMenu["btnGameOpt"] = Option(Option::Button, "Game Options", [&](Option& opt){
        SetGuiMenu("OptionsMenu");
    }, {X,Y+60.f});

    mainMenu["btnSoundOpt"] = Option(Option::Button, "Sound Options", [&](Option& opt){
        SetGuiMenu("SoundMenu");
    }, {X,Y+30.f});

    soundMenu["lblNotDoneYet"] = Option(Option::Label, "Not Added Yet!", [&](Option& opt){},
        { (float)engine.ScreenWidth() / 4.f, (float)engine.ScreenHeight() / 2.f });
}

Options::~Options(){
}

bool Options::MenuSelect() {
    auto& engine = Game_Engine::Current();
    float X = (float)engine.ScreenWidth()* 0.4125f;
    float Y = (float)engine.ScreenHeight() * .014815f;

    if (engine.GetKey(olc::Key::ESCAPE).bPressed) {
        if (!engine.mainmenu->GameStarted)
            engine.m_nGameMode = engine.MODE_MAIN_MENU;
        else engine.m_nGameMode = engine.MODE_LOCAL_MAP;
        SetGuiMenu("MainMenu");
    }
    engine.DrawDecal({ 0,0 }, engine.hud->decals["Background"].get());//Background
    engine.DrawStringDecal(engine.GetMousePos() - olc::vi2d(-5, -20), std::to_string((float)engine.GetMousePos().x / (float)engine.ScreenWidth()) + "/" + std::to_string((float)engine.GetMousePos().y / (float)engine.ScreenHeight()));

    if (engine.GetMousePos().x <X+70.f && engine.GetMousePos().y < 13.f && engine.GetMousePos().x > X) {
        engine.DrawPartialDecal({ X,0.f }, { 79.5,13 }, engine.hud->decals["Gui"].get(), { 15,206 }, { 300,52 });
        engine.DrawStringDecal({ X+17,4.f }, "Back", olc::WHITE, { 0.5,0.5 });
        if (engine.GetMouse(0).bPressed) { 
            if (!engine.mainmenu->GameStarted) 
                engine.m_nGameMode = engine.MODE_MAIN_MENU;
            else engine.m_nGameMode = engine.MODE_LOCAL_MAP; 
            SetGuiMenu("MainMenu");
        }
    }
    else {
        engine.DrawPartialDecal({ X,0.f }, { 79.5,13 }, engine.hud->decals["Gui"].get(), { 15,128 }, { 300,52 });
        engine.DrawStringDecal({  X+12.f,4.f }, "Back to Game", olc::WHITE, { 0.5,0.5 });
    }
    
    if(currentMenu != nullptr){
        DrawGui(*currentMenu); // draw what the current options list is
    }
    return Exit_Game;
}

void Options::DrawGui(OptionsList& options){
    auto& engine = Game_Engine::Current();

    for(auto& [name, option] : options) {
        
        static const std::map<Option::GuiType,olc::vf2d> sizeChart { // gui element sizes go here for now :)
            {Option::Checkbox,{8.f, 8.f}},
            {Option::Button,{79.5f, 15.f}}
        };

        olc::vf2d size {0.f, 0.f};
        if(sizeChart.count(option.type)) size = sizeChart.at(option.type);

        olc::vf2d mpos = olc::vf2d(engine.GetMousePos());
        std::function<void()> localchange = [](){};
        bool hover = (mpos.x >= option.position.x && mpos.y >= option.position.y && mpos.x < option.position.x + size.x && mpos.y < option.position.y + size.y);

        switch(option.type){
            case Option::Checkbox:{
                engine.DrawPartialDecal(option.position, { 5,5 }, engine.hud->decals["Gui"].get(), { 152.f,17.f }, { 17.f,17.f });
                if(std::get<bool>(option.value))
                    engine.DrawPartialDecal(option.position, { 5,5 }, engine.hud->decals["Gui"].get(), { 152.f,61.f }, { 17.f,17.f });
                engine.DrawStringDecal(option.position - olc::vf2d({ 16.f, 12.f }), option.label, olc::WHITE, {0.4f,0.4f});
                localchange = [&](){ option.value = !std::get<bool>(option.value); };//Where Option gets altered
            break;}
            case Option::Button:{
                olc::vf2d src = hover ? olc::vf2d(15.f,206.f) : olc::vf2d(15.f,128.f);
                engine.DrawPartialDecal(option.position, size, engine.hud->decals["Gui"].get(), src, { 300,52 });
                engine.DrawStringDecal(option.position + olc::vf2d(12.f, 4.f), option.label, olc::WHITE, { 0.5,0.5 });
            break;}
            case Option::Label:{
                engine.DrawStringDecal(option.position, option.label, olc::WHITE); // eh.. can make it configurable later
                hover = 0;
            break;}
        }
        // hmm .. temporarily .. but should be moved
        if(engine.GetMouse(0).bPressed && hover){
            localchange(); // change any value if necessary
            option.callback(option);
        }
    }

}



/* Option Ctor */

Options::Option::Option(Options::Option::GuiType t, const std::string& l, Options::Option::CallbackType cb, olc::vf2d pos, Options::Option::ValueType def):
    type(t), label(l), callback(cb), position(pos), value(def) {}