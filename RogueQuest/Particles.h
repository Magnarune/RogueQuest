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
	float m_ftimer {};

	/*
		This is convoluted / redundant
		Why do you have 3 separated vectors to define the existance of particles when you already have an *Effect* class that already contains the lifetime, position, name, etc?
		This should all be removed and instead use the vector of Effect (do not use smart pointers)
	*/

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


	void GenerateEffect(std::string Effectname, olc::vf2d position, olc::vf2d Size);
	void GenerateEffect(std::string Effectname, std::shared_ptr<Collidable> Object);

	void GenerateSmoke(olc::vf2d position, olc::vf2d Size, bool Dust);
	void GenerateBlood(olc::vf2d Spawn);
	void UpdateParticles(float delta);
	void DrawParticles();
	void Destroy();

	float lastCreationTime = 0;
	std::vector<Smoke> smokeParticles;

	Particles();
	~Particles();
};




class Smoke {
	friend class Particles;
public:
    olc::Pixel col = olc::Pixel(128, 128, 128, 128); //Pefect "Building"/ "Dust" colour;
	olc::Pixel colD = olc::Pixel(80, 80, 80, 128);
    olc::vf2d size;
    olc::vf2d pos;
    olc::vf2d pos2;
    float riseSpd;
    float fadeSpd = 1; //How much alpha to drop down per second.
	bool Dusty;
    int freq = rand() % 2 + 1;
    float alpha = 128;
    Smoke(olc::vf2d size, olc::vf2d pos, float riseSpd, float fadeSpd, bool Dusty):
		size(size), riseSpd(riseSpd), fadeSpd(fadeSpd), pos(pos), pos2(pos + size / 8.f), Dusty(Dusty) {}

    void update(float fElapsedTime) {
        alpha = std::clamp(alpha - fadeSpd * fElapsedTime, 0.f, 255.f);
        pos.y -= (riseSpd + float(rand() % 1000) / 1000.0f * riseSpd * 4.f) * fElapsedTime;
        pos2.y -= (riseSpd + float(rand() % 1000) / 1000.0f * riseSpd * 3.5f) * fElapsedTime;
        col.a = (int)alpha;
    }


    bool expired() {
        return !col.a;
    }
};