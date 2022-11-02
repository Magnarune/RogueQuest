#include "Map.h"
#include "Engine.h"

Map::Map(): valid(false) {};

Map::Map(const std::string& path): valid(false) {
    ImportMap(path);
    auto& engine = Game_Engine::Current();
    for (int i = 0; i < 100; i++)
    {
        olc::vf2d drop = { (float)(rand() % engine.ScreenWidth()), (float)(rand() % engine.ScreenHeight()) };
        RainDrops.push_back({ 1.0f, drop });

    }
};

Map::~Map() {
    RainDrops.clear();
};

void Map::UpdateRain(float fElapsedtime) {
    auto& engine = Game_Engine::Current();
    for (auto& Raindrops : RainDrops) {
        Raindrops.Position.x += 1.0;
        Raindrops.Position.y += 1.0;
        Raindrops.Position += {1.0, 1.0};
        olc::vi2d Screen = engine.tv.GetVisibleTiles();
        if (Raindrops.Position.x > engine.tv.GetWorldBR().x) {
            Raindrops.Position.x = engine.tv.GetWorldTL().x;
        }
        if (Raindrops.Position.y > engine.tv.GetWorldBR().y) {
            Raindrops.Position.y = engine.tv.GetWorldTL().y;
        }
        engine.tv.DrawDecal(Raindrops.Position, engine.hud->decals["Pixel"].get());
       // engine.tv.DrawRotatedDecal(Raindrops.Position, RainDcl, 7 * 3.14159f / 4, { 0,0 }, { 0.01f, 0.01f }, olc::Pixel((int)TimeofDay, (int)TimeofDay, (int)TimeofDay, 255));
    }
}

void Map::UpdateTimeofDay(float felapsedtime) {//0 -> 100
    DayLength += felapsedtime;
    darkness_timer = sinf(DayLength /DayNightPeriod) / 2.f + 0.5f;//starts at one goes to 1   
    
    if (darkness_timer > 0.9f)//Max darkness
        Cycle = false;
    else
        Cycle = true;
    if (DayLength < TPI*DayNightPeriod + 0.0001f && DayLength > TPI*DayNightPeriod - 0.0001f)
        DayLength = 0.f;

    PhaseofDay = (int)(23 * darkness_timer + 1);

    if (Cycle) {
        Darkness =  olc::Pixel(uint8_t(100 * darkness_timer),uint8_t(100 * darkness_timer),uint8_t(100 * darkness_timer),uint8_t(255));
    }

   
    
    

}

void Map::UpdateMap(float felapsedtime) {
    UpdateTimeofDay(felapsedtime);

}

bool Map::ImportMap(const std::string& path) {
    auto& engine = Game_Engine::Current();
    if(valid) return false; // cannot import a map that has already been loaded
    
    lua.open_libraries(sol::lib::base);
    lua.open_libraries(sol::lib::math);
    lua.open_libraries(sol::lib::package);
    lua.open_libraries(sol::lib::string);
    try{
        sol::load_result script = lua.load_file(path);
        sol::protected_function_result rcode = script(); // run script
        if(!rcode.valid()){ // check if script ran okay
            sol::error e = rcode;
            std::cerr << "error: " << e.what() << "\n"; // display error
            return false;
        } else {
            mapData = rcode;
        }

        name = mapData["class"];
        sol::table layers = mapData["layers"];
        tileSets = mapData["tilesets"];
        tileSetsSize = int(tileSets.size());
        for (int i = 0; i < tileSetsSize; i++) {
            sol::table ti = tileSets[i+1];
            vFirstGid.push_back(ti["firstgid"]);

            std::unique_ptr<olc::Decal> ptr;
            ptr.reset(
                new olc::Decal(
                    TextureCache::GetCache().GetTexture(
                        TextureCache::GetCache().CreateTexture(ti["filename"])
                    )
                )
            );
            decals.emplace_back(std::move(ptr));
        }

        mapLayers = int(layers.size()); //should be 2
        tileSize = { mapData["tilewidth"] , mapData["tileheight"] };//32by 32
        layerSize = { mapData["width"], mapData["height"] };
        layerData.resize(mapLayers); //[0->1] Size of first collum
        olc::utils::geom2d::rect<float> Size = {olc::vf2d(0.f,0.f), olc::vf2d(layerSize)};
        mapquadtreeList.resize(mapLayers);
       
        for (int i = 0; i < mapLayers; i++) {
            layerData[i].resize(layerSize.x * layerSize.y); //make this [0-1] [0-959]

            sol::table data = layers[i + 1]["data"];
            
            for (int j = 0; j < layerSize.x * layerSize.y; j++) {
                if (j >= layerSize.x * layerSize.y) break;

                layerData[i][j] = data[j + 1];             
            }
        }
   /*     for (int i = 0; i < mapLayers; i++) {
            mapquadtreeList[i].resize(Size);
            for (int x = 0; x < layerSize.x; x++) {
                for (int y = 0; y < layerSize.y; y++) {
                    int pos = x + y * layerSize.x;
                    if (pos >= layerSize.x * layerSize.y) break;                    
                    olc::vi2d position = { x,  y };
                    olc::utils::geom2d::rect<float> WPos = { position* tileSize, olc::vf2d(tileSize) };
                    mapquadtreeList[i].insert({ layerData[i][pos],position * tileSize }, WPos);
                }
            }
        }*/

    } catch(std::exception e){
        std::cerr << e.what() << "\n";
        return false;
    }

    valid = true;
    return true;
}

