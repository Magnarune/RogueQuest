#pragma once
#include "olcPixelGameEngine.h"
#include "Assets.h"
#include "WorldObject.h"
#include"Unit.h"

class Hero : public Unit {
	void RepairBuilding() override {}
	void TrytoBuild(const std::string& name, const olc::vf2d& Target) override {}
	void UnitBehaviour() override {}
	void UnitSearch() override {}
	void UnitHunting() override {}
	void Deliver() override {}
	void Gather() override {}
	void Stop()override;

	void PerformAttack() override;
	void UnitGraphicUpdate(float delta)override;
	void UpdatePosition(float felapstedtime)override;
	bool OnCollision(std::shared_ptr<Collidable> other, olc::vf2d vOverlap) override;
	void Update(float fElapsedTime) override; //remake
	void AfterUpdate(float delta) override;
	void Draw(olc::TileTransformedView* gfx) override;//good

	void CheckHit();
	
public:
	Hero();
	virtual ~Hero();
	friend class Unit;
};

