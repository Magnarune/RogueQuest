#pragma once
#include "olcPixelGameEngine.h"
#include "WorldObject.h"
#include "olcUTIL_Geometry2D.h"
class MoveLogic {
public:
	olc::vf2d Position;
	MoveLogic();
	virtual ~MoveLogic();
	std::vector<olc::vf2d> Move_Node;
	olc::vf2d Test_Node;
	void RayCaster(olc::vf2d width,olc::vf2d position,olc::vf2d target); //I cast a ray from point a to point b*

	void SearchObjects(olc::vf2d width, olc::vf2d position, olc::vf2d target);
	olc::vf2d Adjust(olc::vf2d Position_Of_Unit,olc::vf2d Width_Unit,olc::vf2d width_of_target, olc::vf2d position);
	olc::vf2d& GetClosestPoint(olc::vf2d targetPoint, std::vector<olc::vf2d>& points);
};