void Map::DrawMap(olc::TileTransformedView* tv) {

    topLeftTile     = { (int)    (tv->GetWorldTL().x / tileSize.x),(int)    (tv->GetWorldTL().y / tileSize.y) };
    bottomRightTile = { (int)ceil(tv->GetWorldBR().x / tileSize.x),(int)ceil(tv->GetWorldBR().y / tileSize.y) };

    topLeftTile.x = topLeftTile.x < 0 ? 0 : topLeftTile.x;
    topLeftTile.y = topLeftTile.y < 0 ? 0 : topLeftTile.y;
    bottomRightTile.x = bottomRightTile.x >= layerSize.x ? layerSize.x : bottomRightTile.x;
    bottomRightTile.y = bottomRightTile.y >= layerSize.y ? layerSize.y : bottomRightTile.y;

    for (int i = 0; i < mapLayers-1; i++) {//-1 for now
        for (int y = topLeftTile.y; y <= bottomRightTile.y; y++) {
            for (int x = topLeftTile.x; x <= bottomRightTile.x; x++) {
                int pos = x + y * layerSize.x;
                if (pos >= layerSize.x * layerSize.y) break;
                int tile = layerData[i][pos];
                if (tile == 0) continue;
                olc::vi2d position = { x,  y }; //screen space position
                tv->DrawPartialDecal(offset + position * tileSize, GetTileSet(tile).decal, GetTile(tile) * tileSize, tileSize, { 1.0,1.0 }, olc::WHITE - Darkness);
            }
        }
    }
    //auto& engine = Game_Engine::Current();
    //olc::utils::geom2d::rect<float> screen = {engine.tv.GetWorldTL(),engine.tv.GetWorldBR()- engine.tv.GetWorldTL()};

    //for (int i = 0; i < mapLayers - 1; i++) {
    //    for (const auto& object : mapquadtreeList[i].search(screen)) {
    //        tv->DrawPartialDecal(object->item.second, GetTileSet(object->item.first).decal, GetTile(object->item.first) * tileSize, tileSize, { 1.0,1.0 });
    //   
    //    }
    //}
}

olc::vi2d Map::GetTile(int id) const {
    const TileSet tileset = GetTileSet(id);
    id -= tileset.gid;
    olc::vi2d tileCount = { tileset.decal->sprite->width / tileSize.x,
                            tileset.decal->sprite->height / tileSize.y};
    return olc::vi2d{ id % tileCount.x, id / tileCount.x };
}

Map::TileSet Map::GetTileSet(int id) const {
    for (int i = 1; i <= tileSetsSize; ++i) {
        if ( id < vFirstGid[i - 1]) return TileSet{ decals[i - 2].get(), vFirstGid[i - 2] };
    }
    return TileSet{ decals.back().get(), vFirstGid.back() };
}
