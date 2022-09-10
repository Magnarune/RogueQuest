#include "Unit.h"
#include "Engine.h"

Unit::Unit(): vUnitPosition({0.f,0.f}), vUnitVelocity({0.f,0.f}), vTarget({0.f,0.f}), fUnitAngle(0.f) {}
Unit::~Unit() {}
void Unit::MarchingtoTarget(olc::vf2d Target)
{
	vTarget = Target;

}
void Unit::CheckCollision(std::vector<std::shared_ptr<Unit>> unitList) {

	auto Collisonlymda = [](olc::vf2d pos1, float Collision_Radius1, olc::vf2d pos2, float Collision_Radius2)
	{
		return fabs((pos1.x - pos2.x) * (pos1.x - pos2.x) + (pos1.y - pos2.y) * (pos1.y - pos2.y)) <= (Collision_Radius1 + Collision_Radius2) * (Collision_Radius1 + Collision_Radius2);
	};
	for (auto& unit : unitList)
	{
		for (auto& Test : unitList)
		{
			if (unit->vUnitPosition != Test->vUnitPosition)//dont check your self
			{
				if (Collisonlymda(unit->vUnitPosition, unit->Unit_Collision_Radius, Test->vUnitPosition, Test->Unit_Collision_Radius))
				{
					float fDistance = sqrtf((unit->vUnitPosition.x - Test->vUnitPosition.x) * (unit->vUnitPosition.x - Test->vUnitPosition.x) +
						(unit->vUnitPosition.y - Test->vUnitPosition.y) * (unit->vUnitPosition.y - Test->vUnitPosition.y));
					float fOverlap = 0.5f * (fDistance - unit->Unit_Collision_Radius - Test->Unit_Collision_Radius);
					unit->vUnitPosition.x -= fOverlap * (unit->vUnitPosition.x - Test->vUnitPosition.x) / fDistance;
					unit->vUnitPosition.y -= fOverlap * (unit->vUnitPosition.y - Test->vUnitPosition.y) / fDistance;

					Test->vUnitVelocity.x += fOverlap * (unit->vUnitPosition.x - Test->vUnitPosition.x) / fDistance;
					Test->vUnitVelocity.y += fOverlap * (unit->vUnitPosition.y - Test->vUnitPosition.y) / fDistance;
				}
			}
		}
	}
}
void Unit::UpdateUnit(float fElapsedTime) {
	m_fTimer += fElapsedTime;
	if (m_fTimer >= 0.1f) {
		m_fTimer -= 0.1f;
		++curFrame %= textureMetadata[Graphic_State].ani_len; // if AnimationLength = 9 this gives {0,1,2,3,4,5,6,7,8}
		//if(vUnitVelocity.mag2() < 1.f){
		//	vUnitVelocity = {0.f, 0.f};
		//}
	}
	olc::vf2d Distance = vTarget - vUnitPosition;
	//olc::vf2d Direction = 
	//float friction = 0.1f;	
	//vUnitVelocity += {float(rand() % 60), float(rand() % 60)};
	//vUnitVelocity *= std::pow(friction, fElapsedTime);
	vUnitPosition += vUnitVelocity * fElapsedTime;
	if (fHealth <= 0)
		Graphic_State = Dead;
}
void Unit::DrawUnit(olc::TileTransformedView* gfx) {
	const auto& meta = textureMetadata[Graphic_State];
	olc::vi2d SpriteSheetOffset = { 0, 0 };
	olc::vi2d SpriteSheetTileSize = meta.tile_size;
	const olc::vf2d& SpriteScale = meta.scale;
	olc::vf2d Origin = olc::vf2d(SpriteSheetTileSize) * SpriteScale / 2.f;
	//Calculate graphic facing Direction
	if(sClock.getMilliseconds() > 100.f){
		if(vUnitVelocity != olc::vf2d( { 0.0f, 0.0f }))
			fUnitAngle = std::fmod(2.0f*PI + vUnitVelocity.polar().y, 2.0f*PI);
		if(!std::isnan(fUnitAngle)){
			if (fUnitAngle >= 7.0f*PI/4.0f || fUnitAngle < PI/4.0f)
				FacingDirection = East;
			if (fUnitAngle >= PI/4.0f && fUnitAngle <= 3.0f*PI/4.0f)
				FacingDirection = South;
			if (fUnitAngle >= 3.0f*PI/4.0f && fUnitAngle <= 5.0f*PI/4.0f)
				FacingDirection = West;
			if (fUnitAngle >= 5.0f*PI/4.0f && fUnitAngle <= 7.0f*PI/4.0f)
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
		SpriteSheetOffset, SpriteSheetTileSize, SpriteScale, bSelected ? olc::WHITE : olc::GREY);
	vUnitVelocity = { 0.0f,0.0f };
	
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




