#include "Hero.h"
#include "Engine.h"

Hero::Hero(){

}

Hero::~Hero() {

}

bool Hero::OnCollision(std::shared_ptr<Collidable> other, olc::vf2d vOverlap) { 
	return Unit::OnCollision(other, vOverlap); 
}

void Hero::Stop() {
	Unit::Stop();
}

void Hero::Update(float delta) {	
	UpdatePosition(delta);
}

void Hero::UpdatePosition(float delta) {
	/*Done By Player*/
	if(unitAttacking)
		PerformAttack();
}

void Hero::AfterUpdate(float delta) {
	Collidable::AfterUpdate(delta);
	UnitGraphicUpdate(delta);
}

void Hero::PerformAttack() {
	auto& engine = Game_Engine::Current();
	Graphic_State = Attacking;
	if (curFrame == textureMetadata[Graphic_State].ani_len - 1) {
		if (bIsRanged){
			engine.worldManager->GenerateProjectile(engine.assetManager->GetUnitData(sUnitName).projectileName, engine.unitManager->This_shared_pointer(Position), targetUnit);
		}

		unitAttacking = false;		
	}	
}

void Hero::UnitGraphicUpdate(float delta) {
	bAnimating = true;
	if (Velocity.mag2() < 0.1f * 0.1f && Graphic_State != Dead && Graphic_State != Attacking)
		bAnimating = false;

	if (Graphic_State == Dead && curFrame == textureMetadata[Graphic_State].ani_len - 1) {
		Stop();
		Destroy();
	}
	if (Health <= 0) {
		Graphic_State = Dead;
		Stop();
	}

	if (Last_State != Graphic_State) {
		curFrame = 0;
		Last_State = Graphic_State;
	}
	if (unitAttacking)
		Graphic_State = Attacking;
	else
		Walking;


	if (bAnimating)
		m_fTimer += delta;

	if (m_fTimer >= 0.1f) {
		m_fTimer -= 0.1f;
		++curFrame %= textureMetadata[Graphic_State].ani_len;
	}
}

void Hero::Draw(olc::TileTransformedView* gfx) {

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
	switch (Graphic_State) {
	case Walking:
		SpriteSheetOffset.y = Direction[FacingDirection] * SpriteSheetTileSize.y;
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
	gfx->DrawPartialDecal((Position - SpriteOrigin), meta.target_size, decals[Graphic_State].get(),
		SpriteSheetOffset, SpriteSheetTileSize,
		(bSelected ? olc::WHITE : engine.highlightmanagment->OwnerColor(Owner)) - engine.worldManager->currentMap->Darkness);




}
