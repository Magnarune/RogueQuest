#pragma once
#include "olcPixelGameEngine.h"
#include "WorldObject.h"
class MoveLogic {
public:
	MoveLogic();
	virtual ~MoveLogic();
	std::vector<olc::vf2d> Move_Node;

	void RayCaster(olc::vf2d width,olc::vf2d position,olc::vf2d target); //I cast a ray from point a to point b*
	void Adjust();

};