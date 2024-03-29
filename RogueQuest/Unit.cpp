#include "Unit.h"
#include "Engine.h"
#include <optional>


Unit::Unit() : Collidable(),
	 fUnitAngle(0.f), Graphic_State(Unit::Walking), Last_State(Unit::Walking), curFrame(0) {
}

Unit::~Unit() {
}

void Unit::Stop() {
	HoldTask.reset();
	std::shared_ptr<TaskManager::Task> trash;
	currentTask.swap(trash);
	std::queue<std::shared_ptr<TaskManager::Task>> cq;
	if(!taskQueue.empty()){
		cq.swap(taskQueue);
	}
	targetBuilding.reset();
	targetUnit.reset();
	TreeObject.reset();
	MineTarget.reset();
	ULogic = Passive;
	Target = std::nullopt;
	attackTarget = false;
	Velocity = { 0.f,0.f };
	if(Graphic_State != Dead){
		Graphic_State = Walking;
	}
	repairedbuilding.reset();
	Taskpaused = false;
	

}

void Unit::UnitBehaviour() {
	auto& engine = Game_Engine::Current();
	switch (ULogic) {
	case Passive:
		break;
	case Neutral:
		if (Health < prevHealth) {
			ULogic = Aggressive;
		}
		prevHealth = Health;
		break;
	case Aggressive:
		if (!Taskpaused && (targetBuilding.lock() || targetUnit.lock())) {
			HoldTask = currentTask;//Stop and hunt
			currentTask = engine.unitManager->taskMgr.PrepareTask("Hunting", std::pair<std::shared_ptr<Unit>, std::any>
						{engine.unitManager->This_shared_pointer(Position), std::pair<std::weak_ptr<Building>, std::weak_ptr<Unit>> {targetBuilding, targetUnit} });
			currentTask->initTask();
			Taskpaused = true;
		}
		break;
	}
}

void Unit::Gather() {
	if (MineTarget.lock()) {
		MineTarget.lock()->Gold -= 25;
		Gold += 25;
	}
	else if (TreeObject.lock())
	{
		TreeObject.lock()->Lumber -= 25;
		Lumber += 25;
	}
}

void Unit::Deliver() {
	auto& engine = Game_Engine::Current();
	if (Gold > 0) {
		engine.leaders->LeaderList[Owner]->Gold += Gold;
		Gold -= Gold;
	}
	if (Lumber > 0) {
		engine.leaders->LeaderList[Owner]->Lumber += Lumber;
		Lumber -= Lumber;
	}
}

bool Unit::OnCollision(std::shared_ptr<Collidable> other, olc::vf2d vOverlap) {

	if (other.get() == this) return true; // act as a continue
	if(std::shared_ptr<Unit> unit = std::dynamic_pointer_cast<Unit>(other)){
		// unit vs unit
		if(unit->Velocity.mag2() < 4.f){
			unit->Velocity += Velocity / 2.f;
			unit->bAnimating = true;
		}
		predPosition -= vOverlap;
		Velocity /= 1.05f;
	}
	if(std::shared_ptr<Building> build = std::dynamic_pointer_cast<Building>(other)){
		auto& engine = Game_Engine::Current();

		// unit vs building
		predPosition -= vOverlap;
	}
	if (std::shared_ptr<CollisionMapObject> mapobj = std::dynamic_pointer_cast<CollisionMapObject> (other))	{
		predPosition -= vOverlap;

	}
	return true;
}

void Unit::KnockBack(float damage, olc::vf2d velocity) {
	HitVelocity += velocity;
	Health -= damage;
}

void Unit::Update(float delta) {
	auto& engine = Game_Engine::Current();
	if(fAttackCD > 0)
		fAttackCD -= delta;
	if (!attackTarget) {//if your not told to kill somthing in preticular
		UnitBehaviour();
		if (ULogic == Aggressive) {
			UnitSearch();
		}
	} else {
		UnitHunting();
	}

	if ((currentTask && currentTask->checkCompleted()) || Health < 0) {
		Target = std::nullopt;
		currentTask.reset();
		if (Taskpaused == true) {
			if (HoldTask) {
				currentTask = HoldTask;
				currentTask->initTask();
				HoldTask.reset();
			}
			Taskpaused = false;
		}
	}
	if (!currentTask) {
		if (taskQueue.size()) {
			currentTask = taskQueue.front();
			taskQueue.pop();
			currentTask->initTask();
		}
	}
	
	if (Graphic_State != Dead) 
		UpdatePosition(delta);

	Collidable::Update(delta);

	drawDepth = Position.y / 4000.f;
}

