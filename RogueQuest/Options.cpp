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
    engine.hudManager->ImportHudAssets();

    mainMenu["btnExit"] = Option(Option::Button, "Exit", [&](Option& opt){
        // exit game
    }, olc::vf2d(90.f, 120.f));
    
    optionsMenu["chkEvil"] = Option(Option::Checkbox, "Is Evil (debug)", [&](Option& opt){
        auto& engine = Game_Engine::Current();
        bool checked = std::get<bool>(opt.value);
        engine.config->SaveValue("Evil", checked);
    }, olc::vf2d(9.f,30.f), engine.config->GetValue<bool>("Evil"));

    optionsMenu["ScreenLocked"] = Option(Option::Checkbox, "Is screen Locked", [&](Option& opt) {
        auto& engine = Game_Engine::Current();
        bool checked = std::get<bool>(opt.value);
        engine.SetLocked(checked);
        }, olc::vf2d(9.f, 60.f), engine.config->GetValue<bool>("ScreenLocked"));



    mainMenu["btnGameOpt"] = Option(Option::Button, "Game Options", [&](Option& opt){
        SetGuiMenu("OptionsMenu");
    }, {90.f,60.f});

    mainMenu["btnSoundOpt"] = Option(Option::Button, "Sound Options", [&](Option& opt){
        SetGuiMenu("SoundMenu");
    }, {90.f,30.f});

    soundMenu["lblNotDoneYet"] = Option(Option::Label, "Not Added Yet!", [&](Option& opt){},
        { (float)engine.ScreenWidth() / 4.f, (float)engine.ScreenHeight() / 2.f });
}

Options::~Options(){
}

void Options::MenuSelect() {
    auto& engine = Game_Engine::Current();
    if (engine.GetKey(olc::Key::ESCAPE).bPressed) { engine.m_nGameMode = engine.MODE_LOCAL_MAP; SetGuiMenu("MainMenu"); }
    engine.DrawDecal({ 0,0 }, engine.hudManager->decals["Background"].get());//Background

    if (engine.GetMousePos().x < 169.5f && engine.GetMousePos().y < 13.f && engine.GetMousePos().x > 90.f) {
        engine.DrawPartialDecal({ 90.f,0.f }, { 79.5,13 }, engine.hudManager->decals["Gui"].get(), { 15,206 }, { 300,52 });
        engine.DrawStringDecal({ 102.f,4.f }, "Back to Game", olc::WHITE, { 0.5,0.5 });
        if (engine.GetMouse(0).bPressed) { engine.m_nGameMode = engine.MODE_LOCAL_MAP; SetGuiMenu("MainMenu"); }//Back to game Button This should be a function
    }
    else {
        engine.DrawPartialDecal({ 90.f,0.f }, { 79.5,13 }, engine.hudManager->decals["Gui"].get(), { 15,128 }, { 300,52 });
        engine.DrawStringDecal({ 102.f,4.f }, "Back to Game", olc::WHITE, { 0.5,0.5 });
    }

    if(currentMenu != nullptr){
        DrawGui(*currentMenu); // draw what the current options list is
    }
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
                engine.DrawPartialDecal(option.position, { 5,5 }, engine.hudManager->decals["Gui"].get(), { 152.f,17.f }, { 17.f,17.f });
                if(std::get<bool>(option.value))
                    engine.DrawPartialDecal(option.position, { 5,5 }, engine.hudManager->decals["Gui"].get(), { 152.f,61.f }, { 17.f,17.f });
                engine.DrawStringDecal(option.position - olc::vf2d({ 16.f, 12.f }), option.label, olc::WHITE, {0.4f,0.4f});
                localchange = [&](){ option.value = !std::get<bool>(option.value); };
            break;}
            case Option::Button:{
                olc::vf2d src = hover ? olc::vf2d(15.f,206.f) : olc::vf2d(15.f,128.f);
                engine.DrawPartialDecal(option.position, size, engine.hudManager->decals["Gui"].get(), src, { 300,52 });
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