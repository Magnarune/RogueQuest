#include "WorldObject.h"
#include "Engine.h"
#include "WorldObjectManager.h"

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

void Collidable::Update(float fElapsedTime) {
	WorldObject::Update(fElapsedTime); // inherit

	CheckCollision(fElapsedTime);
}

bool Collidable::CheckCollision(float fElapsedTime) {


	return true;
}