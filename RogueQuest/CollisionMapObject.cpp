#include "CollisionMapObject.h"
#include "Engine.h"
CollisionMapObject::CollisionMapObject() {

}
CollisionMapObject::~CollisionMapObject() {

}

bool CollisionMapObject::OnCollision(std::shared_ptr<Collidable> other, olc::vf2d vOverlap) {
	return false;
}

void CollisionMapObject::Update(float delta) {
	
}

void CollisionMapObject::AfterUpdate(float delta) {
}


void CollisionMapObject::Draw(olc::TileTransformedView* gfx) {
	Collidable::Draw(gfx);
	auto& engine = Game_Engine::Current();
	const auto& meta = engine.cmapmanager->CMapObjectCache.TreeTypes[name];
	if (Lumber <= 0)
		Destroy();
	int y = int(meta.textureids[0] / meta.tilemapwidth);
	olc::vf2d source = olc::vf2d((float)(meta.textureids[0] % meta.tilemapwidth) * (float)meta.tilesize, (float)y * (float)meta.textureids[0] * (float)meta.tilesize);
	gfx->DrawPartialDecal(Position - meta.Origin, meta.ObjectShape * meta.tilesize, engine.cmapmanager->CMapObjectCache.TreeDecal.get(), source, meta.ObjectShape * meta.tilesize);
	
}
 