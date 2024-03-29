#include "Assets.h"
#include "Engine.h"
cAssets::cAssets() {
    lua_state.open_libraries(sol::lib::base);
    lua_state.open_libraries(sol::lib::math);
    lua_state.open_libraries(sol::lib::package);
    lua_state.open_libraries(sol::lib::string);
}
cAssets::~cAssets(){
    
}

void cAssets::LoadUnitAssets(){
    auto& engine = Game_Engine::Current();
    sol::table UnitData, FileSets, UnitStats;

    auto to_vi2d = [](sol::table obj) -> olc::vi2d {
        int32_t x = obj[1],
                y = obj[2];
        return {x, y};
    };
    const std::vector<std::string> filePaths = { //
        "Assets/Units/Spider/Spider.lua",
        "Assets/Units/Mage/Mage.lua",
        "Assets/Units/Goblin/goblin.lua",
        "Assets/Units/Imp/imp.lua",
        "Assets/Units/Sara/sara.lua",
        "Assets/Units/Builder/builder.lua",
        "Assets/Units/Archer/Archer.lua"
    };

    /*
        // To Do: Load all files in directory

    using fs = std::filesystem;

    fs::directory_iterator dir;
    */
    

    for (const auto& path : filePaths) {
        try {
            sol::load_result script = lua_state.load_file(path);

            sol::protected_function_result rcode = script();

            if (!rcode.valid()) {
                sol::error e = rcode;
                std::cout << "error: " << e.what() << "\n";
            } else {
                UnitData = rcode;
            }

            std::string name = UnitData["Name"];
            UnitType::SoundMetaData  sMeta = {};
            UnitType::TextureMetaData meta;



            UnitType unitType;
            for (int i = 1; i < 5; i++)
                meta.Sprite_Order.push_back(UnitData["SpriteOrder"][i]);//Im in danger

            if (UnitData["Description"] != sol::nil) {
                unitType.Description = UnitData["Description"];
            } else
                unitType.Description = "Not Added For this Unit";



            FileSets = UnitData["Files"];
            for (int i = 0; i < FileSets.size(); ++i) {
                sol::table fileset = FileSets[i + 1];
                // load the Name : TextureID for the unitType
                std::string name = fileset["Name"], path = fileset["FileName"];

                meta.tex_id = TextureCache::GetCache().CreateTexture(path);
                meta.ani_len = sol::object(fileset["AnimationLength"]).as<int>();
                meta.sprite_size = to_vi2d(fileset["SpriteSize"]);
                meta.tile_size = to_vi2d(fileset["TileSize"]);
                meta.draw_origin = olc::vf2d(to_vi2d(fileset["Origin"]));

                meta.target_size = to_vi2d(fileset["TargetSize"]);

                meta.scale = olc::vf2d(meta.target_size) / olc::vf2d(meta.tile_size);

                // load head location data into the texture meta data
                if (fileset["HeadImage"] != sol::nil) {
                    unitType.head.tl = to_vi2d(fileset["HeadImage"]["tl"]);
                    unitType.head.sz = to_vi2d(fileset["HeadImage"]["size"]);
                    unitType.head.tex_id = meta.tex_id;
                }

                unitType.texture_metadata.insert_or_assign(name, std::move(meta));
            }
            //Sound uplink          
            if (UnitData["Sounds"] != sol::nil) {
                sol::table soundlist = UnitData["Sounds"];
                for (const auto& kv : soundlist) {
                    sol::table list = kv.second;
                    std::string category_name = kv.first.as<std::string>();

                    for (const auto& snd : list) {
                        engine.soundmanager->LoadAudioFile(name,category_name, snd.second.as<std::string>());
                    }
                }
            }
            unitType.Cost = UnitData["Parameters"]["Cost"];
            unitType.Food = UnitData["Parameters"]["Food"];
            sol::table Requirements = UnitData["Parameters"]["Requirements"];
            for (int i = 0; i < Requirements.size(); i++){
                unitType.Requirements.push_back(Requirements[i+1]);
            }
            if(UnitData["Parameters"] != sol::nil &&
               UnitData["Parameters"]["Abilities"] != sol::nil &&
               UnitData["Parameters"]["Abilities"]["Tasks"] != sol::nil){
                sol::table tasks = UnitData["Parameters"]["Abilities"]["Tasks"];
                for(int i = 0; i < tasks.size(); ++i){
                    unitType.task_abilities.emplace_back(tasks[i + 1].get<std::string>());
                }
            }
            if (UnitData["Parameters"]["Ranged"] == true)
                unitType.projectileName = UnitData["Parameters"]["Projectile"];

            unitType.lua_data = std::move(UnitData); // UnitData is empty now
            
            unitCache.insert_or_assign(name, std::move(unitType));//if you say unit you get Full table!
        } catch(std::exception e) {
            std::cerr << e.what() << "\n";
        }
    }
    
}

