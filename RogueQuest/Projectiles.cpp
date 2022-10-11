#include "Projectiles.h"
#include "Engine.h"

Projectile::Projectile() : Collidable() {
}

Projectile::~Projectile(){
}

void Projectile::Destroy() 
{Collidable::Destroy();}

void Projectile::Update(float fElapsedtime) {
	m_fTimer += fElapsedtime;
	if (m_fTimer >= 0.1f) {
		m_fTimer -= 0.1f;
	//	++curFrame %= textureMetadata[Graphic_State].ani_len;
	}
    if ((TargetPos - Position).mag2() < 64) {
        Destroy();
    }

    direction = (TargetPos - Position).norm();
    // Velocity = direction * Arrowspeed; // arrow speed is incorrect. This is a projectile, not an arrow. Not all projectiles are Arrows. Think of another way to do this such as generic velocity or proj speed
    Collidable::Update(fElapsedtime);
}

void Projectile::Draw(olc::TileTransformedView* gfx) {
	WorldObject::Draw(gfx);
	auto& engine = Game_Engine::Current();
    float angle = std::fmod(2.0f * PI + Velocity.polar().y, 2.0f * PI);

    gfx->DrawPartialRotatedDecal(Position, decals[projType].get(),angle, { 8.5f, 68.5f }, { 0.f,0.f }, {17.f,137.f}, {0.5f,0.5f});
}