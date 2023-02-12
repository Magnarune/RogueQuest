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
    

    for(const auto& path : filePaths) {
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
            UnitType::SoundMetaData sMeta;
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
            //sol::table soundlist = UnitData["Sounds"];//what if unit has no sound

            //for (const auto& kv : soundlist) {
            //    sol::table list = kv.second;
            //    std::string category_name = kv.first.as<std::string>();

            //    for (const auto& snd : list) {
            //        SoundManager::LoadAudioFile(category_name, snd.second.as<std::string>());
            //    }
            //}
            //    
                
                
  

            
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


SoundManager::SoundManager() {
    auto& engine = Game_Engine::Current();
    engine.soundEngine.InitialiseAudio();//Need to initialiseaudio engine;
}
SoundManager::~SoundManager() {

}

void SoundManager::Stop_all_sounds() {
    auto& engine = Game_Engine::Current();
    engine.soundEngine.StopAll(); //Stop EVERYTHING
  
}







void SoundManager::Master_Volume(float volume) {
    auto& engine = Game_Engine::Current();
    engine.soundEngine.SetOutputVolume(volume); //volume = volume input

}

void SoundManager::Play_Sound_Effect(olc::sound::Wave soundeffect) {
    auto& engine = Game_Engine::Current();
    
  olc::sound::PlayingWave garry =  engine.soundEngine.PlayWaveform( &soundeffect , false, 1.0); //play actual sound, dont loop it when finished, normal speed
}

void SoundManager::Play_Music(olc::sound::Wave music) {
    auto& engine = Game_Engine::Current();
    engine.soundEngine.PlayWaveform( &music , false, 1.0); //play actual sound, loop it when finished, normal speed
}
void SoundManager::Stop_Sound(olc::sound::Wave sound) {
    auto& engine = Game_Engine::Current();

    //engine.soundEngine.StopWaveform(sound);
 
}

uint64_t SoundManager::Play_sound_pack(const std::string& sound, void* link = nullptr) {
    auto& engine = Game_Engine::Current();
    
        int number_of_sounds = game_sounds[sound].size();
        int random = rand() % number_of_sounds;



        playing_sounds.push_back(engine.soundEngine.PlayWaveform(&game_sounds[sound][random]));//Picks a random sound of that type    
}