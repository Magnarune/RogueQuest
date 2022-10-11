#include "Unit.h"
#include "Engine.h"
#include <optional>


Unit::Unit(const cAssets::UnitType& type) : Collidable(), unitType(type),
	 fUnitAngle(0.f), Graphic_State(Unit::Walking), Last_State(Unit::Walking), curFrame(0), currentTask(HoldTask){
}

Unit::~Unit() {
	HoldTask.reset();
	repairedbuilding.reset();
	std::shared_ptr<TaskManager::Task> trash;
	currentTask.swap(trash);
	std::queue<std::shared_ptr<TaskManager::Task>> cq;
	if (!taskQueue.empty())
		cq.swap(taskQueue);
	if(targetBuilding)
		targetBuilding.reset();
	if(targetUnit)
		targetUnit.reset();
	//Direction.clear();
}

void Unit::Stop() {
	HoldTask.reset();
	std::shared_ptr<TaskManager::Task> trash;
	currentTask.swap(trash);
	std::queue<std::shared_ptr<TaskManager::Task>> cq;
	if(!taskQueue.empty())
	cq.swap(taskQueue);
	targetBuilding.reset();
	targetUnit.reset();
	ULogic = Passive;
	Target = std::nullopt;
	Velocity = { 0.f,0.f };
	repairedbuilding.reset();
}

void Unit::UnitBehaviour() {
	auto& engine = Game_Engine::Current();
	switch (ULogic) {
	case Passive:
		break;
	case Neutral:
		if (fHealth < prevHealth) {
			ULogic = Aggressive;
		}
		prevHealth = fHealth;
		break;
	case Aggressive:
		if(targetBuilding || targetUnit && !Taskpaused){
			HoldTask = currentTask;//Stop and hunt

			currentTask = engine.unitManager->taskMgr.PrepareTask("Hunting", std::pair<std::shared_ptr<Unit>, std::any>
						{engine.unitManager->This_shared_pointer(Position), std::pair<std::shared_ptr<Building>, std::shared_ptr<Unit>> {targetBuilding, targetUnit} });
			currentTask->initTask();
			Taskpaused = true;
		}
		break;
	}
}

bool Unit::OnCollision(std::shared_ptr<Collidable> other, olc::vf2d vOverlap) {

	if (other.get() == this) return true; // act as a continue
	if(std::shared_ptr<Unit> unit = std::dynamic_pointer_cast<Unit>(other)){
		// unit vs unit
		if(unit->Velocity.mag2() < 4.f){
			unit->Velocity += Velocity;
			unit->bAnimating = true;
			if(Velocity.mag2() > 16.f && unit->Velocity.mag2() > 16.f){
				vOverlap /= 1.3f;
			}
		}
		predPosition -= vOverlap;
		Velocity /= 1.05f;
	}
	if(std::shared_ptr<Building> build = std::dynamic_pointer_cast<Building>(other)){
		// unit vs building
		predPosition -= vOverlap;
	}


	return true;
}

void Unit::KnockBack(float damage, olc::vf2d velocity) {
	HitVelocity += velocity;
	fHealth -= damage;
}

void Unit::Destroy() {
	Collidable::Destroy(); // inherit
}

void Unit::Update(float delta) {
	auto& engine = Game_Engine::Current();
	if(fAttackCD > 0)
		fAttackCD -= delta;
	if (ULogic == Aggressive) {
		//if (execTimeout.getMilliseconds() > 50) {
		UnitSearch();
		//	execTimeout.restart();
	//}
	}
	UnitBehaviour();

	if(currentTask && currentTask->checkCompleted() || fHealth < 0){
		//std::shared_ptr<TaskManager::Task> trash;
		currentTask.reset();//swap(trash);
		if (Taskpaused == true) {			
			if (!HoldTask) {
				currentTask = HoldTask;
				//std::shared_ptr<TaskManager::Task> trashHold;
				HoldTask.reset();//swap(trashHold);
			}
			Taskpaused = false;
		}
	}
	if(!currentTask){
		if(taskQueue.size()){
			currentTask = taskQueue.front();
			taskQueue.pop();
			currentTask->initTask();
		}
	}
	if (Graphic_State != Dead) 
		UpdatePosition(delta);		
	Collidable::Update(delta);
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
	if (repairedbuilding) {
		repairedbuilding->health += 0.1f;
		if (repairedbuilding->curStage == "Construction" && repairedbuilding->health >= repairedbuilding->maxHealth) {
			repairedbuilding->curStage = "Level one";
			repairedbuilding.reset();

		}
	}
}