void cAssets::LoadHeroAssets() {
    sol::table UnitData, FileSets, UnitStats;

    auto to_vi2d = [](sol::table obj) -> olc::vi2d {
        int32_t x = obj[1],
            y = obj[2];
        return { x, y };
    };
    const std::vector<std::string> filePaths = {
        "Assets/Units/Hero_Units/Magnus.lua"
    };

    for (const auto& path : filePaths) {
        try {
            sol::load_result script = lua_state.load_file(path);

            sol::protected_function_result rcode = script();

            if (!rcode.valid()) {
                sol::error e = rcode;
                std::cout << "error: " << e.what() << "\n";
            } else {
                UnitData = rcode;
            }

            std::string name = UnitData["Name"];

            UnitType::TextureMetaData meta;
            UnitType unitType;
            for (int i = 1; i < 5; i++)
                meta.Sprite_Order.push_back(UnitData["SpriteOrder"][i]);//Im in danger

            if (UnitData["Description"] != sol::nil) {
                unitType.Description = UnitData["Description"];
            } else
                unitType.Description = "Not Added For this Unit";



            FileSets = UnitData["Files"];
            for (int i = 0; i < FileSets.size(); ++i) {
                sol::table fileset = FileSets[i + 1];
                // load the Name : TextureID for the unitType
                std::string name = fileset["Name"], path = fileset["FileName"];

                meta.tex_id = TextureCache::GetCache().CreateTexture(path);
                meta.ani_len = sol::object(fileset["AnimationLength"]).as<int>();
                meta.sprite_size = to_vi2d(fileset["SpriteSize"]);
                meta.tile_size = to_vi2d(fileset["TileSize"]);
                meta.draw_origin = olc::vf2d(to_vi2d(fileset["Origin"]));

                meta.target_size = to_vi2d(fileset["TargetSize"]);

                meta.scale = olc::vf2d(meta.target_size) / olc::vf2d(meta.tile_size);

                // load head location data into the texture meta data
                if (fileset["HeadImage"] != sol::nil) {
                    unitType.head.tl = to_vi2d(fileset["HeadImage"]["tl"]);
                    unitType.head.sz = to_vi2d(fileset["HeadImage"]["size"]);
                    unitType.head.tex_id = meta.tex_id;
                }

                unitType.texture_metadata.insert_or_assign(name, std::move(meta));
            }          
         
            if (UnitData["Parameters"] != sol::nil &&
                UnitData["Parameters"]["Abilities"] != sol::nil &&
                UnitData["Parameters"]["Abilities"]["Tasks"] != sol::nil) {
                sol::table tasks = UnitData["Parameters"]["Abilities"]["Tasks"];
                for (int i = 0; i < tasks.size(); ++i) {
                    unitType.task_abilities.emplace_back(tasks[i + 1].get<std::string>());
                }
            }
            if (UnitData["Parameters"]["Ranged"] == true)
                unitType.projectileName = UnitData["Parameters"]["Projectile"];

            unitType.lua_data = std::move(UnitData); // UnitData is empty now

            unitCache.insert_or_assign(name, std::move(unitType));//if you say unit you get Full table!
        } catch (std::exception e) {
            std::cerr << e.what() << "\n";
        }
    }
}