void Unit::TrytoBuild(const std::string& name,const olc::vf2d& target) {
	auto& engine = Game_Engine::Current();
	const auto& buildingdata = engine.assetManager->GetBuildingData(name);
	auto to_vi2d = [](sol::table obj) -> olc::vi2d {
		int32_t x = obj[1],
			y = obj[2];
		return { x, y };
	};
	Target = target;
	buildName = name;
	buildingSize = to_vi2d(buildingdata.lua_data["Parameters"]["CollisionSize"]);

}

void Unit::RepairBuilding() {
	auto& engine = Game_Engine::Current();
	if (repairedbuilding.lock()) {
		repairedbuilding.lock()->Health += 0.2f;
		if (execTimeout.getMilliseconds() > 400) {
			engine.particles->GenerateSmoke(repairedbuilding.lock()->Position, olc::vf2d(repairedbuilding.lock()->Size), true);			
			execTimeout.restart();
		}
		if (repairedbuilding.lock()->curStage == 0 && repairedbuilding.lock()->Health >= repairedbuilding.lock()->maxHealth) {
			repairedbuilding.reset();
		}
	}
}

void Unit::AfterUpdate(float delta) {
	UnitGraphicUpdate(delta);
	mask.origin = textureMetadata.at(Graphic_State).draw_origin;
	Collidable::AfterUpdate(delta);
}

void Unit::UnitSearch() {//Target = unit/build.front()
	auto& engine = Game_Engine::Current();
	engine.unitManager->ParseObject(engine.unitManager->SearchClosestEnemy(Owner,Position,AgroRange), targetBuilding, targetUnit, TreeObject);
	if (targetBuilding.lock() || targetUnit.lock()) {
		if (targetBuilding.lock()) {
			Target = targetBuilding.lock()->Position + olc::vf2d(targetBuilding.lock()->Size) / 2.f;
			targetUnit.reset();
		}
		else {
			Target = targetUnit.lock()->Position;
			targetBuilding.reset();
		}		
		UnitHunting();
	}
}

void Unit::UnitHunting() {
	if (targetBuilding.lock()) {
		Target = targetBuilding.lock()->Position;
	}
	if (targetUnit.lock()) {
		Target = targetUnit.lock()->Position;
	}
}

void Unit::PerformAttack() {
	auto& engine = Game_Engine::Current();

	if (targetUnit.lock()) {
		if (bIsRanged) {
			//engine.soundmanager->Play_Random_PackSound("Attack");
			engine.worldManager->GenerateProjectile(engine.assetManager->GetUnitData(sUnitName).projectileName, engine.unitManager->This_shared_pointer(Position), targetUnit);
		}
		else {
			targetUnit.lock()->Health -= fAttackDamage;
			engine.particles->GenerateBlood(targetUnit.lock()->Position);//Blood
		}
		//knockback here
	}
	else if (targetBuilding.lock()) {
		if (bIsRanged) {
			engine.worldManager->GenerateProjectile(engine.assetManager->GetUnitData(sUnitName).projectileName, engine.unitManager->This_shared_pointer(Position), targetBuilding);
		}
		else {
			targetBuilding.lock()->Health -= fAttackDamage;
			engine.particles->GenerateSmoke(targetBuilding.lock()->Position, olc::vf2d(targetBuilding.lock()->Size), false);//Blood
		}
	}	
	fAttackCD = fAttackSpeed; // reset time for next attack
	Graphic_State = Walking;
	curFrame = 0;
}

void Unit::UpdatePosition(float delta) {
	auto& engine = Game_Engine::Current();
	if (Target.has_value()) {		
			Distance = Target.value() - Position;//distance from target location
			olc::vf2d direction = (Target.value() - Position).norm(); //direction to target
			Velocity = direction * fMoveSpeed;//Velocity toward that direction * move speed

			if (ActionMode) {
				if (Distance.mag2() < ActionZone.mag2()) {//If action zone is a circle do if inside circle
					if (currentTask)
						currentTask->performTask();
					Velocity = { 0.f,0.f };
				}
			} else{
				Distance = Target.value() - Position;
				if (   std::abs(Distance.x) < ActionZone.x 
					&& std::abs(Distance.y) < ActionZone.y) {
					if (currentTask)
						currentTask->performTask();
					Velocity = { 0.f,0.f };
				}				
			}			
	}else
		Velocity = { 0.f,0.f };

	//To Do: Re-add Knockback soon
	//HitVelocity *= std::pow(0.05f, delta);
	//if (HitVelocity.mag2() < 4.f)
	//	HitVelocity = { 0.f, 0.f };
	//predPosition += HitVelocity * delta; // hitback only
}

