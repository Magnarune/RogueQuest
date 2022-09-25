#include "Assets.h"

cAssets::cAssets(){
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
        "Assets/Units/Builder/builder.lua"
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

            std::string name =  UnitData["Name"];
            
            UnitType::TextureMetaData meta;
            for (int i = 1; i < 5; i++)
                meta.Sprite_Order.push_back(UnitData["SpriteOrder"][i]);//Im in danger

            UnitType unitType;
            FileSets = UnitData["Files"];
            for (int i = 0; i < FileSets.size(); ++i){
                sol::table fileset = FileSets[i + 1];
                // load the Name : TextureID for the unitType
                std::string name = fileset["Name"], path = fileset["FileName"];

                meta.tex_id = TextureCache::GetCache().CreateTexture(path);
                meta.ani_len = sol::object(fileset["AnimationLength"]).as<int>();
                meta.sprite_size = to_vi2d(fileset["SpriteSize"]);
                meta.tile_size = to_vi2d(fileset["TileSize"]);
            
                meta.target_size = to_vi2d(fileset["TargetSize"]);

                meta.scale = olc::vf2d(meta.target_size) / olc::vf2d(meta.tile_size);

                // load head location data into the texture meta data
                if(fileset["HeadImage"] != sol::nil){
                    unitType.head.tl = to_vi2d(fileset["HeadImage"]["tl"]);
                    unitType.head.sz = to_vi2d(fileset["HeadImage"]["size"]);
                    unitType.head.tex_id = meta.tex_id;
                }

                unitType.texture_metadata.insert_or_assign(name, std::move(meta));
            }

            unitType.lua_data = std::move(UnitData); // UnitData is empty now

            assetCache.insert_or_assign(name, std::move(unitType));//if you say unit you get Full table!
        } catch(std::exception e) {
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
    textures.reserve(64);
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