void Unit::AfterUpdate(float delta) {
	UnitGraphicUpdate(delta);

	Collidable::AfterUpdate(delta);// inherit
}

void Unit::UnitSearch() {//Target = unit/build.front()
	auto& engine = Game_Engine::Current();
	engine.unitManager->ParseObject(engine.unitManager->SearchClosestObject(Position, AgroRange), targetBuilding, targetUnit);
	if (targetBuilding || targetUnit) {
		if (targetBuilding && !targetBuilding->bFriendly) {
			Target = targetBuilding->Position;
			targetUnit.reset();
		}
		else {
			if(!targetUnit->bFriendly)
			Target = targetUnit->Position;
			targetBuilding.reset();
		}		
		UnitHunting();
	}
}

void Unit::UnitHunting() {
	if (targetBuilding) {
		Target = targetBuilding->Position + olc::vf2d(targetBuilding->Size) / 2.f;
	}
	if (targetUnit) {
		Target = targetUnit->Position + olc::vf2d(targetUnit->Unit_Collision_Radius * 1.414f,
			targetUnit->Unit_Collision_Radius * 1.414f);
	}
}

void Unit::PerformAttack() {
	auto& engine = Game_Engine::Current();
	
	if (targetUnit) {
		if (bIsRanged) {
			engine.worldManager->GenerateProjectile(Position, targetUnit);
		}
		targetUnit->fHealth -= fAttackDamage;
		engine.particles->CreateParticles(targetUnit->Position);//Blood
		//knockback here
	}
	else if (targetBuilding) {
		targetBuilding->health -= fAttackDamage;
	}	
	fAttackCD = fSpellCooldown; // reset time for next attack
}

void Unit::UpdatePosition(float delta) {
	auto& engine = Game_Engine::Current();//UnitBehaviour();
	if (Target.has_value()) {
		Distance = Target.value() - Position;
		olc::vf2d direction = (Target.value() - Position).norm();
		Velocity = direction * fMoveSpeed;
		if (Distance.mag2() < ActionZone.mag2()) {
			if(currentTask)
				currentTask->performTask();
			Velocity = { 0.f,0.f };
		}
	}
	//HitVelocity *= std::pow(0.05f, delta);
	//if (HitVelocity.mag2() < 4.f)
	//	HitVelocity = { 0.f, 0.f };
	//predPosition += HitVelocity * delta; // hitback only
}

void Unit::UnitGraphicUpdate(float delta) {
	bAnimating = true;
	if (Velocity.mag2() < 0.1f * 0.1f && Graphic_State != Dead)
		bAnimating = false;

if (Graphic_State == Dead && curFrame == textureMetadata[Graphic_State].ani_len - 1) {
		Stop();
		Destroy();
	}
	if (currentTask) {//Exceptions to the rule
		if (currentTask->taskName == "Repair" && Velocity.mag2() < 0.1f * 0.1f)
			Graphic_State = Build;
		if (currentTask->taskName == "Hunting" && Velocity.mag2() < 0.1f * 0.1f)
			Graphic_State = Attacking;
	}
	else
		Graphic_State = Walking;

	if (repairedbuilding && Velocity.mag2() < 0.1f * 0.1f)
		bAnimating = true;
	if (targetUnit && Velocity.mag2() < 0.1f * 0.1f)
		bAnimating = true;

	if (fHealth <= 0)
		Graphic_State = Dead;

	

	if (Last_State != Graphic_State) {
		curFrame = 0;
		Last_State = Graphic_State;
	}

	if (bAnimating)
		m_fTimer += delta;
	if (m_fTimer >= 0.1f) {
		m_fTimer -= 0.1f;
		++curFrame %= textureMetadata[Graphic_State].ani_len;
	}
}

