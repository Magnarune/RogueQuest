#pragma once

#include "Assets.h"
#include "WorldObject.h"
#include <queue>

class Projectile : public Collidable {

	Projectile();
	static inline const float PI = 3.14159265358f;
public:
	virtual ~Projectile();

	bool OnCollision(std::shared_ptr<Collidable> other, olc::vf2d vOverlap) override { return false; };
	void Update(float fElapsedtime) override;
	void Draw(olc::TileTransformedView* gfx) override;

	olc::vf2d direction;
	std::weak_ptr<WorldObject> TargetObj;
	float Damage {}; //Damage of projectile
	float Speed {}; //Speed of projectile
	float spincounter {};
	bool Spinning {};
	bool Traveling {};
	float PSpeed {}; //Speed of projectile

	std::map<std::string, std::string> projectType {
		{ "Arrow","Normal" }, {"Axe","ThrowAxe"}
	};

	std::string projType = "Normal"; // TO DO: Change to enum and load accordingly - OR remove altogether if deemed unecessary

private:
	std::map<std::string, std::unique_ptr<olc::Decal>> decals; // multiple textures for different states
	std::map<std::string, cAssets::ProjectileType::TextureMetaData> textureMetadata;

	friend class WorldManager;
	friend class TaskManager;
	friend class UnitManager;
	friend class BuildingManager;
	friend class Game_Engine;
};

