#pragma once

#include "Unit.h"
#include "WorldObject.h"
#include "WorldObjectManager.h"
#include <queue>

class Projectile : public Collidable
{
	float PI = 3.14159265358f;
	void loadImage(const std::string& name, const std::string& path);
	void loadImage(const std::string& name, size_t tex_id);
	std::map<std::string, std::unique_ptr<olc::Decal>> Pdecals;
public:
	Projectile();
	void ImportProjectileAssets();
	void Destroy() override;
	bool OnCollision(std::shared_ptr<Collidable> other, olc::vf2d vOverlap) override { return false; };
	void Update(float fElapsedtime) override;
	void Draw(olc::TileTransformedView* gfx) override;
virtual ~Projectile();
olc::vf2d direction;
	float Damage; //Damage of projectile
	bool Spinning;
	bool Traveling;
	float Arrowspeed = 20.f;

	friend class WorldManager;
	friend class TaskManager;
	friend class UnitManager;
	friend class BuildingManager;
};

