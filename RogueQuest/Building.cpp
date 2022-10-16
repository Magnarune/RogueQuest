#include "Building.h"
#include "Engine.h"
#include "clock.h"

Building::Building(): Collidable() {
}

Building::~Building() {
}

bool Building::OnCollision(std::shared_ptr<Collidable> other, olc::vf2d vOverlap) {
	return false;
}

void Building::UpgradeBuilding() {
	if(curStage < Level.size()-1)
		curStage += 1;
}

void Building::Destroy() {
	Collidable::Destroy();
}

void Building::BuildingBehaviour() {
	auto& engine = Game_Engine::Current();
	if (execTimeout.getMilliseconds() > 400) {
		engine.unitManager->ParseObject(engine.unitManager->SearchClosestEnemy(Owner, Position + olc::vf2d(Size) / 2.f, 120.f), BuildTarget, UnitTarget);
		execTimeout.restart();
	}
	if (AttackTypes.size() > 1){
		if (BuildTarget.lock() && AttackCD < 0.f) {
			engine.worldManager->GenerateProjectile(AttackTypes[curStage], Position + olc::vf2d(Size) / 2.f, BuildTarget);
			AttackCD = AttackSpeed;
		}
		if (UnitTarget.lock() && AttackCD <= 0.f) {
			engine.worldManager->GenerateProjectile(AttackTypes[curStage], Position + olc::vf2d(Size) / 2.f, UnitTarget);
			AttackCD = AttackSpeed;
		}
	}
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
		SendUnit(engine.worldManager->GenerateUnit(unitName, Owner, Position + olc::vf2d({ -10.f, float(rand() % Size.y) })));
		building = false;
		startbuilding = false;
	}
}

void Building::SentUnitlocation(olc::vf2d pos) {

}

void Building::SendUnit(std::shared_ptr<Unit> unit) {
	auto& engine = Game_Engine::Current();
	if (sentUnitPos.mag2() > 0.f) {
		
		engine.leaders->Gold(Owner, -unit->cost);
		unit->taskQueue.push(engine.unitManager->taskMgr.PrepareTask("Move", std::pair<std::shared_ptr<Unit>, std::any>{unit, std::pair<olc::vf2d, bool> {sentUnitPos, false} }));
	}
}

void Building::Update(float delta){
	auto& engine = Game_Engine::Current();
	if (AttackCD > 0)
		AttackCD -= delta;
	BuildingBehaviour();

	if (Health < maxHealth) {
		BuildingEffect();
	}

	if (Health > maxHealth) {
		UpgradeBuilding();
		Health = maxHealth;
		Health -= 10;
	}

	if (productionQue.size() > 0 && !startbuilding) {
		startbuilding = true;
		m_fTimer = 0.f;
		ProduceUnit(productionQue.front());		
	}
	if (building&& engine.leaders->LeaderList[Owner]->Gold > 12) {
		ProduceUnit(unitproduced);
		m_fTimer += delta;
	}
	if(Health < 0)
		Collidable::Destroy();

	Collidable::Update(delta); // Inherit
}

void Building::BuildingEffect() {
	auto& engine = Game_Engine::Current();
	if (!fire) {
		
		fire = true;
	}
}

void Building::Draw(olc::TileTransformedView* gfx){
	const auto& meta = textureMetadata.at(Graphic_State);
	const auto& stage = meta.level_offsets.at(Level.at(curStage));
	Collidable::Draw(gfx); // inherit
	gfx->DrawPartialDecal(Position, meta.target_size, decals[Graphic_State].get(), stage.offset, stage.tile_size, bSelected ? olc::WHITE : olc::GREY);
	
	//gfx->DrawLineDecal(Position, { Position.x + Size .x,Position.y });//collision box
	//gfx->DrawLineDecal(Position, { Position.x ,Position.y + Size.y });
	//gfx->DrawLineDecal(Position + Size, { Position.x,Position.y + Size.y });
	//gfx->DrawLineDecal(Position + Size, { Position.x + Size.x,Position.y  });

}
