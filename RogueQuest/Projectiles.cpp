#include "Projectiles.h"


Projectile::Projectile() : Collidable(),
	Target({0.f,0.f}) {
}

Projectile::~Projectile(){

}

void Projectile::Update(float fElapsedtime) {
	m_fTimer += fElapsedtime;


	if (m_fTimer >= 0.1f) {
		m_fTimer -= 0.1f;
	//	++curFrame %= textureMetadata[Graphic_State].ani_len;
	}




	olc::vf2d direction = Position - Target;	
	Position += direction.norm() * Velocity * fElapsedtime;
}

void Projectile::Draw(olc::TileTransformedView* gfx) {
	WorldObject::Draw(gfx); // what is this?



}