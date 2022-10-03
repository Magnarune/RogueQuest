#include "WorldObject.h"
#include "Engine.h"
#include "WorldObjectManager.h"
#include <cmath>
#include <type_traits>

/*
    World Object
*/


WorldObject::WorldObject(): Position({512.f,480.f }), Velocity({0.f, 0.f}) {
	
	
		
}

WorldObject::~WorldObject() {

}

void WorldObject::Destroy() {
    auto& engine = Game_Engine::Current();
    engine.worldManager->DestroyObject(this);
}

void WorldObject::Update(float fElapsedTime) {

}

void WorldObject::Draw(olc::TileTransformedView* gfx) {
    /*
	auto& pge = Game_Engine::Current();

	static olc::Sprite* _ = new olc::Sprite(16 * 2, 16 * 2);
	static olc::Decal* circle = new olc::Decal(_);

	pge.SetDrawTarget(_);
	pge.Clear(olc::BLANK); 
	pge.FillCircle(_->width / 2, _->height / 2, _->width / 2);
	pge.SetDrawTarget(nullptr);

	circle->Update();
	pge.tv.DrawDecal((Position - olc::vf2d(_->width, _->height) / 2.f), circle, { 1.f,1.f }, 0x70FFFFFF);
	*/
}


// __Collisions__
Collidable::Collidable(): WorldObject() {

}

Collidable::~Collidable() {
}

Collidable::Mask::Mask(): type(MASK_NONE) {}
Collidable::Mask::Mask(olc::vf2d rect): type(MASK_RECTANGLE), rect(rect) {}
Collidable::Mask::Mask(float radius): type(MASK_CIRCLE), radius(radius) {}

void Collidable::SetMask(const Mask& mask) {
	this->mask = mask;
}

void Collidable::Update(float fElapsedTime) {
	WorldObject::Update(fElapsedTime); // inherit

	predPosition = Position + Velocity * fElapsedTime;

	CheckCollision(fElapsedTime);

	Position = predPosition;
}


bool Collidable::CheckCollision(float fElapsedTime) {
	auto& engine = Game_Engine::Current();

	const olc::vf2d& nextPos = predPosition; // predict the next potential space to land

	auto checkCvC = [&](std::shared_ptr<Collidable> other, float r1, float r2) {
		olc::vf2d ray = (nextPos - other->Position);
		float overlap = (r1 + r2) - ray.mag();
		if(std::isnan(overlap)) overlap = 0;

		if(overlap > 0.01f)
			return OnCollision(other, -ray.norm() * overlap);

		return true; // yay for simple
	};

	auto checkRvC = [&](std::shared_ptr<Collidable> other, double radius, olc::vf2d size, bool inverse=false) {
		olc::vf2d circlePos = inverse ? nextPos : other->Position,
				  rectPos = inverse ? other->Position : nextPos;

		// nearest position
		olc::vf2d nPos(std::clamp(circlePos.x, rectPos.x, rectPos.x + size.x), 
					   std::clamp(circlePos.y, rectPos.y, rectPos.y + size.y));		

		olc::vf2d ray(nPos - circlePos);
		float overlap = (float)radius - ray.mag();
		if(std::isnan(overlap)) overlap = 0;

		if(overlap > 0.01f) // collision
			return OnCollision(other, ray.norm() * overlap);
		return true;
	};

	auto checkRvR = [&](std::shared_ptr<Collidable> other, olc::vf2d mSize, olc::vf2d oSize) {
		const olc::vf2d& oPos = other->Position;

		olc::vf2d overlap{};
		if (nextPos.x + mSize.x > oPos.x && nextPos.x < oPos.x + oSize.x &&
			nextPos.y + mSize.y > oPos.y && nextPos.y < oPos.y + oSize.y) {
			olc::vf2d quadrant((nextPos + mSize / 2.f) - (oPos + oSize / 2.f));
			bool top = quadrant.y < 0,
				left = quadrant.x < 0;
			overlap.x = left ? (nextPos.x + mSize.x) - oPos.x : nextPos.x - (oPos.x + oSize.x);
			overlap.y = top ? (nextPos.y + mSize.y) - oPos.y : nextPos.y - (oPos.y + oSize.y);

			olc::vf2d alap = { std::fabs(overlap.x),std::fabs(overlap.y) };
			if (alap.x > alap.y) overlap.x = 0.f; else overlap.y = 0.f; // find shortest distance
		}

		if (overlap.x > 0.01f || overlap.y > 0.01f) {
			return OnCollision(other, -overlap);
		}
		return true;
	};


	return engine.worldManager->IterateObjects([&](std::shared_ptr<WorldObject> _obj){
		if(mask.type == Mask::MASK_NONE) return false; // don't continue if we don't even have a mask
		auto obj = std::dynamic_pointer_cast<Collidable>(_obj);
		if(!obj) return true;
		if(obj->mask.type == Mask::MASK_NONE) return true; // skip non-masked

		// rectangle mask
		if(obj->mask.type == Mask::MASK_RECTANGLE){
			olc::vf2d& rect = obj->mask.rect;
			if(mask.type == Mask::MASK_RECTANGLE){
				return checkRvR(obj, mask.rect, rect);
			}
			if(mask.type == Mask::MASK_CIRCLE){
				return checkRvC(obj, mask.radius, rect,true);
			}
		}
		if(obj->mask.type == Mask::MASK_CIRCLE){
			float& radius = obj->mask.radius;
			if(mask.type == Mask::MASK_CIRCLE){
				return checkCvC(obj, mask.radius, radius);
			}
			if(mask.type == Mask::MASK_RECTANGLE){
				return checkRvC(obj, radius, mask.rect);
			}
		}

		throw std::runtime_error("Invalid collision detection reached");
	});
	return true;
}