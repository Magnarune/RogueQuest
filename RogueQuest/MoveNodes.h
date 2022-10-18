#pragma once
#include "olcPixelGameEngine.h"
#include "WorldObject.h"
class MoveLogic {
	MoveLogic();
	virtual ~MoveLogic();
	std::vector<std::vector<int>> mapData;
	std::vector<std::weak_ptr<Collidable>> obstacles;
public:
	void addStaticObjects();
	void addDynamicObject(std::weak_ptr<Collidable> object);
	void UpdateDynamicObject();
	void removeDynamicObject();
	
	void UnitCheckPath();
};