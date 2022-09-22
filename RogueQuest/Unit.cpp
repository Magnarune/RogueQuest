#include "Unit.h"
#include "Engine.h"


Unit::Unit() : WorldObject(),
	vTarget({ 0.f,0.f }), fUnitAngle(0.f), Graphic_State(Unit::Walking), Last_State(Unit::Walking), curFrame(0) {

}

Unit::~Unit() {}

void Unit::MarchingtoTarget(olc::vf2d Target){
	vTarget = Target;
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
void Unit::CheckCollision() {
	auto& engine = Game_Engine::Current();
	
	auto Collisonlymda = [](olc::vf2d pos1, float Collision_Radius1, olc::vf2d pos2, float Collision_Radius2){
		return fabs((pos1.x - pos2.x) * (pos1.x - pos2.x) + (pos1.y - pos2.y) * (pos1.y - pos2.y)) <= (Collision_Radius1 + Collision_Radius2) * (Collision_Radius1 + Collision_Radius2);
	};

	engine.worldManager->IterateObjects([&](std::shared_ptr<WorldObject> obj) {
		auto other = std::dynamic_pointer_cast<Unit>(obj);
		if (other == nullptr || other.get() == this || Position == other->Position) return true; // act as a continue

		if (Collisonlymda(Position, Unit_Collision_Radius, other->Position, other->Unit_Collision_Radius)) {
			float fDistance = sqrtf((Position.x - other->Position.x) * (Position.x - other->Position.x) +
				(Position.y - other->Position.y) * (Position.y - other->Position.y));
			float fOverlap = 0.5f * (fDistance - Unit_Collision_Radius - other->Unit_Collision_Radius);
			Position.x -= fOverlap * (Position.x - other->Position.x) / fDistance;
			Position.y -= fOverlap * (Position.y - other->Position.y) / fDistance;
			other->Velocity.x += fOverlap * (Position.x - other->Position.x) / fDistance;
			other->Velocity.y += fOverlap * (Position.y - other->Position.y) / fDistance;
		}

		return true;
	}); // a for loop wrapper for iterating all objects
}

void Unit::PerformAttack() {
	Graphic_State = Attacking;

	//Projectilemanager->AddProjectile(Unit, AttackTarget)

	if(!Hunted.expired()){
		auto obj = Hunted.lock();

		if(auto unit = std::dynamic_pointer_cast<Unit>(obj)){
			olc::vf2d vel = (unit->Position - Position).norm() * fAttackDamage * 0.f;
			if(std::isnan(vel.x) || std::isnan(vel.y)) vel = {0.f,0.f};
			unit->KnockBack(fAttackDamage, vel);
		}
		/*
		if(auto building = std::dynamic_pointer_cast<Building>(obj)){

		}
		*/
	}
	
	fAttackCD = fSpellCooldown; // reset time for next attack
}

void Unit::KnockBack(float damage, olc::vf2d velocity) {
	HitVelocity += velocity;
	fHealth -= damage;
}

// Override Methods

void Unit::Destroy() {
	WorldObject::Destroy(); // inherit
}

void Unit::Update(float fElapsedTime) {
	WorldObject::Update(fElapsedTime); // inherit
	
	CheckCollision();

	if(fAttackCD > 0)
		fAttackCD -= fElapsedTime;

	m_fTimer += fElapsedTime;
	if (m_fTimer >= 0.1f){
		m_fTimer -= 0.1f;
		++curFrame %= textureMetadata[Graphic_State].ani_len; // if AnimationLength = 9 this gives {0,1,2,3,4,5,6,7,8}
	}

	if(Graphic_State != Dead){
		UnitBehaviour();
		if (Hunted.expired()){ // not hunting anything
			olc::vf2d Distance = vTarget - Position;
			if(Graphic_State != Walking) Graphic_State = Walking;
			if (vTarget != olc::vf2d({ 0.f, 0.f })){
				olc::vf2d Direction = (vTarget - Position).norm();
				Velocity = Direction * fMoveSpeed;
				if (Distance.mag2() < 64) // 8px
					vTarget = olc::vf2d({ 0.f,0.f });
			} else {
				Velocity = {0.f, 0.f};
			}
		} else { // hunting some world object
			olc::vf2d Distance = AttackTarget - Position;
			if(Distance.mag2() > 32*32){
				olc::vf2d Direction = (AttackTarget - Position).norm();
				Velocity = Direction * fMoveSpeed; //Go to Target
			} else {
				Velocity = {0.f, 0.f};
			}
			if (Distance.mag2() < fAttackRange*fAttackRange && fAttackCD <= 0){
				PerformAttack();
				Velocity = { 0.f,0.f };
			}
		}
	}
	
	Position += (Velocity + HitVelocity) * fElapsedTime;

	HitVelocity *= std::pow(0.05f, fElapsedTime);

	if(HitVelocity.mag2() < 4.f)
		HitVelocity = {0.f, 0.f};

	if (Graphic_State == Dead && curFrame == textureMetadata[Graphic_State].ani_len - 1)
		Destroy();

	if (fHealth <= 0)
		Graphic_State = Dead;

	if(Last_State != Graphic_State){
		curFrame = 0;
		Last_State = Graphic_State;
	}
}

void Unit::Draw(olc::TileTransformedView* gfx){
	WorldObject::Draw(gfx); // inherit
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
	//Direction[FacingDirection + 1];

	switch (Graphic_State){
	case Walking:
		if (sClock.getMilliseconds() > 100.f) {
			sClock.restart();
		}
		if (Velocity.mag2() > 0.1f) {
			SpriteSheetOffset.y = Direction[FacingDirection] *SpriteSheetTileSize.y;
			SpriteSheetOffset.x = curFrame * SpriteSheetTileSize.x;
		} else {
			SpriteSheetOffset.y = Direction[FacingDirection] * SpriteSheetTileSize.y;
		}
		break;
	case Dead:
		SpriteSheetOffset.x = curFrame * SpriteSheetTileSize.x;		
		break;
	case Attacking:
		SpriteSheetOffset.y = Direction[FacingDirection] * SpriteSheetTileSize.y;
		SpriteSheetOffset.x = curFrame * SpriteSheetTileSize.x;
		break;
	}
	gfx->DrawPartialDecal((Position - Origin), decals[Graphic_State].get(),
		SpriteSheetOffset, SpriteSheetTileSize, SpriteScale, bSelected ? olc::WHITE : olc::GREY);
	gfx->DrawLineDecal(Position - olc::vf2d({ 10.f, 10.f }) * float((fHealth / fMaxHealth)), Position + olc::vf2d({ 5.f , -10.f }) * float((fHealth / fMaxHealth)), olc::RED);
	//gfx->DrawLineDecal(Position - olc::vf2d({ 10.f, 11.f }), Position + olc::vf2d({ 5.f, -11.f }), olc::RED);
}