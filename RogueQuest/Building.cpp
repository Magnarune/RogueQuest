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

void Building::BuildingBehaviour() {
	auto& engine = Game_Engine::Current();
	if (execTimeout.getMilliseconds() > 400) {
		engine.unitManager->ParseObject(engine.unitManager->SearchClosestEnemy(Owner, Position, 180.f), BuildTarget, UnitTarget);
		execTimeout.restart();
	}
	if (AttackTypes.size() > 1){
		if (BuildTarget.lock() && AttackCD < 0.f) {
			engine.worldManager->GenerateProjectile(AttackTypes[curStage], engine.buildingManager->This_Building(Position), BuildTarget);
			AttackCD = AttackSpeed;
		}
		if (UnitTarget.lock() && AttackCD <= 0.f) {
			engine.worldManager->GenerateProjectile(AttackTypes[curStage],  engine.buildingManager->This_Building(Position), UnitTarget);
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

void Building::InitializeProduction(const std::string& objectname) {
	auto& engine = Game_Engine::Current();
	if (engine.assetManager->UnitExists(objectname)) {
		m_fTimer = 0.f;
		unitproduced = objectname;
		auto& data = engine.assetManager->GetUnitData(objectname);
		productiontime = data.lua_data["Parameters"]["ProductionTime"];
		production = isUnit;
		building = true;

	}else
	if (engine.assetManager->ResearchExists(objectname)) {
		m_fTimer = 0.f;
		researchproduced = objectname;
		auto& data = engine.assetManager->GetResearchData(objectname);
		productiontime = data.lua_data["Parameters"]["ResearchTime"];
		production = isResearch;
		building = true;
	} else
		production = isNone;
	if (production == isNone && !productionQue.empty()) {		
		building = false;
	}
}

void Building::UnProduceObject() {
	auto& engine = Game_Engine::Current();
	if (!Refunds.empty()) {
		engine.leaders->LeaderList[Owner]->Gold += Refunds.front();
		Refunds.pop();
	}
	building = false;
	startbuilding = false;
	m_fTimer = 0.f;
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

	if (productionQue.size() > 0 && !building) {
			InitializeProduction(productionQue.front());
			productionQue.pop();
	}
	if (building && productiontime < m_fTimer) {
		switch (production){
		case isNone:
			break;
		case isUnit:
			SendUnit(engine.worldManager->GenerateUnit(unitproduced, Owner, Position + olc::vf2d({ -10.f, float(rand() % Size.y) })));
			building = false;
			Refunds.pop();
			break;
		case isResearch:
			engine.leaders->AddResearch(Owner, engine.researchmanager->Researchables[researchproduced]);
			auto erase = std::erase(engine.hudManager->Hide_Research,researchproduced);
			CheckResearch(researchproduced);
			building = false;
			Refunds.pop();
			break;
		}
	}

	if(Health <= 0.f){
		Destroy();
	}
	
	m_fTimer += delta;

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

	//if (engine.hud->BuildMode) {
	//auto maskpos = Position - SpriteOrigin;
	//	gfx->DrawLineDecal(maskpos, { maskpos.x + mask.rect.x,maskpos.y });//collision box
	//	gfx->DrawLineDecal(maskpos, { maskpos.x ,maskpos.y + mask.rect.y });
	//	gfx->DrawLineDecal(maskpos + mask.rect, { maskpos.x,maskpos.y + mask.rect.y });
	//	gfx->DrawLineDecal(maskpos + mask.rect, { maskpos.x + mask.rect.x,maskpos.y  });
	//}
}

void Building::CheckResearch(std::string name) {
	auto& engine = Game_Engine::Current();
	for (int i = 0; i < engine.leaders->LeaderList[Owner]->researchUpgrades.size(); i++){
		if (engine.leaders->LeaderList[Owner]->researchUpgrades[i].lock()->name == name){
			if (engine.leaders->LeaderList[Owner]->researchUpgrades[i].lock()->Level == engine.leaders->LeaderList[Owner]->researchUpgrades[i].lock()->researchlen) {
				auto erased = std::erase(researchproduction,name);
			}
		}			 
	}
}
