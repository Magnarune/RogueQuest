#include "Unit.h"

Unit::Unit(std::string name, sol::table Data)
{
	sUnitName = name;
	tUnitdata = Data;
	Unit_Collision_Radius = 1.0f;
	vUnitPosition = { 10.0f,10.0f };
	vUnitVelocity = { 0.0f, 0.0f };
	vRubberBand = vUnitPosition;
	fUnitAngle = 0.0f;

	bSelected  = false;
	bOwnership = true;	
	bFriendly  = true;
	
	bLoot = false;
	bDead = false;
	fKBPower = 10.0f;
}
Unit::~Unit()
{

}
void Unit::DrawUnit(olc::PixelGameEngine* gfx, olc::vf2d Offset , float fElapsedTime)
{
	olc::vi2d nSpriteSheetOffset = { 0,0 };

	if (m_fKnockBackTimer > 0.0f)
	{
		vUnitVelocity.x = m_fKnockBackDX * fKBPower;
		vUnitVelocity.y = m_fKnockBackDY * fKBPower;
		bIsAttackable = false;
		m_fKnockBackTimer -= fElapsedTime;

		if (m_fKnockBackTimer <= 0.0f)
		{
			m_fStateTick = 0.0f;
			bIsAttackable = true;
		}
	}
	else
	{
		m_fTimer += fElapsedTime;
		if (m_fTimer >= 0.2f)
		{
			m_fTimer -= 0.2f;
			m_nGraphicCounter++;
			m_nGraphicCounter %= AnimationLength + 1;// if AnimationLength = 9 this gives {0,1,2,3,4,5,6,7,8,9}

		}

		if (vUnitVelocity.mag2() > 0)
			Graphic_State = Walking;
		else
			Graphic_State = Idle;
		if (fHealth <= 0)
			Graphic_State = Dead;
		//MovementUpdate(fElapsedTime, Neutral);
		if (fUnitAngle >= 0 || fUnitAngle >= 5.499f)
			FacingDirection = East;
		if (fUnitAngle >= 0.7854f)
			FacingDirection = North;
		if (fUnitAngle >= 1.571f)
			FacingDirection = West;
		if (fUnitAngle >= 3.927f && fUnitAngle <= 5.498f)
			FacingDirection = South;

		switch (Graphic_State)
		{
		case Idle:
			nSpriteSheetOffset.y = FacingDirection * SpriteSheetTileSize.y;//Say South = 3 & Spritetile = 64 * 64 so its 192 but the X needs to be 0?
		case Walking:
			nSpriteSheetOffset.y = FacingDirection * SpriteSheetTileSize.y;
			nSpriteSheetOffset.x = m_nGraphicCounter * SpriteSheetTileSize.x;//Start at 0 and go mod 9 to loop animation
		case Dead:
			break;
		}
		if (bSelected)
		{
			//gfx->DrawPartialDecal((vUnitPosition - Offset) * SpriteSheetTileSize, SpriteSheetTileSize, ErzaDCL, nSpriteSheetOffset, SpriteSheetTileSize, olc::GREY);
		}
		else
		{
			//gfx->DrawPartialDecal((vUnitPosition - Offset) * SpriteSheetTileSize, SpriteSheetTileSize, ErzaDCL, nSpriteSheetOffset, SpriteSheetTileSize, olc::WHITE);
		}
	}
}