void cAssets::LoadBuildingAssets() {
    sol::table BuildingData, FileSets, BuildingStats, Offsets;

    auto to_vi2d = [](sol::table obj) -> olc::vi2d {
        int32_t x = obj[1],
            y = obj[2];
        return { x, y };
    };
    const std::vector<std::string> filePaths = { //
        "Assets/Buildings/House/house.lua",
        "Assets/Buildings/Stone_Tower/Stone_Tower.lua",
        "Assets/Buildings/Castle/Castle.lua",
        "Assets/Buildings/Mine/GoldMine.lua"

    };

    for (const auto& path : filePaths) {
        try {
            sol::load_result script = lua_state.load_file(path);

            sol::protected_function_result rcode = script();

            if (!rcode.valid()) {
                sol::error e = rcode;
                std::cout << "error: " << e.what() << "\n";
            }
            else {
                BuildingData = rcode;
            }

            std::string name = BuildingData["Name"];

            BuildingType::TextureMetaData meta;

            BuildingType buildingType;
            if (BuildingData["Description"] != sol::nil) {
                buildingType.Description = BuildingData["Description"];
            } else
                buildingType.Description = "Not Added For this Unit";

            if (BuildingData["Icon"] != sol::nil) {
                buildingType.icon.fsz = to_vi2d(BuildingData["Icon"]["FileSize"]);
                buildingType.icon.sz = to_vi2d(BuildingData["Icon"]["size"]);
                buildingType.icon.tex_id = TextureCache::GetCache().CreateTexture(BuildingData["Icon"]["FileName"]);
            }            

            FileSets = BuildingData["Files"];
            for (int i = 0; i < FileSets.size(); ++i) {
                sol::table fileset = FileSets[i + 1];
                // load the Name : TextureID for the Building
                std::string name = fileset["Name"];

                std::string path = fileset["FileName"];
                meta.tex_id = TextureCache::GetCache().CreateTexture(path);
                meta.target_size = to_vi2d(fileset["TargetSize"]);
                meta.scale = olc::vf2d(meta.target_size);
                meta.draw_origin = olc::vf2d(to_vi2d(fileset["Origin"]));

                BuildingType::LevelOffset lvlo;
                Offsets = fileset["LevelOffsets"];
                for(int i = 0; i < Offsets.size(); ++i){
                    sol::table level = Offsets[i+1];
                    std::string name = level["Name"];
                    lvlo.offset = to_vi2d(level["Offset"]);
                    lvlo.tile_size = to_vi2d(level["TileSize"]);
                    meta.level_offsets.insert_or_assign(name, std::move(lvlo));
                }
                buildingType.Cost = BuildingData["Parameters"]["Cost"];
                sol::table Requirements = BuildingData["Parameters"]["Requirements"];
                for (int i = 0; i < Requirements.size(); i++) {
                    buildingType.Requirements.push_back(Requirements[i + 1]);
                }
                buildingType.texture_metadata.insert_or_assign(name, std::move(meta));
            }


            buildingType.lua_data = std::move(BuildingData);

            buildCache.insert_or_assign(name, std::move(buildingType));
        } catch (std::exception e) {
            std::cerr << e.what() << "\n";
        }
    }

}
/* Working On Loading Projectile Assets */
void cAssets::LoadProjectileAssets() {
    sol::table ProjectileData, FileSets;

    auto to_vi2d = [](sol::table obj) -> olc::vi2d {
        int32_t x = obj[1],
            y = obj[2];
        return { x, y };
    };
    const std::vector<std::string> filePaths = { //
        "Assets/Projectiles/Arrows/arrow.lua",
        "Assets/Projectiles/Axe/ThrowAxe.lua",
        "Assets/Projectiles/Bolt/Bolt.lua"
    };

    for (const auto& path : filePaths) {
        try {
            sol::load_result script = lua_state.load_file(path);

            sol::protected_function_result rcode = script();

            if (!rcode.valid()) {
                sol::error e = rcode;
                std::cout << "error: " << e.what() << "\n";
            }
            else {
                ProjectileData = rcode;
            }

            std::string name = ProjectileData["Name"];

            ProjectileType::TextureMetaData meta;

            ProjectileType projectileType;

            FileSets = ProjectileData["Files"];
            for (int i = 0; i < FileSets.size(); ++i) {
                sol::table fileset = FileSets[i + 1];
                // load the Name : TextureID for the Projectile
                std::string name = fileset["Name"];

                std::string path = fileset["FileName"];
                meta.tex_id = TextureCache::GetCache().CreateTexture(path);
                meta.target_size = to_vi2d(fileset["TargetSize"]);
                meta.scale = olc::vf2d(meta.target_size);

                projectileType.texture_metadata.insert_or_assign(name, std::move(meta));
            }

            projectileType.lua_data = std::move(ProjectileData);

            projCache.insert_or_assign(name, std::move(projectileType));
        } catch (std::exception e) {
            std::cerr << e.what() << "\n";
        }
    }
}

