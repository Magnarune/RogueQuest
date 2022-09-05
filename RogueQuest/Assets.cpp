#include "Assets.h"


cAssets::cAssets()
{

}

cAssets::~cAssets()
{

}

void cAssets::LoadUnitAssets()
{
    sol::table UnitData, FileSets, UnitStats;


    lua_state.open_libraries(sol::lib::base);
    lua_state.open_libraries(sol::lib::math);
    lua_state.open_libraries(sol::lib::package);
    lua_state.open_libraries(sol::lib::string);

    std::vector<std::string> filePaths = {"Assets/Units/Mage/Mage.lua"}; // temporary for now
    /*
        // To Do: Load all files in directory

    using fs = std::filesystem;

    fs::directory_iterator dir;
    */

    for(const auto& path : filePaths){
        try {
            sol::load_result script = lua_state.load_file(path);

            sol::protected_function_result rcode = script();

            if (!rcode.valid()) { 
                sol::error e = rcode;
                std::cout << "error: " << e.what() << "\n"; 
            } else {
                UnitData = rcode; 
            }


            UnitType unitType;
            FileSets = UnitData["files"];
            for (int i = 0; i < FileSets.size(); ++i){
                sol::table fileset = FileSets[i + 1];
                // load the Name : TextureID for the unitType
                std::string name = fileset["Name"], path = fileset["FileName"];
                unitType.tex_ids.insert_or_assign(name, TextureCache::GetCache().CreateTexture(path));
                unitType.ani_lens.insert_or_assign(name, sol::object(fileset["AnimationLength"]).as<size_t>());
            }

            unitType.lua_data = std::move(UnitData);

            std::string name = UnitData["name"];
            assetCache.insert_or_assign(name, std::move(unitType));//if you say unit you get Full table!
        } catch(std::exception e) {
            std::cerr << e.what() << "\n";
        }
    }
    
}



TextureCache* TextureCache::self = nullptr;

TextureCache::TextureCache() {

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

    texture->LoadFromFile(path);

    textures.emplace_back(texture);
    return textures.size() - 1;
}

olc::Sprite* TextureCache::GetTexture(size_t texid) {
    if(texid >= textures.size()) return nullptr;
    return (textures.at(texid));
}