void Unit::UnitGraphicUpdate(float delta) {
	auto& engine = Game_Engine::Current();
	bAnimating = true;
	if (Velocity.mag2() < 0.1f * 0.1f && Graphic_State != Dead)
		bAnimating = false;	
	
	if (Distance.mag2() < ActionZone.mag2()) {

		if (repairedbuilding.lock()) {
			Graphic_State = Build;
			bAnimating = true;
		}
		if ((targetUnit.lock() || targetBuilding.lock()) && fAttackCD < 0.f) {
			Graphic_State = Attacking;
			bAnimating = true;
		}	
	}

	if (Graphic_State == Dead && curFrame == textureMetadata[Graphic_State].ani_len - 1) {
		Stop();
		engine.soundmanager->Play_Random_PackSound("Death",sUnitName);
		Destroy();
	}
	//if (currentTask) {//Exceptions to the rule
	//	if (currentTask->taskName == "Repair" && Velocity.mag2() < 0.1f * 0.1f)
	//		Graphic_State = Build;
	//	
	//}
	//else
	//	Graphic_State = Walking;

	//if (repairedbuilding.lock() && Velocity.mag2() < 0.1f * 0.1f)
	//	bAnimating = true;
	//if (targetUnit.lock() && Velocity.mag2() < 0.1f * 0.1f && fAttackCD < 0.f)
	//	bAnimating = true;
	//if (targetBuilding.lock() && Velocity.mag2() < 0.1f * 0.1f && fAttackCD < 0.f)
	//	bAnimating = true;

	if (Health <= 0) {
		Graphic_State = Dead;
		Stop();
	}	

	if (Last_State != Graphic_State) {
		curFrame = 0;
		Last_State = Graphic_State;
	}

	if (bAnimating)
		m_fTimer += delta;

	if (m_fTimer >= 0.1f) {//if time > 0.1f ms
		m_fTimer -= 0.1f;
		++curFrame %= textureMetadata.at(Graphic_State).ani_len;//This is animation length it's complex cause my game 
		// ++toframe 
	}
}

