#pragma once
#include "olcPixelGameEngine.h"
#include "olcPGEX_TransformedView.h"
#include "WorldObject.h"
#include <queue>

class Projectile : public Collidable {
	float PI = 3.14159265358f;
	void loadImage(const std::string& name, const std::string& path);
	void loadImage(const std::string& name, size_t tex_id);
public:
	Projectile();
	std::map<std::string, std::unique_ptr<olc::Decal>> Pdecals;
	void ImportProjectileAssets();
	void Destroy() override;
	bool OnCollision(std::shared_ptr<Collidable> other, olc::vf2d vOverlap) override { return false; };
	void Update(float fElapsedtime) override;
	void Draw(olc::TileTransformedView* gfx) override;
	void AfterUpdate(float delta) override {};
virtual ~Projectile();
olc::vf2d direction;
	float Damage; //Damage of projectile
	bool Spinning;
	bool Traveling;
	std::shared_ptr<WorldObject> cTarget;
	float Arrowspeed = 40.f;

	friend class WorldManager;
	friend class TaskManager;
	friend class UnitManager;
	friend class BuildingManager;
	friend class Game_Engine;
};

