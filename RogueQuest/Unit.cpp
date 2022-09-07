#include "Unit.h"
#include "Engine.h"


Unit::Unit(): vUnitPosition({0.f,0.f}), vUnitVelocity({0.f,0.f}), vTarget({0.f,0.f}), fUnitAngle(0.f) {
//	sUnitName;
//	Unit_Collision_Radius = 1.0f;
//	vUnitPosition = { 10.0f,10.0f };
//	vUnitVelocity = { 0.0f, 0.0f };
//	vRubberBand = vUnitPosition;
//	fUnitAngle = 0.0f;
//
//	bSelected  = false;
//	bOwnership = true;	
//	bFriendly  = true;
//	
//	bLoot = false;
//	bDead = false;
//	fKBPower = 10.0f;
}

Unit::~Unit() {

}

void Unit::UpdateUnit(float fElapsedTime) {
	m_fTimer += fElapsedTime;
	if (m_fTimer >= 0.1f){
		m_fTimer -= 0.1f;
		++curFrame %= textureMetadata[Graphic_State].ani_len ; // if AnimationLength = 9 this gives {0,1,2,3,4,5,6,7,8}
		//if(vUnitVelocity.mag2() < 1.f){
		//	vUnitVelocity = {0.f, 0.f};
		//}
	}	
	//float friction = 0.1f;	
	//vUnitVelocity += {float(rand() % 60), float(rand() % 60)};
	vUnitPosition += vUnitVelocity * fElapsedTime;
	//vUnitVelocity *= std::pow(friction, fElapsedTime);
	vUnitVelocity = { 0.0f,0.0f };
	if (fHealth <= 0)
		Graphic_State = Dead;
}

void Unit::DrawUnit(olc::TileTransformedView* gfx) {
	const auto& meta = textureMetadata[Graphic_State];
	olc::vi2d SpriteSheetOffset = { 0, 0 };
	olc::vi2d SpriteSheetTileSize = meta.tile_size;
	const olc::vf2d& SpriteScale = meta.scale;
	olc::vf2d Origin = olc::vf2d(SpriteSheetTileSize) * SpriteScale / 2.f;
	/*
		North
		\   /
	West  +  East
		/   \
		South
	*/
	//Calculate graphic facing Direction
	if(sClock.getMilliseconds() > 100.f){
		if(vUnitVelocity != olc::vf2d( { 0.0f, 0.0f }))
			fUnitAngle = std::fmod(6.2831853f + vUnitVelocity.polar().y, 6.2831853f);
		if(!std::isnan(fUnitAngle)){
			if (fUnitAngle >= 5.498f || fUnitAngle < 0.7854f)
				FacingDirection = East;
			if (fUnitAngle >= 0.7854f && fUnitAngle <= 2.356f)
				FacingDirection = South;
			if (fUnitAngle >= 2.356f && fUnitAngle <= 3.927f)
				FacingDirection = West;
			if (fUnitAngle >= 3.927f && fUnitAngle <= 5.498f)
				FacingDirection = North;
		} else {
			FacingDirection = South;
		}
		sClock.restart();
	}
	if (vUnitVelocity.mag2() > 0.f) {
		SpriteSheetOffset.y = FacingDirection * SpriteSheetTileSize.y;
		SpriteSheetOffset.x = curFrame * SpriteSheetTileSize.x;//Start at 0 and go mod N to loop animation
	} else {
		SpriteSheetOffset.y = FacingDirection * SpriteSheetTileSize.y;//Say South = 3 & Spritetile = 64 * 64 so its 192 but the X needs to be 0?
	}
	gfx->DrawPartialDecal((vUnitPosition - Origin), decals[Graphic_State].get(),
		SpriteSheetOffset, SpriteSheetTileSize, SpriteScale, bSelected ? olc::GREY : olc::WHITE);
	/*
	auto& pge = Game_Engine::Current();

	static olc::Sprite* _ = new olc::Sprite(Unit_Collision_Radius * 2, Unit_Collision_Radius * 2);
	static olc::Decal* circle = new olc::Decal(_);

	pge.SetDrawTarget(_);
	pge.Clear(olc::BLANK);
	pge.FillCircle(_->width / 2, _->height / 2, _->width / 2);
	pge.SetDrawTarget(nullptr);

	circle->Update();
	pge.tv.DrawDecal((vUnitPosition - olc::vf2d(_->width, _->height) / 2.f), circle, { 1.f,1.f }, 0x70FFFFFF);
	*/
}




