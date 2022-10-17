#pragma once
#include "olcPixelGameEngine.h"
class MoveNodes{//Static
	MoveNodes();
	virtual ~MoveNodes();
	struct Node {
		bool isObstacle = false;// Is the node an obstruction?
		bool hasVisited = false; // Have we searched this node before?
		float fGlobalGoal;		 // Distance to goal so far
		float LocalGoal;		 // Distance to goal if we took the alternative route
		olc::vi2d xy;			// Nodes position in 2D space

		std::vector<std::shared_ptr<Node>> vecNeighbours;// Connections to neighbours
		std::weak_ptr<Node> parent;// Node connecting to this node that offers shortest parent
	};
	void GenerateNodes();
	std::shared_ptr<Node> nodes;
	olc::vi2d MapSize;

};

