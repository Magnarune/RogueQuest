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
}

void Building::Stop() {
	std::queue<std::string> cq;
	std::swap(productionQue, cq);
	building = false;
	m_fTimer = 0.f;
}

void Building::ProduceUnit(const std::string& unitName) {
	auto& engine = Game_Engine::Current();
	if(!engine.assetManager->UnitExists(unitName)) return;
	unitproduced = unitName;
	if (!building) {
		auto& data = engine.assetManager->GetUnitData(unitName);
		productiontime = data.lua_data["Stats"]["Health"];
		building = true;
	}

	if (m_fTimer > productiontime) {
		if (!productionQue.empty()) productionQue.pop();
		auto unit = engine.worldManager->GenerateUnit(unitName, Position + olc::vf2d({ -10.f, float(rand() % Size.y) }));
		if(!unit) return; 

		SendUnit(unit);
		building = false;
		startbuilding = false;
	}
}

void Building::SendUnit(std::shared_ptr<Unit> unit) {
	if (sentUnitPos != olc::vf2d(0.f,0.f))
		unit->vTarget = sentUnitPos;
}

void Building::Update(float delta){

	if (productionQue.size() > 0 && !startbuilding) {
		startbuilding = true;
		m_fTimer = 0.f;
		ProduceUnit(productionQue.front());		
	}
	if (building) {
		ProduceUnit(unitproduced);
		m_fTimer += delta;
	}
	if(health < 0)
		Collidable::Destroy();

	Collidable::Update(delta); // Inherit
}

void Building::Draw(olc::TileTransformedView* gfx){
	const auto& meta = textureMetadata.at(Graphic_State);
	const auto& stage = meta.level_offsets.at(curStage);
	Collidable::Draw(gfx); // inherit
	gfx->DrawPartialDecal(Position, Size, decals[Graphic_State].get(), stage.offset, stage.tile_size, bSelected ? olc::WHITE : olc::GREY);
}
