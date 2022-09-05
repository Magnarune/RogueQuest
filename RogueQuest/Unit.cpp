#include "Unit.h"


Unit::Unit()
{
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

Unit::~Unit()
{

}

void Unit::DrawUnit(olc::PixelGameEngine* gfx, olc::vf2d Offset , float fElapsedTime)
{
	olc::vi2d nSpriteSheetOffset = { 0,0 };

	m_fTimer += fElapsedTime;
	if (m_fTimer >= 0.2f)
	{
		m_fTimer -= 0.2f;
		curFrame++;
		curFrame %= AnimationLengths[Graphic_State] + 1;// if AnimationLength = 9 this gives {0,1,2,3,4,5,6,7,8}
	}

	if (fHealth <= 0)
		Graphic_State = Dead;
	//Calculate facing Direction
	if (fUnitAngle >= 0 || fUnitAngle >= 5.499f)
		FacingDirection = East;
	if (fUnitAngle >= 0.7854f)
		FacingDirection = North;
	if (fUnitAngle >= 1.571f)
		FacingDirection = West;
	if (fUnitAngle >= 3.927f && fUnitAngle <= 5.498f)
		FacingDirection = South;

	if(vUnitVelocity.mag2() > 0){
		nSpriteSheetOffset.y = FacingDirection * SpriteSheetTileSize.y;
		nSpriteSheetOffset.x = curFrame * SpriteSheetTileSize.x;//Start at 0 and go mod N to loop animation
	} else {
		nSpriteSheetOffset.y = FacingDirection * SpriteSheetTileSize.y;//Say South = 3 & Spritetile = 64 * 64 so its 192 but the X needs to be 0?
	}

	gfx->DrawPartialDecal((vUnitPosition - Offset) * SpriteSheetTileSize, SpriteSheetTileSize,
						decals[Graphic_State].get(), nSpriteSheetOffset, SpriteSheetTileSize, bSelected ? olc::GREY : olc::WHITE);
}
