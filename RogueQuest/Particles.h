#pragma once
#include "olcPGEX_TransformedView.h"
#include "olcPixelGameEngine.h"
#include "WorldObject.h"
#include <iostream>
#include <vector>
#include <algorithm>

class Smoke;
class Particles {
	void loadImage(const std::string& name, const std::string& path, int animation);
	void loadImage(const std::string& name, size_t tex_id);
public:
	void ImportEffectsAssets();
	std::map<std::string, std::unique_ptr<olc::Decal>> Effectdecals;//All the Effects Png
	float m_ftimer;
	//					Timer		Type
	std::vector<std::pair<float, std::string>> Instance; //.size() # of effects in game

	std::vector<olc::vf2d> EffectPositions; //List of all effect location

	std::map<std::string,int> animationlength; // gives animation length

	struct Effect {
		std::string name;
		int animationlength;
		std::vector<olc::vf2d> Positions;
		float Duration;
		int frame;
	};
	std::vector<std::weak_ptr<Effect>> effects;

	float range(float low, float high) {
		return rand() % 10000 / 10000.f * (high - low) + (low);
	}



	void RemoveEffect();

	void GenerateEffect(std::string Effectname, olc::vf2d position, olc::vf2d Size);
	void GenerateEffect(std::string Effectname, std::shared_ptr<Collidable> Object);

	void GenerateSmoke(olc::vf2d position, olc::vf2d Size, bool Dust);
	void GenerateBlood(olc::vf2d Spawn);
	void UpdateParticles(float delta);
	void DrawParticles();
	void Destroy(std::weak_ptr<Effect> eff);

	float lastCreationTime = 0;
	std::vector<Smoke> smokeParticles;

	Particles();
	~Particles();
};



#include <algorithm>

using namespace olc;

class Smoke {
	friend class Particles;
public:
    Pixel col = Pixel(128, 128, 128, 128); //Pefect "Building"/ "Dust" colour;
	Pixel colD = Pixel(80, 80, 80, 128);
    vf2d size;
    vf2d pos;
    vf2d pos2;
    float riseSpd;
    float fadeSpd = 1; //How much alpha to drop down per second.
	bool Dusty;
    int freq = rand() % 2 + 1;
    float alpha = 128;
    Smoke(vf2d size, vf2d pos, float riseSpd, float fadeSpd, bool Dusty)
        :size(size), riseSpd(riseSpd), fadeSpd(fadeSpd), pos(pos), Dusty(Dusty) {
        pos2 = pos + size / 8;
    };

    void update(float fElapsedTime) {
        alpha = std::clamp(alpha - fadeSpd * fElapsedTime, 0.f, 255.f);
        pos.y -= (riseSpd + rand() % 1000 / 1000.0F * riseSpd * 4) * fElapsedTime;
        pos2.y -= (riseSpd + rand() % 1000 / 1000.0F * riseSpd * 3.5) * fElapsedTime;
        col.a = (int)alpha;
    }


    bool expired() {
        return !col.a;
    }
};