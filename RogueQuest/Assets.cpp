#include "Assets.h"

/*

cAssets::cAssets(std::string name)
{

}
cAssets::~cAssets()
{

}
void cAssets::LoadUnitAssets()
{
	sol::load_result script = lua_assets.load_file("\Mage.lua");
	sol::protected_function_result rcode = script();

    if (!rcode.valid()) { 
        sol::error e = rcode;
        std::cout << "error: " << e.what() << "\n"; 
    }
    else {
        UnitData = rcode;//UnitData = Major code
    }
 //   m_UnitData[std::string(UnitData["Name"])] = UnitData;//<--Load UnitData and put it into map
    if (UnitData["Multifile"] == 1)
    {
        bMultiFile = true;  //Hey, this sprite need Extra pngs! Use the vector of pngs!
    }
    
    FileSets = UnitData["files"];
    for (int i = 0; i < FileSets.size(); ++i)
    {
        sol::table fileset = FileSets[i + 1];
        olc::Sprite* gfx = new olc::Sprite;

        if (gfx->LoadFromFile(StringifyObject(fileset["filename"])) != olc::OK) {
            std::cout << "Error loading: " << StringifyObject(fileset["filename"]) << "\n";
            delete gfx; 
            continue;
        }
        Animation = FileSets["Animation"]; //this is int for m_fGraphicCounter
        renderGraphics.emplace_back(gfx);
    }
    vTileSize = UnitData["tileSize"]; //This might need to be a table instead of vector :/
    sol::table UnitStats = UnitData["Stats"];  
    
}
*/