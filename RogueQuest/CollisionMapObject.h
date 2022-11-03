#pragma once
#include "WorldObject.h"
#include "olcPixelGameEngine.h"
#include "Map.h"
class CollisionMapObject : public Collidable {
	
public:
	void Draw(olc::TileTransformedView* gfx) override;
	bool OnCollision(std::shared_ptr<Collidable> other, olc::vf2d vOverlap) override;
	void Update(float delta) override;
	void AfterUpdate(float delta) override;
	
	//draw data
	//olc::vf2d Origin;//Center's image 
	//olc::vf2d Size;//Size of image
	//olc::vf2d sourcePos;
	//olc::vf2d sourceSize;
	//std::unique_ptr<olc::Decal> objectdecal;//the image
	//
	std::string name;
	int Lumber=100;//just a tree
	CollisionMapObject();
	virtual ~CollisionMapObject();
};