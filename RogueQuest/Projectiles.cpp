#include "Projectiles.h"
#include "Engine.h"

Projectile::Projectile() : Collidable() {
}

Projectile::~Projectile(){
    TargetObj.reset();
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
    if (!TargetObj.expired()) {
        auto obj = TargetObj.lock();
        if ((obj->Position - Position).mag2() < 64) {
            if(auto build = std::dynamic_pointer_cast<Building>(obj)){
                build->Health -= Damage;
            }
            if(auto unit = std::dynamic_pointer_cast<Unit>(obj)){
                unit->Health -= Damage;
            }

            engine.particles->GenerateBlood(obj->Position);
            Destroy();
        }
        direction = (obj->Position - Position).norm();
    } else {
        Destroy();
    }
    Velocity = direction * PSpeed; 
    Collidable::Update(fElapsedtime);
    drawDepth = Position.y / 4000.f;
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