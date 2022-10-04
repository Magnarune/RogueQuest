#include "Unit.h"
#include "Engine.h"


Unit::Unit(const cAssets::UnitType& type) : Collidable(), unitType(type),
	vTarget({ 0.f,0.f }), fUnitAngle(0.f), Graphic_State(Unit::Walking), Last_State(Unit::Walking), curFrame(0) {
}

Unit::~Unit() {
}

void Unit::MarchingtoTarget(const olc::vf2d& Target){
	MoveQue.push(Target);
	vTarget = Target;
}

void Unit::Stop() {
	std::queue<olc::vf2d> cq;
	std::swap(MoveQue, cq);
	Hunted.reset();
	ULogic = Passive;
	vTarget = { 0.f,0.f };
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


		// move object via collision overlap compensation
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
	Graphic_State = Attacking;
	bAnimating = true;
	//Projectilemanager->AddProjectile(Unit, AttackTarget)

	if(!Hunted.expired()){
		auto obj = Hunted.lock();

		if(auto unit = std::dynamic_pointer_cast<Unit>(obj)){
			olc::vf2d vel = (unit->Position - Position).norm() * fAttackDamage * 0.f;
			if(std::isnan(vel.x) || std::isnan(vel.y)) vel = {0.f,0.f};
			unit->KnockBack(fAttackDamage, vel);
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

// Override Methods

void Unit::Destroy() {
	Collidable::Destroy(); // inherit
}

void Unit::Update(float delta) {
	if(fAttackCD > 0)
		fAttackCD -= delta;
	if(bAnimating) 
		m_fTimer += delta;

	if(!Hunted.expired())
		UnitHunting();
	else
		UpdatePosition(delta);
	
	Collidable::Update(delta);
}

void Unit::AfterUpdate(float delta) {
	UnitGraphicUpdate();

	Collidable::AfterUpdate(delta);// inherit
}

void Unit::UnitHunting() {

	olc::vf2d Distance = AttackTarget - Position;
	fUnitAngle = std::fmod(2.0f * PI + (Distance).polar().y, 2.0f * PI);

	if (Distance.mag2() > 32 * 32) {
		olc::vf2d Direction = (AttackTarget - Position).norm();
		Velocity = Direction * fMoveSpeed;
	}
	else if (Distance.mag2() < fAttackRange * fAttackRange && fAttackCD <= 0) {
		Graphic_State = Attacking;
		bAnimating = true; // ugh this is all so spaghetti
		if (curFrame == textureMetadata[Graphic_State].ani_len - 1) {
			PerformAttack();
			Graphic_State = Walking;
		}
		Velocity = { 0.f,0.f };
	}
}

void Unit::UpdatePosition(float delta) {

	if (Graphic_State != Dead) {
		UnitBehaviour();
		if (MoveQue.size() > 0)//has move order
			vTarget = MoveQue.front();//this is target location

			olc::vf2d Distance = vTarget - Position;//dist to target
			if (Graphic_State != Walking) Graphic_State = Walking;
			if (vTarget != olc::vf2d({ 0.f, 0.f })) {
				olc::vf2d Direction = (vTarget - Position).norm();
				Velocity = Direction * fMoveSpeed;
				bAnimating = true;
				if (Distance.mag2() < 64) {
					vTarget = { 0.f,0.f };
					if (!MoveQue.empty()) MoveQue.pop();
				}
			} else {
				Velocity = { 0.f, 0.f };
				bAnimating = false;
			}	
	}
	else {
		bAnimating = true;
	}


	HitVelocity *= std::pow(0.05f, delta);

	if (HitVelocity.mag2() < 4.f)
		HitVelocity = { 0.f, 0.f };
	else
		bAnimating = true;

	predPosition += HitVelocity * delta; // hitback only
}

void Unit::UnitGraphicUpdate() {	
	if (m_fTimer >= 0.1f) {
		m_fTimer -= 0.1f;
		++curFrame %= textureMetadata[Graphic_State].ani_len;
	}

	if (fHealth <= 0)
		Graphic_State = Dead;

	if (Graphic_State == Dead && curFrame == textureMetadata[Graphic_State].ani_len - 1)
		Destroy();

	if (Last_State != Graphic_State) {
		curFrame = 0;
		Last_State = Graphic_State;
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



		if (vTarget != olc::vf2d(0, 0)) {
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