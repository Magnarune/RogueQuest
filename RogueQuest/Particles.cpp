#include "Particles.h"
#include "Engine.h"
Particles::Particles() {}

Particles::~Particles() {
	pos.clear();
	timer.clear();
}

void Particles::CreateParticles(olc::vf2d spawn) {
	timer.push_back(0.25f);
	
	for (int i = 0; i < 15; i++) {
		olc::vf2d Rand{ float(rand() % 4 - 2), float(rand() % 4 - 2) + 3.f };
		pos.push_back(spawn + Rand);
	}
}

void Particles::UpdateParticles(float delta) {
	for (int i = 0; i < pos.size(); i++) {
		pos[i].x += rand() % 5 - 2.f;
		pos[i].y += (rand() % 2 ) * 0.4f;
	}	
	for (int i = 0; i < timer.size(); i++)
		timer[i] -= timer[i] * delta;
	
}

void Particles::DrawDestroyParticles() {
	auto& engine = Game_Engine::Current();
	for (int j = 0; j < timer.size(); j++) {

		if (timer[j] > 0.15f) {
			for (int i = 0; i < pos.size(); i++) {
				engine.tv.DrawDecal(pos[i], engine.hud->decals["Pixel"].get(), { 1.f,1.f }, olc::PixelF(1.f, 0, 0, timer[j] / 0.25f ));
			}		
		}
		else{
			timer.erase(timer.begin());
			pos.erase(pos.begin(), pos.begin() + 15);
		}
	}
}