#include "WorldObject.h"
#include "Engine.h"
#include "WorldObjectManager.h"
#include <cmath>
#include <numbers>
#include <type_traits>

/*
    World Object
*/


WorldObject::WorldObject(): Position({512.f,480.f }), Velocity({0.f, 0.f}), updatePriority(1.f) {
}

WorldObject::~WorldObject() {
}

void WorldObject::Destroy() {
    auto& engine = Game_Engine::Current();
    engine.worldManager->DestroyObject(this);
}


void WorldObject::Update(float delta) {
}

void WorldObject::AfterUpdate(float delta) {
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

/*
	World Collidable Object
*/

Collidable::Collidable(): WorldObject() {

}

Collidable::~Collidable() {
}

Collidable::Mask::Mask() : type(MASK_NONE), origin({ 0.f, 0.f }) {}
Collidable::Mask::Mask(olc::vf2d rect): type(MASK_RECTANGLE), rect(rect), origin({ 0.f, 0.f }) {}
Collidable::Mask::Mask(float radius): type(MASK_CIRCLE), radius(radius), origin({ 0.f, 0.f }) {}

void Collidable::SetMask(const Mask& mask) {
	
	this->mask = mask;
}

void Collidable::Update(float delta) {
	WorldObject::Update(delta); // inherit

	predPosition = Position + Velocity * delta; // set the next potential position to jump to
	Position = predPosition; // temporary assignment of ghosting position for post step collision calculations (it's complicated, but this is absolutely necessary or it won't work)
	CheckCollision(delta); // collision checking
}

void Collidable::AfterUpdate(float delta) {
	Position = predPosition; // finalize position after calcuations are made
	WorldObject::AfterUpdate(delta); // inherit
}

bool Collidable::CheckCollision(float delta) {
	auto& engine = Game_Engine::Current();
	if (mask.type == Mask::MASK_NONE) return false; // don't continue if we don't even have a mask

	const olc::vf2d& nextPos = predPosition; // next frame position
	auto checkCvC = [&](std::shared_ptr<Collidable> other, float r1, float r2, olc::vf2d origin) {
		olc::vf2d ray = (other->Position - nextPos);
		float overlap = (r1 + r2) - ray.mag();
		if(std::isnan(overlap)) overlap = 0;

		if(overlap > 0.01f)
			return OnCollision(other, (ray.mag2() < 0.01 ? ray : ray.norm()) * overlap);

		return true;
	};

	auto checkRvC = [&](std::shared_ptr<Collidable> other, double radius, olc::vf2d size, olc::vf2d origin, bool inverse=false) {
		olc::vf2d circlePos = inverse ? nextPos : other->Position ,
				  rectPos = inverse ? other->Position - other->mask.origin : nextPos - origin;

		// nearest position
		olc::vf2d nPos = circlePos.max(rectPos).min(rectPos + size); // simplified vector clamp
		olc::vf2d ray;
		float overlap;
		if(nPos == circlePos){
			olc::vf2d tld = rectPos - circlePos;
			olc::vf2d brd = tld + size;
			olc::vf2d fPos = {FLT_MAX,FLT_MAX};

			tld = {std::fabs(tld.x), std::fabs(tld.y)};
			brd = {std::fabs(brd.x), std::fabs(brd.y)};
			float brds = std::fmin(brd.x, brd.y), tlds = std::fmin(tld.x, tld.y);

			if(tlds < brds) {
				if (tld.x < tld.y) {
					fPos.x = -tld.x;
				} else {
					fPos.y = -tld.y;
				}
			} else {
				if (brd.x < brd.y) {
					fPos.x = brd.x;
				} else {
					fPos.y = brd.y;
				}
			}

			if (std::fabs(fPos.x) < std::fabs(fPos.y)) {
				nPos.x += fPos.x;
			} else {
				nPos.y += fPos.y;
			}
			ray = circlePos - nPos;
			overlap = ray.mag() + (float)radius;
		} else {
			ray = nPos - circlePos;
			overlap = (float)radius - ray.mag();
		}
		if(std::isnan(overlap)) overlap = 0;

		if(overlap > 0.01f) // collision
			return OnCollision(other, (ray.mag2() < 0.01 ? ray : ray.norm()) * overlap);
		return true;
	};

	auto checkRvR = [&](std::shared_ptr<Collidable> other, olc::vf2d mSize, olc::vf2d oSize, olc::vf2d origin) {
		const olc::vf2d& oPos = other->Position - other->mask.origin;
		const olc::vf2d& mPos = nextPos - origin;

		olc::vf2d overlap{};
		if (mPos.x + mSize.x > oPos.x && mPos.x < oPos.x + oSize.x &&
			mPos.y + mSize.y > oPos.y && mPos.y < oPos.y + oSize.y) {
			olc::vf2d quadrant((mPos + mSize / 2.f) - (oPos + oSize / 2.f));
			bool top = quadrant.y < 0,
				left = quadrant.x < 0;
			overlap.x = left ? (mPos.x + mSize.x) - oPos.x : mPos.x - (oPos.x + oSize.x);
			overlap.y = top ? (mPos.y + mSize.y) - oPos.y : mPos.y - (oPos.y + oSize.y);

			olc::vf2d alap = { std::fabs(overlap.x),std::fabs(overlap.y) };
			if (alap.x > alap.y) overlap.x = 0.f; else overlap.y = 0.f; // find shortest distance
		}

		if (overlap.x > 0.01f || overlap.y > 0.01f) {
			return OnCollision(other, overlap);
		}
		return true;
	};


	return engine.worldManager->IterateObjects([&](std::shared_ptr<WorldObject> _obj){
		auto obj = std::dynamic_pointer_cast<Collidable>(_obj);
		if(!obj || obj.get() == this) return true;
		if(obj->mask.type == Mask::MASK_NONE) return true; // skip non-masked

		// rectangle mask
		if(obj->mask.type == Mask::MASK_RECTANGLE){
			olc::vf2d& rect = obj->mask.rect;
			if(mask.type == Mask::MASK_RECTANGLE){
				return checkRvR(obj, mask.rect, rect, mask.origin);
			}
			if(mask.type == Mask::MASK_CIRCLE){
				return checkRvC(obj, mask.radius, rect, mask.origin, true); // reverse for circle v rect
			}
		}
		if(obj->mask.type == Mask::MASK_CIRCLE){
			float& radius = obj->mask.radius;
			if(mask.type == Mask::MASK_CIRCLE){
				return checkCvC(obj, mask.radius, radius, mask.origin);
			}
			if(mask.type == Mask::MASK_RECTANGLE){
				return checkRvC(obj, radius, mask.rect, mask.origin); // just rect v circle
			}
		}

		throw std::runtime_error("Invalid collision detection reached");
	});
}