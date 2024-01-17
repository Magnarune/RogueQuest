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
	//Test Points
	olc::vf2d dev_X;
	olc::vf2d dev_Y;
	olc::vf2d dev_Z;
	olc::vf2d dev_W;

	olc::vf2d dev2_X;
	olc::vf2d dev2_Y;
	olc::vf2d dev2_Z;
	olc::vf2d dev2_W;

	olc::vf2d dev_Pos;
	olc::vf2d dev2_XX;
	olc::vf2d dev2_YY;
	olc::vf2d dev2_ZZ;
	olc::vf2d dev2_WW;


	olc::vf2d UnitW;

	bool DevFlipper;




	void RayCaster(olc::vf2d width,olc::vf2d position,olc::vf2d target); //I cast a ray from point a to point b*

	void SearchObjects(olc::vf2d width, olc::vf2d position, olc::vf2d target);
	olc::vf2d Adjust(olc::vf2d Position_Of_Unit,olc::vf2d Width_Unit,olc::vf2d width_of_target, olc::vf2d position);
	olc::vf2d& GetClosestPoint(olc::vf2d targetPoint, std::vector<olc::vf2d>& points);
};