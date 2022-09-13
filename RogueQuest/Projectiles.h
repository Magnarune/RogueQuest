#pragma once

#include "Unit.h"
#include "WorldObject.h"


class Projectile : public WorldObject
{
	Projectile();
	void Update(float fElapsedtime) override;
	void Draw(olc::TileTransformedView* gfx) override;
public:
virtual ~Projectile();
	float Damage; //Damage of projectile
	olc::vf2d Target; // Target's Position
};