void cAssets::LoadResearchAssets() {
    auto& engine = Game_Engine::Current();
    sol::table Researchdata, FileSets;

    auto to_vi2d = [](sol::table obj) -> olc::vi2d {
        int32_t x = obj[1],
            y = obj[2];
        return { x, y };
    };
    const std::vector<std::string> filePaths = { //
        "Assets/Research/Weapon_Upgrade.lua",
        "Assets/Research/Armour_Upgrade.lua",
        "Assets/Research/Boots_Upgrade.lua",
        "Assets/Research/BowCraft.lua",
        "Assets/Research/Wands.lua"
    };

    for (const auto& path : filePaths) {
        try {
            sol::load_result script = lua_state.load_file(path);
            sol::protected_function_result rcode = script();
            if (!rcode.valid()) {
                sol::error e = rcode;
                std::cout << "error: " << e.what() << "\n";
            } else {
                Researchdata = rcode;
            }
            std::string name = Researchdata["Name"];
            engine.researchmanager->ImportList.push_back(name);
            ResearchType researchType;
            if (Researchdata["Description"] != sol::nil) {
                researchType.Description = Researchdata["Description"];
            } else
                researchType.Description = "Not Added For this Unit";


                // load the Name : TextureID for the Projectile   

            if (Researchdata["Icon"] != sol::nil) {
                researchType.icon.fsz = to_vi2d(Researchdata["Icon"]["FileSize"]);
                researchType.icon.sz = to_vi2d(Researchdata["Icon"]["size"]);
                researchType.icon.tex_id = TextureCache::GetCache().CreateTexture(Researchdata["Icon"]["FileName"]);
            } 
           sol::table Requirements = Researchdata["Requirements"]["Buildings"];
           for (int i = 0; i < Requirements.size(); i++) {
               researchType.Requirements.push_back(Requirements[i + 1]);
           }
            researchType.lua_data = std::move(Researchdata);
            resCache.insert_or_assign(name, std::move(researchType));
        } catch (std::exception e) {
            std::cerr << e.what() << "\n";
        }
    }
}

void cAssets::ImportGameMusic() {
    auto& engine = Game_Engine::Current();
    //Make this automatic later;
    engine.soundmanager->LoadAudioFile("Main Menu", "Assets/Sounds/Game_Music/Game_Start_Song.wav", true);
}

bool cAssets::ImportCursor(const std::string& name, const std::string& path, const olc::vf2d& size) {
    Cursor cursor;
    olc::Sprite* spr = TextureCache::GetCache().GetTexture(
        TextureCache::GetCache().CreateTexture(path)
    );
    if(spr == nullptr) return false;

    cursor.decal.reset(new olc::Decal(spr));
    cursor.scale = size / olc::vf2d(float(spr->width), float(spr->height));

    assetCursorCache.insert_or_assign(name, std::move(cursor));
    return true;
}

void cAssets::LoadCursorAssets() {

    auto to_vf2d = [](sol::table obj) -> olc::vf2d {
        float x = obj[1],
              y = obj[2];
        return {x, y};
    };

    sol::table FilePaths;
    try {
            sol::load_result script = lua_state.load_file("Assets/Gui/cursors.lua");

            sol::protected_function_result rcode = script();

            if (!rcode.valid()) { 
                sol::error e = rcode;
                std::cout << "error: " << e.what() << "\n"; 
            } else {
                FilePaths = rcode;
            }

            for(int i=0; i < FilePaths.size(); ++i){
                sol::table Cursor = FilePaths[i + 1];
                std::string name = Cursor["Name"], path = Cursor["Path"];
                olc::vf2d size {16.f, 16.f};
                if(Cursor["Size"] != sol::nil){
                    size = to_vf2d(Cursor["Size"]);
                }
                ImportCursor(name, path, size);
            }
    } catch(std::exception e) {
        std::cerr << e.what() << "\n";
    }
}

TextureCache* TextureCache::self = nullptr;

TextureCache::TextureCache() {
    textures.reserve(128);
}

TextureCache::~TextureCache() {
    for(auto* p : textures) delete p;
    textures.clear();
}

void TextureCache::InitCache() {
    self = new TextureCache;
}

void TextureCache::FreeCache() {
    delete self;
}

size_t TextureCache::CreateTexture(const std::string& path) {
    olc::Sprite* texture = new olc::Sprite(32,32); // errored/default texture on failed to load

    if(texture->LoadFromFile(path) != olc::OK){
        std::cerr << "Failed to load texture: " << path << "\n";
    }

    textures.emplace_back(texture);
    return textures.size() - 1;
}

olc::Sprite* TextureCache::GetTexture(size_t texid) {
    if(texid >= textures.size()) return nullptr;
    return (textures.at(texid));
}
//-----------------------------------------SOUND MANAGER ---------------------------------------
SoundManager::SoundManager() {
    soundEngine.InitialiseAudio();//Need to start engine
    soundEngine.SetCallBack_OnWaveDestroy(std::bind(&SoundManager::OnWaveFinished,this,std::placeholders::_1));
    DialogEngine.InitialiseAudio();//Start Dialog engine
    SfxEngine.InitialiseAudio();
    MusicEngine.InitialiseAudio();
}



SoundManager::~SoundManager() {

}

void SoundManager::Stop_all_sounds() {
    soundEngine.StopAll(); //Stop EVERYTHING  
}
void SoundManager::OnWaveFinished(olc::sound::PlayingWave wave) {
    playing_sounds.remove_if([&wave](const auto& w) { return wave == w; });
}

