#include "Projectiles.h"
#include "Engine.h"

Projectile::Projectile() : Collidable() {
}

Projectile::~Projectile(){
    TargetPos.reset();
}

void Projectile::Destroy() {
    Collidable::Destroy();
}

void Projectile::Update(float fElapsedtime) {
    auto& engine = Game_Engine::Current();
	m_fTimer += fElapsedtime;
	if (m_fTimer >= 0.1f) {
        spincounter += 6.f*m_fTimer;       
        m_fTimer -= 0.1f;
	       
	}
    if (TargetPos.lock()) {
        if ((TargetPos.lock()->Position - Position).mag2() < 64) {
            TargetPos.lock()->Health -= Damage;
            engine.particles->GenerateBlood(TargetPos.lock()->Position);
            Destroy();
        }
    }
    else
        Destroy();
    if(TargetPos.lock())
        direction = (TargetPos.lock()->Position - Position).norm();
     Velocity = direction * PSpeed; // arrow speed is incorrect. This is a projectile, not an arrow. Not all projectiles are Arrows. Think of another way to do this such as generic velocity or proj speed
    Collidable::Update(fElapsedtime);
}

void Projectile::Draw(olc::TileTransformedView* gfx) {
    WorldObject::Draw(gfx);
    auto& engine = Game_Engine::Current();
    float angle = std::fmod(2.0f * PI + Velocity.polar().y, 2.0f * PI);
    if(Spinning == false)
        gfx->DrawPartialRotatedDecal(Position, decals[projType].get(), angle + PI/2.f, { 8.5f, 68.5f }, { 0.f,0.f }, { 17.f,137.f }, { 0.1f,0.1f });
    else
        gfx->DrawPartialRotatedDecal(Position, decals[projType].get(),  spincounter + PI / 2.f, { 7.f, 16.f }, { 0.f,0.f }, { 14.f,32.f }, { 0.5f,0.5f });
}