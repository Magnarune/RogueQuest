#include "MoveNodes.h"
#include "Engine.h"
MoveLogic::MoveLogic() {
	
}
MoveLogic::~MoveLogic() {

}

void MoveLogic::RayCaster(olc::vf2d Width,olc::vf2d position,olc::vf2d Target) {
	auto& engine = Game_Engine::Current();
    Position = position;    
    olc::vf2d searcharea = { Width.x,Width.x };//5.f
    SearchObjects(searcharea, position, Target);//Houndreds of 800.f
}

void MoveLogic::SearchObjects(olc::vf2d Width_Of_Unit, olc::vf2d Initial_Position, olc::vf2d Target_Position) {
    auto& engine = Game_Engine::Current();
    olc::utils::geom2d::line <float>Ray_to_Target = { Initial_Position, Target_Position };
    std::vector<olc::vf2d> Center_Of_Rectangles;
    Move_Node.clear();
    for (float startDist = 0.f; startDist < Ray_to_Target.length(); startDist += Width_Of_Unit.x) {
        Center_Of_Rectangles.push_back(Ray_to_Target.rpoint(startDist));
        Move_Node.push_back(Ray_to_Target.rpoint(startDist));
    }       
    
    for (int i = 1; i < Center_Of_Rectangles.size()-1; i++) {
        engine.worldManager->IterateObjectsQT(
            olc::utils::geom2d::rect<float>(  Center_Of_Rectangles[i] - Width_Of_Unit / 2.f, Width_Of_Unit),
            [&](std::shared_ptr<WorldObject>& _obj) {
                auto obj = std::dynamic_pointer_cast<Collidable>(_obj);

                if (!obj || obj.get()->Position == this->Position) return true;
                if (obj->mask.type == obj->mask.MASK_NONE) return true; // skip non-masked

                // rectangle mask
                if (obj->mask.type == obj->mask.MASK_RECTANGLE) {
                    olc::vf2d& rect = obj->mask.rect;
                    olc::vf2d& pos = obj->Position;
                    //olc::vf2d NewNode = Adjust(Position,Width_Of_Unit, rect, pos);
                    Test_Node = Adjust(Position, Width_Of_Unit, rect, pos);
                    return true;
                }

            });
    
    };
}


olc::vf2d MoveLogic::Adjust(olc::vf2d Position_of_Unit, olc::vf2d width_of_Unit, olc::vf2d Size_of_Object, olc::vf2d Position_Of_Object) {
    olc::vf2d Next_Node;
    olc::utils::geom2d::line<float> DistanceTester;
    std::vector<olc::vf2d> Mid_Point{ { Position_Of_Object.x,Position_Of_Object.y + Size_of_Object.y + width_of_Unit.y },
                                      { Position_Of_Object.x + Size_of_Object.x + width_of_Unit.x,Position_Of_Object.y },
                                      { Position_Of_Object.x,Position_Of_Object.y - Size_of_Object.y - width_of_Unit.y },
                                      { Position_Of_Object.x - Size_of_Object.x - width_of_Unit.x ,Position_Of_Object.y}};

   Next_Node = GetClosestPoint(Position_of_Unit, Mid_Point); 

    //Left to right Up and Down
   return Next_Node;
}

 olc::vf2d& MoveLogic::GetClosestPoint(olc::vf2d targetPoint, std::vector<olc::vf2d>& points) {
    std::sort(points.begin(), points.end(), [&](olc::vf2d& p1, olc::vf2d& p2) {
        float distToP1 = olc::utils::geom2d::line<float>(p1, targetPoint).length();
        float distToP2 = olc::utils::geom2d::line<float>(p2, targetPoint).length();
                             
        return distToP1 < distToP2;
        });
    return points[1];
}