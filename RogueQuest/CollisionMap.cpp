#include "CollisionMap.h"
#include "Engine.h"
CollisionMap::CollisionMap() {
    lua_state.open_libraries(sol::lib::base);
    lua_state.open_libraries(sol::lib::math);
    lua_state.open_libraries(sol::lib::package);
    lua_state.open_libraries(sol::lib::string);
}

CollisionMap::~CollisionMap(){
}
//Oner
void CollisionMap::ImportCollisionData(){
    sol::table ObjectData, TypeSets;
    auto to_vi2d = [](sol::table obj) -> olc::vi2d {
        int32_t x = obj[1],
            y = obj[2];
        return { x, y };
    };
    const std::vector<std::string> filePaths = { //
       "Assets/MapObjects/Trees/BasicTree.lua"
    };   

    for (const auto& path : filePaths) {
        try {
            sol::load_result script = lua_state.load_file(path);

            sol::protected_function_result rcode = script();

            if (!rcode.valid()) {
                sol::error e = rcode;
                std::cout << "error: " << e.what() << "\n";
            } else {
                ObjectData = rcode;
            }

            std::string name = ObjectData["Name"];
             
            //CollidableMapObject object;
            CMapObjectCache.TreeTextureID = MapTextureCache::GetCache().CreateTexture(ObjectData["FileName"]);

            CMapObjectCache.TreeDecal.reset(new olc::Decal(MapTextureCache::GetCache().GetTexture(CMapObjectCache.TreeTextureID)));


            int firstgid = ObjectData["firstgid"];
            TypeSets = ObjectData["Types"];
            for (int i = 0; i < TypeSets.size(); i++){
                sol::table fileset = TypeSets[i + 1];
                std::string name = fileset["Name"];

            CollidableMapObject::TreeType meta;            
            meta.tilesize = ObjectData["Tilesize"];
            int Tilesize = meta.tilesize;
            meta.tilemapwidth = ObjectData["TileMapWidth"];
                sol::table tileid = fileset["Tileids"];
                for (int i = 0; i < tileid.size(); i++)
                    meta.textureids.push_back(tileid[i+1]);              

                for (int i = 0; i < meta.textureids.size(); i++) {
                    int value = meta.textureids[i] + firstgid;
                    meta.tileids.push_back(value);
                }

                meta.ObjectShape = to_vi2d(fileset["TileShape"]);
                meta.Origin = { meta.ObjectShape.x * Tilesize / 2,meta.ObjectShape.y * Tilesize / 2 };//object origin
                meta.CollisionOrigin = to_vi2d(fileset["CollisionOrigin"]);//collision origin
                meta.collisionbox = to_vi2d(fileset["CollisionBox"]);



                CMapObjectCache.TreeTypes.insert_or_assign(name, meta);//add Gate here
            }           
        } catch (std::exception e) {
            std::cerr << e.what() << "\n";
        }
    }
}
//Made map place trees and rocks and rivers
void CollisionMap::PlaceMapObjects() {
    auto& engine = Game_Engine::Current();
    auto& map = engine.worldManager->currentMap;
    //     tree-name  Treedata
    for (auto& [name, Trees] : CMapObjectCache.TreeTypes) {//For Every Tree


           
        for (int i = 0; i < map->layerSize.y*map->layerSize.x; i++) {//Check the entire Map         
          
                  

            if (Trees.tileids[0] == CLayerdata[i])//Bing! we got a match on the top left tile on the texture
            {
                if (FindMapObject(Trees.tileids,Trees.ObjectShape, i)) {
                    olc::vf2d position = Trees.ObjectShape * Trees.tilesize / 2.f;
                    float X = (i % map->layerSize.x) * map->tileSize.x;
                    int test = (int)(i / map->layerSize.x);
                    float Y = (float)test * (float)map->tileSize.x;
                    olc::vf2d pos = olc::vf2d( X, Y);
                    GenerateCollisionObject(name, pos + position);
                    i += Trees.ObjectShape.x - 1;
                }

            }
                
        }
    }

}

bool CollisionMap::FindMapObject(std::vector<int> obj,olc::vf2d Shape, int Pos) {
    auto& map = Game_Engine::Current().worldManager->currentMap;
    //                    {2,2}
 
    for (int y = 0; y < Shape.y; y++) {
        for (int x = 0; x < Shape.x; x++) {//Check if next "X" ids match   

            int pos =( x + y * map->layerSize.x) + Pos;//If y = 1...
            
            if (pos > map->layerSize.x * map->layerSize.y) return false;//Size of Map
            int objectpos = x + y * Shape.x;
            if (obj[objectpos] != CLayerdata[pos]) {
                return false;
            }
        }
      
    }
    return true;
}


void CollisionMap::GenerateCollisionObject(const std::string& name, olc::vf2d pos) {
    Game_Engine& engine = Game_Engine::Current();  
    auto& data = CMapObjectCache.TreeTypes[name];
    
    // Make map object
    std::shared_ptr<CollisionMapObject> mapobject;
    mapobject.reset(new CollisionMapObject);
    mapobject->name = name;
    mapobject->Position = pos;
    //mapobject->Origin = data.Origin;//Get object data
    mapobject->Lumber = 100;    
    int y = int(data.textureids[0] / data.tilemapwidth);
    //mapobject->sourcePos = olc::vf2d((data.textureids[0] % data.tilemapwidth )* data.tilesize, y * data.textureids[0] * data.tilesize);
    //mapobject->Size = olc::vf2d( data.ObjectShape.x* data.tilesize, data.ObjectShape.y* data.tilesize ); //one to one
   // mapobject->sourceSize = olc::vf2d(data.ObjectShape.x * 32,data.ObjectShape.y * 32 );
    
    
    //Make Texture Manager For now 
    //manual
    
         // local state ref
        // load a decal texture and add to decal map        
        
   // mapobject->objectdecal.reset(new olc::Decal(MapTextureCache::GetCache().GetTexture(CMapObjectCache.TreeTextureID)));     
        // copy texture metadata   
    mapobject->cType = Collidable::isMapObject;
    mapobject->SetMask(Collidable::Mask(data.collisionbox));
    mapobject->mask.origin = data.CollisionOrigin;   
    mapobject->drawDepth = mapobject->Position.y / 4000.f;
    mapobject->updatePriority = 0.95f;
    //engine.worldManager->addStaticObjectToList(mapobject);
    mapobject->PosSize = { mapobject->Position,data.collisionbox };
    engine.worldManager->addObjectToList(mapobject, mapobject->PosSize);
 

}

MapTextureCache* MapTextureCache::self = nullptr;

MapTextureCache::MapTextureCache() {
    Maptextures.reserve(128);
}

MapTextureCache::~MapTextureCache() {
    for (auto* p : Maptextures) delete p;
    Maptextures.clear();
}

void MapTextureCache::InitCache() {
    self = new MapTextureCache;
}

void MapTextureCache::FreeCache() {
    delete self;
}

size_t MapTextureCache::CreateTexture(const std::string& path) {
    olc::Sprite* texture = new olc::Sprite(32, 32); // errored/default texture on failed to load
    if (texture->LoadFromFile(path) != olc::OK) {
        std::cerr << "Failed to load texture: " << path << "\n";
    }
    Maptextures.emplace_back(texture);
    return Maptextures.size() - 1;
}

olc::Sprite* MapTextureCache::GetTexture(size_t texid) {
    if (texid >= Maptextures.size()) return nullptr;
    return (Maptextures.at(texid));
}