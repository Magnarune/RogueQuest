#include "Building.h"
#include "Engine.h"


Building::Building(): Collidable() {
}

Building::~Building() {
}

bool Building::OnCollision(std::shared_ptr<Collidable> other, olc::vf2d vOverlap) {

		return false;
}
void Building::BuildingBehaviour(){
	//Do nothing
	//Produce something
	//Defend if can?
	//Somthing to think about
}

void Building::Update(float delta){

	Collidable::Update(delta); // inherit
}

void Building::Draw(olc::TileTransformedView* gfx){
	const auto& meta = textureMetadata.at(Graphic_State);
	const auto& stage = meta.level_offsets.at(curStage);

	Collidable::Draw(gfx); // inherit

	gfx->DrawPartialDecal(pos, Size, decals[Graphic_State].get(), stage.offset, stage.tile_size, olc::WHITE);
}