void Unit::Draw(olc::TileTransformedView* gfx){
	Collidable::Draw(gfx); // inherit

	const auto& meta = textureMetadata[Graphic_State];
	olc::vi2d SpriteSheetOffset = { 0, 0 };
	olc::vi2d SpriteSheetTileSize = meta.tile_size;
	const olc::vf2d& SpriteScale = meta.scale;
	olc::vf2d Origin = olc::vf2d(SpriteSheetTileSize) * SpriteScale / 2.f;

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
	gfx->DrawPartialDecal((Position - Origin), decals[Graphic_State].get(),
		SpriteSheetOffset, SpriteSheetTileSize, SpriteScale, bSelected ? olc::WHITE : olc::GREY);
	if (bSelected == true) {

		//gfx->DrawLineDecal((Position)+olc::vf2d(-AgroRange, 0), (Position)+olc::vf2d(AgroRange, 0), olc::RED);//AggroRange
		//gfx->DrawLineDecal((Position)+olc::vf2d(0, -AgroRange), (Position)+olc::vf2d(0, AgroRange), olc::RED);
		//gfx->DrawLineDecal((Position)+olc::vf2d(AgroRange, -AgroRange) * sqrtf(2) / 2, (Position)+olc::vf2d(-AgroRange, AgroRange) * sqrtf(2) / 2, olc::RED);
		//gfx->DrawLineDecal((Position)+olc::vf2d(-AgroRange, -AgroRange) * sqrtf(2) / 2, (Position)+olc::vf2d(AgroRange, AgroRange) * sqrtf(2) / 2, olc::RED);

		//gfx->DrawLineDecal((Position)+olc::vf2d(-fAttackRange, 0), (Position)+olc::vf2d(fAttackRange, 0), olc::BLUE);//Attack Range
		//gfx->DrawLineDecal((Position)+olc::vf2d(0, -fAttackRange), (Position)+olc::vf2d(0, fAttackRange), olc::BLUE);
		//gfx->DrawLineDecal((Position)+olc::vf2d(fAttackRange, -fAttackRange) * sqrtf(2) / 2, (Position)+olc::vf2d(-fAttackRange, fAttackRange) * sqrtf(2) / 2, olc::BLUE);
		//gfx->DrawLineDecal((Position)+olc::vf2d(-fAttackRange, -fAttackRange) * sqrtf(2) / 2, (Position)+olc::vf2d(fAttackRange, fAttackRange) * sqrtf(2) / 2, olc::BLUE);

		//gfx->DrawLineDecal((Position)+olc::vf2d(-Unit_Collision_Radius, 0), (Position)+olc::vf2d(Unit_Collision_Radius, 0), olc::GREY);//Collision Radius
		//gfx->DrawLineDecal((Position)+olc::vf2d(0, -Unit_Collision_Radius), (Position)+olc::vf2d(0, Unit_Collision_Radius), olc::GREY);
		//gfx->DrawLineDecal((Position)+olc::vf2d(Unit_Collision_Radius, -Unit_Collision_Radius) * sqrtf(2) / 2, (Position)+olc::vf2d(-Unit_Collision_Radius, Unit_Collision_Radius) * sqrtf(2) / 2, olc::GREY);
		//gfx->DrawLineDecal((Position)+olc::vf2d(-Unit_Collision_Radius, -Unit_Collision_Radius) * sqrtf(2) / 2, (Position)+olc::vf2d(Unit_Collision_Radius, Unit_Collision_Radius) * sqrtf(2) / 2, olc::GREY);


	}
}