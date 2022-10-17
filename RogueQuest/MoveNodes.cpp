#include "MoveNodes.h"
#include "Engine.h"

MoveNodes::MoveNodes() {

}

MoveNodes::~MoveNodes()
{
}

void MoveNodes::GenerateNodes() {
	//auto& engine = Game_Engine::Current();
	//MapSize = engine.worldManager->curMap().layerSize;
	//nodes.reset(new Node[MapSize.x*MapSize.y]);
	//for (int x = 0; x < MapSize.x; x++)
	//	for (int y = 0; y < MapSize.y; y++)
	//	{
	//		nodes[y * MapSize.x + x].x = x; // ...because we give each node its own coordinates
	//		nodes[y * MapSize.x + x].y = y;
	//		nodes[y * MapSize.x + x].bObstacle = false;
	//		nodes[y * MapSize.x + x].parent = nullptr;
	//		nodes[y * MapSize.x + x].bVisited = false;
	//	}

}