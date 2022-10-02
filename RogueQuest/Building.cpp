#include "Building.h"
#include "Engine.h"


Building::Building(): Collidable() {
	
}

Building::~Building() {
}

bool Building::OnCollision(std::shared_ptr<Collidable> other, olc::vf2d vOverlap) {
	if (other.get() == this || Position == other->Position) return true;
	predPosition -= vOverlap;
		return false;
}
void Building::BuildingBehaviour(){
	//Do nothing
	//Produce something
	//Defend if can?
	//Somthing to think about
}
void Building::ProduceUnit(std::string unit) {
	auto& engine = Game_Engine::Current();
	
	if (!building) {
		unitproduced = unit;
		auto& data = engine.assetManager->GetUnitData(unit);
		productiontime = data.lua_data["Stats"]["Health"];
		building = true;
	}
	if (m_fTimer > productiontime) {
		engine.worldManager->GenerateUnit(unit, pos + olc::vf2d({ 0.f, float(rand() % Size.y) }));
		building = false;		
	}
}



void Building::Update(float delta){
	
	Collidable::Update(delta); // Do this better
	if (!building)
		m_fTimer = 0.f;
	else
		ProduceUnit(unitproduced);
	    m_fTimer += delta * 2.f;

	if(health < 0)
		Collidable::Destroy();
	
}

void Building::Draw(olc::TileTransformedView* gfx){
	const auto& meta = textureMetadata.at(Graphic_State);
	const auto& stage = meta.level_offsets.at(curStage);

	Collidable::Draw(gfx); // inherit

	gfx->DrawPartialDecal(pos, Size, decals[Graphic_State].get(), stage.offset, stage.tile_size, bSelected ? olc::WHITE : olc::GREY);
}
