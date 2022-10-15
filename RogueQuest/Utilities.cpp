#include "Utilities.h"
#include "Engine.h"

Circle::Circle(float res) : resolution(res), position({}) {
	auto& engine = Game_Engine::Current();
	sprite.reset(new olc::Sprite((int)resolution, (int)resolution));
	engine.SetDrawTarget(sprite.get());
	engine.Clear(olc::BLANK);
	engine.FillCircle(int(res / 2.f), int(res / 2.f), int(res / 2.f));
	engine.SetDrawTarget(nullptr);
	decal.reset(new olc::Decal(sprite.get()));
}

void Circle::Draw() const {
	auto& engine = Game_Engine::Current();
	engine.tv.DrawDecal(position - radius, decal.get(), radius * 2.f / resolution, col);
}