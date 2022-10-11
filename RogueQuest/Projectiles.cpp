#include "Projectiles.h"
#include "Engine.h"

Projectile::Projectile() : Collidable() {
}

Projectile::~Projectile(){
    Pdecals.clear();
}

void Projectile::loadImage(const std::string& name, const std::string& path) {
    std::unique_ptr<olc::Decal> ptr;
    ptr.reset(
        new olc::Decal(
            TextureCache::GetCache().GetTexture(
                TextureCache::GetCache().CreateTexture(path)
            )
        )
    );
    Pdecals.insert_or_assign(name, std::move(ptr));
};

void Projectile::loadImage(const std::string& name, size_t tex_id) {
    std::unique_ptr<olc::Decal> ptr;
    ptr.reset(
        new olc::Decal(
            TextureCache::GetCache().GetTexture(tex_id)
        )
    );
    Pdecals.insert_or_assign(name, std::move(ptr));
};

void Projectile::ImportProjectileAssets() {
    loadImage("Arrow", "Assets/Projectiles/Arrows/arrow.png");

}

void Projectile::Destroy() {
    Collidable::Destroy();
}

void Projectile::Update(float fElapsedtime) {
	m_fTimer += fElapsedtime;
	if (m_fTimer >= 0.1f) {
		m_fTimer -= 0.1f;
	//	++curFrame %= textureMetadata[Graphic_State].ani_len;
	}
    if ((Target.value() - Position).mag2() < 64) {
        Destroy();
    }

    direction = (Target.value() - Position).norm();
    Velocity = direction * Arrowspeed;
    Collidable::Update(fElapsedtime);
}

void Projectile::Draw(olc::TileTransformedView* gfx) {
	WorldObject::Draw(gfx);
	auto& engine = Game_Engine::Current();
    float angle = std::fmod(2.0f * PI + Velocity.polar().y, 2.0f * PI);

    gfx->DrawPartialRotatedDecal(Position, Pdecals["Arrow"].get(),angle, { 8.5f, 68.5f }, { 0.f,0.f }, {17.f,137.f}, {0.5f,0.5f});
}