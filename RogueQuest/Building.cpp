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
void Building::ProduceUnit(std::string unit) {
	auto& engine = Game_Engine::Current();
	
	if (!building) {		
		auto& data = engine.assetManager->GetUnitData(unit);
		productiontime = data.lua_data["Stats"]["Health"];
		building = true;
	}
	if (m_fTimer > productiontime) {
		if (!productionQue.empty()) productionQue.pop();
		SendUnit(engine.worldManager->GenerateUnit(unit, Position + olc::vf2d({ -10.f, float(rand() % Size.y) })));
		building = false;
		startbuilding = false;
	}
}

void Building::SendUnit(std::shared_ptr<Unit> unit) {
	if (sentUnitPos != olc::vf2d(0.f,0.f))
		unit->vTarget = sentUnitPos;
}

void Building::Update(float delta){	
	Collidable::Update(delta); // Do this better

	if (productionQue.size() > 0 && !startbuilding) {
		startbuilding = true;
		m_fTimer = 0.f;
		ProduceUnit(productionQue.front());		
	}
	if (building) {
		ProduceUnit(productionQue.front());
		m_fTimer += delta * 2.f;
	}
	if(health < 0)
		Collidable::Destroy();
	
}

void Building::Draw(olc::TileTransformedView* gfx){
	const auto& meta = textureMetadata.at(Graphic_State);
	const auto& stage = meta.level_offsets.at(curStage);

	Collidable::Draw(gfx); // inherit

	gfx->DrawPartialDecal(Position, Size, decals[Graphic_State].get(), stage.offset, stage.tile_size, bSelected ? olc::WHITE : olc::GREY);
}
