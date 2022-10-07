#pragma once

#include "Unit.h"
#include "WorldObject.h"
#include <queue>

class Projectile : public Collidable
{
	Projectile();
	void Update(float fElapsedtime) override;
	void Draw(olc::TileTransformedView* gfx) override;
public:
virtual ~Projectile();
	float Damage; //Damage of projectile
	olc::vf2d Target; // Target's Position
};