void Unit::Draw(olc::TileTransformedView* gfx){
	Collidable::Draw(gfx); // inherit
	auto& engine = Game_Engine::Current();

	const auto& meta = textureMetadata.at(Graphic_State);
	olc::vi2d SpriteSheetOffset = { 0, 0 };
	olc::vi2d SpriteSheetTileSize = meta.tile_size;
	const olc::vf2d& SpriteOrigin = meta.draw_origin;

	//Calculate graphic facing Direction
	if (Velocity != olc::vf2d({ 0.0f, 0.0f }))
		fUnitAngle = std::fmod(2.0f * PI + Velocity.polar().y, 2.0f * PI);
	if (!std::isnan(fUnitAngle)) {
		if (fUnitAngle >= 7.0f * PI / 4.0f || fUnitAngle < PI / 4.0f)
			FacingDirection = East;	
		if (fUnitAngle >= PI / 4.0f && fUnitAngle <= 3.0f * PI / 4.0f)
			FacingDirection = South;
		if (fUnitAngle >= 3.0f * PI / 4.0f && fUnitAngle <= 5.0f * PI / 4.0f)
			FacingDirection = West;
		if (fUnitAngle >= 5.0f * PI / 4.0f && fUnitAngle <= 7.0f * PI / 4.0f)
			FacingDirection = North;
	} else {
		FacingDirection = South;
	}
	switch (Graphic_State){
		case Walking:
			SpriteSheetOffset.y = Direction[FacingDirection] *SpriteSheetTileSize.y;
			SpriteSheetOffset.x = curFrame * SpriteSheetTileSize.x;
			break;
		case Dead:
			SpriteSheetOffset.x = curFrame * SpriteSheetTileSize.x;		
			break;
		case Attacking:
			SpriteSheetOffset.y = Direction[FacingDirection] * SpriteSheetTileSize.y;
			SpriteSheetOffset.x = curFrame * SpriteSheetTileSize.x;
			break;
		case Build:
			SpriteSheetOffset.y = Direction[FacingDirection] * SpriteSheetTileSize.y;
			SpriteSheetOffset.x = curFrame * SpriteSheetTileSize.x;
			break;
	}
	gfx->DrawPartialDecal((Position - SpriteOrigin),meta.target_size, decals[Graphic_State].get(),
		SpriteSheetOffset, SpriteSheetTileSize, 
		(bSelected ? olc::WHITE: engine.highlightmanagment->OwnerColor(Owner)) - engine.worldManager->currentMap->Darkness);
	
	//if (bSelected == true) {//Debug Selection
	//	//static Circle debugCircle{ 512.f };
	//	//debugCircle.position = Position ;	

	//	//debugCircle.radius = olc::vf2d(mask.radius, mask.radius);
	//	//debugCircle.col = olc::Pixel(0x700F0AAA);
	//	//debugCircle.Draw();
	//	////DrawCircleDecal(Position, AgroRange, olc::DARK_RED, gfx);
	//	//DrawCircleDecal(Position, mask.radius, olc::GREEN, gfx);
	//	//DrawCircleDecal(Position, fAttackRange, olc::BLACK, gfx);

	//	if (Target.has_value()) {
	//		gfx->DrawLineDecal(Position, Target.value());

	//		if (ActionZone > olc::vf2d(0.f, 0.f)) {
	//			if (ActionMode)
	//				DrawCircleDecal(Target.value(), ActionZone.mag(), olc::WHITE, gfx);
	//			else {
	//				auto homebase = HomeBase.lock();
	//				auto minetarget = TreeObject.lock();
	//				if (TreeObject.lock() && Target != homebase->Position) {
	//					gfx->DrawLineDecal(minetarget->Position - ActionZone, olc::vf2d({ minetarget->Position.x + ActionZone.x, minetarget->Position.y - ActionZone.y }));
	//					gfx->DrawLineDecal(minetarget->Position - ActionZone, olc::vf2d({ minetarget->Position.x - ActionZone.x  ,minetarget->Position.y + ActionZone.y }));
	//					gfx->DrawLineDecal(minetarget->Position + ActionZone, olc::vf2d({ minetarget->Position.x + ActionZone.x  ,minetarget->Position.y - ActionZone.y }));
	//					gfx->DrawLineDecal(minetarget->Position + ActionZone, olc::vf2d({ minetarget->Position.x - ActionZone.x ,minetarget->Position.y + ActionZone.y }));
	//				}
	//				else 
	//				{					
	//					if (homebase) {
	//						gfx->DrawLineDecal(homebase->Position - ActionZone, olc::vf2d({ homebase->Position.x + ActionZone.x, homebase->Position.y - ActionZone.y }));
	//						gfx->DrawLineDecal(homebase->Position - ActionZone, olc::vf2d({ homebase->Position.x - ActionZone.x  ,homebase->Position.y + ActionZone.y }));
	//						gfx->DrawLineDecal(homebase->Position + ActionZone, olc::vf2d({ homebase->Position.x + ActionZone.x  ,homebase->Position.y - ActionZone.y }));
	//						gfx->DrawLineDecal(homebase->Position + ActionZone, olc::vf2d({ homebase->Position.x - ActionZone.x ,homebase->Position.y + ActionZone.y }));
	//					}
	//				}
	//			}
	//		}
	//	}
	//}

}

void Unit::DrawCircleDecal(olc::vf2d pos, float radius, olc::Pixel col, olc::TileTransformedView* gfx) {
int CIRCLE_PRECISION = 32;
	auto& engine = Game_Engine::Current();
	std::vector<olc::vf2d> poly;
	std::vector<olc::vf2d> uvs;
	for (int i = 0; i < CIRCLE_PRECISION; i++) {
		poly.push_back({ pos.x + sinf(2 * 3.14159f / CIRCLE_PRECISION * i) * radius,pos.y + cosf(2 * 3.14159f / CIRCLE_PRECISION * i) * radius });
		uvs.push_back({ 0,0 });
	}
	poly.push_back(poly[0]);
	uvs.push_back({ 0,0 });
	engine.SetDecalStructure(olc::DecalStructure::LINE);
	engine.SetDecalMode(olc::DecalMode::WIREFRAME);
	gfx->DrawPolygonDecal(nullptr, poly, uvs, col);
	engine.SetDecalMode(olc::DecalMode::NORMAL);
	engine.SetDecalStructure(olc::DecalStructure::FAN);
}