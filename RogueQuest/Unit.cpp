#include "Unit.h"
#include "Engine.h"
#include <optional>


Unit::Unit(const cAssets::UnitType& type) : Collidable(), unitType(type),
	 fUnitAngle(0.f), Graphic_State(Unit::Walking), Last_State(Unit::Walking), curFrame(0) {
}

Unit::~Unit() {
	repairedbuilding.reset();
	currentTask.reset();
	std::queue<std::shared_ptr<TaskManager::Task>> cq;
	if (!taskQueue.empty())
		cq.swap(taskQueue);
	Hunted.reset();
	pUnit.reset();
	pBuilding.reset();
}

void Unit::Stop() {
	currentTask.reset();
	std::queue<std::shared_ptr<TaskManager::Task>> cq;
	if(!taskQueue.empty())
	cq.swap(taskQueue);
	Hunted.reset();
	ULogic = Passive;
	Target = std::nullopt;
	Velocity = { 0.f,0.f };
	repairedbuilding.reset();
}

void Unit::UnitBehaviour() {
	auto& engine = Game_Engine::Current();

	if (execTimeout.getMilliseconds() > 50) {//do this every 0.50ms
		switch (ULogic){
			case Passive://Just walk with no logic
				break;

			case Neutral:
				if (bAttacked) //Walk to location unless you are attacked
					ULogic = Attack;
				break;

			case Attack:
				if(Hunted.expired()){
					engine.worldManager->IterateObjects([&](std::shared_ptr<WorldObject> obj) {
						auto other = std::dynamic_pointer_cast<Unit>(obj);
						
						if (other == nullptr || other.get() == this || Position == other->Position) return true; // act as a continue
						float distance = (other->Position - Position).mag2();
						
						if (bFriendly != other->bFriendly && distance < AgroRange*AgroRange) {
							Hunted = other;
							return false;
						}
						
						return true;
						
					});

				} else {
					auto other = Hunted.lock();
					float distance = (other->Position - Position).mag2();
					AttackTarget = other->Position;

					if(distance >= AgroRange*AgroRange){
						Hunted.reset();
					}
				}
				break;
			default:
				ULogic = Passive;
				break;
		}
		execTimeout.restart();
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

void Unit::PerformAttack() {
	auto& engine = Game_Engine::Current();
	Graphic_State = Attacking;
	bAnimating = true;
	//Projectilemanager->AddProjectile(Unit, AttackTarget)

	if(!Hunted.expired()){
		auto obj = Hunted.lock();
		if(auto unit = std::dynamic_pointer_cast<Unit>(obj)){
			olc::vf2d vel = (unit->Position - Position).norm() * fAttackDamage * 0.f;
			if(std::isnan(vel.x) || std::isnan(vel.y)) vel = {0.f,0.f};
			unit->KnockBack(fAttackDamage, vel);
			engine.particles->CreateParticles(unit->Position);
		}
		
		if(auto building = std::dynamic_pointer_cast<Building>(obj)){

		}
		
	}
	
	fAttackCD = fSpellCooldown; // reset time for next attack
}

void Unit::KnockBack(float damage, olc::vf2d velocity) {
	HitVelocity += velocity;
	fHealth -= damage;
}

void Unit::Destroy() {
	Collidable::Destroy(); // inherit
}

void Unit::Update(float delta) {
	if(fAttackCD > 0)
		fAttackCD -= delta;

	if(!currentTask){
		if(taskQueue.size()){
			currentTask = taskQueue.front();
			taskQueue.pop();
			currentTask->initTask();
		}
	} else if(currentTask->checkCompleted()){
		currentTask.reset();
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
	repairedbuilding.lock()->health += 0.1f;
	if (repairedbuilding.lock()->curStage == "Construction" && repairedbuilding.lock()->health >= repairedbuilding.lock()->maxHealth){
		repairedbuilding.lock()->curStage = "Level one";
		repairedbuilding.reset();
		
	}	
}

void Unit::AfterUpdate(float delta) {
	UnitGraphicUpdate(delta);

	Collidable::AfterUpdate(delta);// inherit
}

void Unit::UnitHunting() {

	 Distance = AttackTarget - Position;
	fUnitAngle = std::fmod(2.0f * PI + (Distance).polar().y, 2.0f * PI);

	if (Distance.mag2() > 32 * 32) {
		olc::vf2d Direction = (AttackTarget - Position).norm();
		Velocity = Direction * fMoveSpeed;
	}
	else if (Distance.mag2() < fAttackRange * fAttackRange && fAttackCD <= 0) {
		Graphic_State = Attacking;
		bAnimating = true; 
		if (curFrame == textureMetadata[Graphic_State].ani_len - 1) {
			PerformAttack();
			Graphic_State = Walking;
		}
		Velocity = { 0.f,0.f };
	}
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

	
	if (currentTask) {
		if (currentTask->taskName == "Repair" && Velocity.mag2() < 0.1f * 0.1f)
			Graphic_State = Build;
	}
	else
		Graphic_State = Walking; //idle


	if (repairedbuilding.lock() && Velocity.mag2() < 0.1f * 0.1f)
		bAnimating = true;

	if (fHealth <= 0)
		Graphic_State = Dead;

	if (Graphic_State == Dead && curFrame == textureMetadata[Graphic_State].ani_len - 1)
		Destroy();

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



		if (Target.has_value()) {
			std::queue<olc::vf2d> ShowQue = MoveQue;
			while (!ShowQue.empty()) {
				if (ULogic == Attack) {
					gfx->DrawLineDecal((Position), ShowQue.front(), olc::RED);
				}
				else
					gfx->DrawLineDecal((Position), ShowQue.front(), olc::WHITE);
				ShowQue.pop();
			}
			
		}
	}
}