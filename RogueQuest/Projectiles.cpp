#include "Projectiles.h"


Projectile::Projectile() : WorldObject(),
	Target({0.f,0.f}) {
}

Projectile::~Projectile(){

}

void Projectile::Update(float fElapsedtime) {
	olc::vf2d direction = Position - Target;	
	Position += direction.norm() * Velocity * fElapsedtime;
}

void Projectile::Draw(olc::TileTransformedView* gfx) {
	WorldObject::Draw(gfx); // inherit
}