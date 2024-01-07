#include "MoveNodes.h"
#include "Engine.h"
MoveLogic::MoveLogic() {
	
}
MoveLogic::~MoveLogic() {

}

void MoveLogic::RayCaster(olc::vf2d Width,olc::vf2d position,olc::vf2d Target) {
	auto& engine = Game_Engine::Current();
    std::vector<std::shared_ptr<CollisionMapObject>> test_objects;
    std::shared_ptr<CollisionMapObject> testTree;

    olc::vf2d Pos_Diff = { std::fabs(position.x - Target.x), std::fabs(position.y - Target.y) };
    float theta = atanf(Pos_Diff.y /Pos_Diff.x);
    olc::vf2d widthint = { position.x * cosf(theta),position.y * sinf(theta) };


    engine.worldManager->IterateObjectsQT({ position - olc::vf2d(200.f,200.f), {400.f,400.f} }, [&](std::shared_ptr<WorldObject> obj) -> bool {
        if (testTree = std::dynamic_pointer_cast<CollisionMapObject>(obj)) {
            test_objects.push_back(testTree);
        }
        return true;
        });


}