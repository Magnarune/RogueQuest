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
		engine.unitManager->ParseObject(engine.unitManager->SearchClosestEnemy(Owner, Position, 180.f), BuildTarget, UnitTarget);
		execTimeout.restart();
	}
	if (AttackTypes.size() > 1){
		if (BuildTarget.lock() && AttackCD < 0.f) {
			engine.worldManager->GenerateProjectile(AttackTypes[curStage], Position, BuildTarget);
			AttackCD = AttackSpeed;
		}
		if (UnitTarget.lock() && AttackCD <= 0.f) {
			engine.worldManager->GenerateProjectile(AttackTypes[curStage], Position, UnitTarget);
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
		productiontime = data.lua_data["Parameters"]["ProductionTime"];
		building = true;
	}

	if (m_fTimer > productiontime) {
		if (!productionQue.empty()) productionQue.pop();		
		SendUnit(engine.worldManager->GenerateUnit(unitName, Owner, Position + olc::vf2d({ -10.f, float(rand() % Size.y) })));
		building = false;
		startbuilding = false;
	}
}

void Building::UnProduceUnit() {
	if (!productionQue.empty()) productionQue.pop();
	building = false;
	startbuilding = false;
	m_fTimer = 0.f;
}

void Building::SentUnitlocation(olc::vf2d pos) {

}

void Building::SendUnit(std::shared_ptr<Unit> unit) {
	auto& engine = Game_Engine::Current();
	if (sentUnitPos.mag2() > 0.f) {		
		engine.buildingManager->AssignTask(sentUnitPos, unit);		
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
	if (building) {
		ProduceUnit(unitproduced);
		m_fTimer += delta;
	}
	if(Health <= 0.f)
		Collidable::Destroy();

	Collidable::Update(delta); // Inherit

	drawDepth = Position.y / 4000.f;
}

void Building::AfterUpdate(float delta) {
	mask.origin = textureMetadata.at(Graphic_State).draw_origin;
	Collidable::AfterUpdate(delta);

}

void Building::BuildingEffect() {
	auto& engine = Game_Engine::Current();
	if (!fire) {
		
		fire = true;
	}
}

void Building::Draw(olc::TileTransformedView* gfx){
	auto& engine = Game_Engine::Current();
	const auto& meta = textureMetadata.at(Graphic_State);
	const auto& stage = meta.level_offsets.at(Level.at(curStage));	
	//const olc::vf2d& SpriteScale = meta.scale;
	const olc::vf2d& SpriteOrigin = meta.draw_origin;

	Collidable::Draw(gfx); // inherit


	gfx->DrawPartialDecal(Position -SpriteOrigin - olc::vf2d(offset), meta.target_size, decals[Graphic_State].get(), stage.offset, stage.tile_size,
						 (bSelected ? olc::WHITE : engine.highlightmanagment->OwnerColor(Owner)) - engine.worldManager->currentMap->Darkness);
	gfx->DrawLineDecal(Position, Position + olc::vf2d(3, 3));
	gfx->DrawLineDecal(Position - SpriteOrigin, Position - SpriteOrigin + olc::vf2d(3, 3));
	//if (engine.hud->BuildMode) {
	auto maskpos = Position - SpriteOrigin;
		gfx->DrawLineDecal(maskpos, { maskpos.x + mask.rect.x,maskpos.y });//collision box
		gfx->DrawLineDecal(maskpos, { maskpos.x ,maskpos.y + mask.rect.y });
		gfx->DrawLineDecal(maskpos + mask.rect, { maskpos.x,maskpos.y + mask.rect.y });
		gfx->DrawLineDecal(maskpos + mask.rect, { maskpos.x + mask.rect.x,maskpos.y  });
	//}
}
