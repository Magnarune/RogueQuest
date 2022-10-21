#include "Particles.h"
#include "Engine.h"
#include <iostream>
#include <vector>
#include <algorithm>



Particles::Particles() {}

Particles::~Particles() {
	Instance.clear();
	EffectPositions.clear();
	effects.clear();
}


void Particles::loadImage(const std::string& name, const std::string& path, int animation) {
	std::unique_ptr<olc::Decal> ptr;
	ptr.reset(
		new olc::Decal(
			TextureCache::GetCache().GetTexture(
				TextureCache::GetCache().CreateTexture(path)
			)
		)
	);
	Effectdecals.insert_or_assign(name, std::move(ptr));
	animationlength.insert({ name,animation});
}

void Particles::loadImage(const std::string& name, size_t tex_id) {
	std::unique_ptr<olc::Decal> ptr;
	ptr.reset(
		new olc::Decal(
			TextureCache::GetCache().GetTexture(tex_id)
		)
	);
	Effectdecals.insert_or_assign(name, std::move(ptr));
}

void Particles::ImportEffectsAssets() {
	loadImage("Fire", "Assets/Effects/flames.png", 12);
	loadImage("Smoke", "Assets/Effects/smoke.png", 0);
}

void Particles::GenerateSmoke(olc::vf2d position, olc::vf2d Size, bool dust) {//size of building
	int smokeCount = 75;
	int RANDOM_SPREAD_VAL = 10; //Spread out between this amount;
	for (int i = 0; i < smokeCount; i++) {
		smokeParticles.push_back(
			/*Size*/{ {20,range(16,40)},
			/*Position*/{range(position.x - (float)Size.x / 2.f ,position.x + (float)Size.x / 2.f ),
							range(position.y - (float)Size.y / 2.f ,position.y + (float)Size.y / 2.f) }
			,/*Rise Speed*/range(1,3),/*Fade Speed*/range(60,100),dust });
	}
	//Instance.push_back({ 2.f, "Smoke" });
}

void Particles::GenerateEffect(std::string Effectname, std::shared_ptr<Collidable> Object) {
	auto& engine = Game_Engine::Current();	
	std::weak_ptr<Building> tbuild;
	std::weak_ptr<Unit> tUnit;
	engine.unitManager->ParseObject(Object, tbuild,tUnit);
	std::weak_ptr<Effect> newEffect;
	newEffect.lock()->animationlength = animationlength[Effectname];
	for (int i = 0; i < 5; i++) {
		if(tbuild.lock())
			newEffect.lock()->Positions.push_back(olc::vf2d(tbuild.lock()->Position.x + rand() % (int)tbuild.lock()->Size.x,
				tbuild.lock()->Position.y + rand() % (int)tbuild.lock()->Size.y));
	}
	if (Effectname == "Fire")
		newEffect.lock()->Duration = -1.f;
	effects.push_back(newEffect);
	
}

void Particles::GenerateBlood(olc::vf2d spawn) {
	for (int i = 0; i < 15; i++) {
		olc::vf2d Rand{ float(rand() % 4 - 2), float(rand() % 4 - 2) + 3.f };
		EffectPositions.push_back(spawn + Rand);
	}
	Instance.push_back(0.25f);
}

void Particles::UpdateParticles(float delta) {
	auto& engine = Game_Engine::Current();

	for (int i = 0; i < Instance.size(); i++) {

		for (int i = 0; i < 15; i++) {
			EffectPositions[i].x += rand() % 5 - 2.f;
			EffectPositions[i].y += (rand() % 2) * 0.4f;
		}
		Instance[i] -= Instance[i] * delta;
	}
	
	

	for (int i = 0; i < smokeParticles.size(); i++) {
		smokeParticles[i].update(delta);
		if (smokeParticles[i].expired()) {
			smokeParticles.erase(smokeParticles.begin() + i--);
		}
	}

	for (int i = 0; i < effects.size(); i++) {
		if (effects[i].lock()) {
			if (delta > 0.2f) {
				effects[i].lock()->frame %= (effects[i].lock()->animationlength - 1);
				delta = 0.f;
			}
		} else {
			
		}
	}
	Destroy();

}

void Particles::DrawParticles() {
	auto& engine = Game_Engine::Current();
	for (int j = 0; j < Instance.size(); j++) {	
		for (int i = 0; i < EffectPositions.size(); i++) {
			engine.tv.DrawDecal(EffectPositions[i], engine.hud->decals["Pixel"].get(), { 1.f,1.f }, olc::PixelF(1.f, 0, 0, Instance[j] / 0.25f));
		}		
	}

	for (int i = 0; i < smokeParticles.size(); i++) {			
			engine.tv.DrawWarpedDecal(Effectdecals["Smoke"].get(), { {smokeParticles[i].pos.x,smokeParticles[i].pos.y},{smokeParticles[i].pos.x,smokeParticles[i].pos2.y},
				{smokeParticles[i].pos2.x,smokeParticles[i].pos2.y},{smokeParticles[i].pos2.x,smokeParticles[i].pos.y} },smokeParticles[i].Dusty ? smokeParticles[i].col : smokeParticles[i].colD);
		
	}

	for (int j = 0; j < effects.size(); j++) {
		for (int i = 0; i < effects[j].lock()->Positions.size(); i++) {
			engine.tv.DrawPartialDecal(effects[j].lock()->Positions[i], { 32.f,12.f }, Effectdecals["Fire"].get(), { 24.f * (float)effects[j].lock()->frame,0.f }, { 64.f,24.f });
		}
	}
}

void Particles::Destroy() {
	for (int j = 0; j < Instance.size(); j++) {		
		if (Instance[j] < 0.15f) {
			if (Instance.size() > 1)
				std::swap(Instance.front(), Instance.back());
			Instance.pop_back();
			EffectPositions.erase(EffectPositions.begin(), EffectPositions.begin() + 15);
		}
	}	
}