float SoundManager::Volume_Calculator(int vol_id) {
    auto& engine = Game_Engine::Current();
    switch (vol_id) {
    case 3:
        return engine.config->GetValue<float>("Unit_Audio_Level");
    case 2:
        return engine.config->GetValue<float>("SFX_Audio_Level");
    case 1:
        return engine.config->GetValue<float>("Music_Audio_Level");
    case 0:
        return engine.config->GetValue<float>("Master_Audio_Level");
    default:
        return engine.config->GetValue<float>("Master_Audio_Level");
    }
    return {};

}

olc::sound::PlayingWave SoundManager::Play_Sound_Effect(olc::sound::Wave& soundeffect, double vol) {

    olc::sound::PlayingWave w =  soundEngine.PlayWaveform( &soundeffect , false, 1.0, 0.2); //play actual sound, dont loop it when finished, normal speed
    playing_sounds.emplace_back(w);
    return w;
}
void SoundManager::Play_Music(olc::sound::Wave music) {
    auto w = soundEngine.PlayWaveform( &music , true, 1.0); //play actual sound, loop it when finished, normal speed
    playing_sounds.emplace_back(w);
    

}
void SoundManager::Stop_Sound(olc::sound::PlayingWave sound) {
    soundEngine.StopWaveform(sound); 
}

bool SoundManager::LoadAudioFile(const std::string& object_name,const std::string& snd_pack_name, const std::string& Sound_path) {
    olc::sound::Wave wave;
    if (!wave.LoadAudioWaveform(Sound_path))
        return false;
    std::map<std::string, std::map<std::string, olc::sound::Wave>> soundpack = {};
    if (!soundpacks.count(object_name)) {
        //soundpacks.insert_or_assign(object_name,soundpack);    
        soundpacks.insert_or_assign(object_name, std::map < std::string, std::map < std::string, olc::sound::Wave >> {});
    }
    if (!soundpacks.at(object_name).count(snd_pack_name)) {
        /*soundpack.insert_or_assign(snd_pack_name, std::map < std::string, olc::sound::Wave > {});  */
        //soundpacks.insert_or_assign(object_name, std::map < std::string, std::map < std::string, olc::sound::Wave >> {});
        //soundpacks.at(object_name).insert_or_assign(snd_pack_name, std::map < std::string, olc::sound::Wave> {});
    }
    
    soundpacks[object_name][snd_pack_name][std::filesystem::path(Sound_path).filename().string()] = std::move(wave);
    //soundpacks.at(object_name).at(snd_pack_name).insert_or_assign(std::filesystem::path(Sound_path).filename().string(), std::move(wave));
    return true;
}
bool SoundManager::LoadAudioFile(const std::string& snd_pack_name, const std::string& Sound_path, bool isSystemSound) {
    olc::sound::Wave wave;
    if (!wave.LoadAudioWaveform(Sound_path))
        return false;
    SystemSounds.insert_or_assign(snd_pack_name, Sound_path);
    return true;

}

olc::sound::PlayingWave SoundManager::Play_Random_PackSound(const std::string& sound_pack_name, std::string link) {
    if (!soundpacks.count(link))
        return {};
    if (!soundpacks.at(link).count(sound_pack_name)) {
        return {};
    }
    static std::map<std::string, Clock> timeout_map;
    if(sound_pack_name != "Death")
        if (!timeout_map.count(sound_pack_name)) {
        Clock Timeout; 
       Timeout.setMilliseconds(100);
        timeout_map.insert_or_assign(sound_pack_name, Timeout);
        }
        else if (timeout_map.at(sound_pack_name).getMilliseconds() > 2000) {
        timeout_map.at(sound_pack_name).restart();
        }
        else
            return{};


    auto& sound_pack = soundpacks.at(link).at(sound_pack_name);
    int idx = rand() % sound_pack.size()+1;
    if (idx == 0) {
        for (auto& [name, snd] : sound_pack) {
            return Play_Sound_Effect(snd);
        }
    }
    for (auto& [name, snd] : sound_pack) {
        if (!--idx) {
            return Play_Sound_Effect(snd);
        }
    }
    return {};
}

olc::sound::PlayingWave SoundManager::Play_System_Sound(const std::string& soundname, int vol_id) {


    float Volume = Volume_Calculator(vol_id);
    soundEngine.SetOutputVolume(Volume);
    if (!SystemSounds.count(soundname)) {
        return {};
    }
    for (auto& [name, snd] : SystemSounds) {
        return Play_Sound_Effect(snd,Volume);
    }
    return {};
}
