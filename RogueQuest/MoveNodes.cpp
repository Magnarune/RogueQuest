#include "MoveNodes.h"
MoveLogic::MoveLogic() {
	obstacles.reserve(std::size_t(1000) * 1000);
}
MoveLogic::~MoveLogic() {
	obstacles.clear();
}

void MoveLogic::addStaticObjects(){

}

void MoveLogic::addDynamicObject(std::weak_ptr<Collidable> object){
	obstacles.emplace_back(object);
}

void MoveLogic::UpdateDynamicObject(){

}

void MoveLogic::removeDynamicObject(){

}

void MoveLogic::UnitCheckPath(){

}
