/*
#pragma once
#include "olcPixelGameEngine.h"
#include <cmath>
#include <algorithm>
//#include "anisprite.h"
#include "clock.h"

namespace ParticleMemory {
	extern const size_t& activeParticles;
	extern void InitParticleMemory();
	extern void FreeParticleMemory();
}

struct ParticleType {
	const char* type;
	olc::vf2d position, velocity, scale;
	float alpha, lifetime, fadetime, friction,
		angle = 0.f, animationSpeed = -1.f, fadeIn;

	ParticleType() = default;

	ParticleType(const char* type,
		olc::vf2d position, olc::vf2d velocity, olc::vf2d scale,
		float alpha, float lifetime, float fadetime = 1.0f, float friction = 1.0f, float angle = 0.f, float animationSpeed = -1.f, float fadeIn = 0.f) :
		type(type), position(position), velocity(velocity), scale(scale),
		alpha(alpha), lifetime(lifetime), fadetime(fadetime), friction(friction), angle(angle), animationSpeed(animationSpeed), fadeIn(fadeIn) {}
};

class Particle /*: public olc::Animation*/
/* {
	struct DelayedParticle {
		Clock timer;
		float length;
		ParticleType particle;
		bool expired;
	};

	// static private members
	static std::vector<std::shared_ptr<Particle>> particles;
	static std::vector<DelayedParticle> delayedParticles;

private:
	const uint32_t _memid; // index pointer to memory location

protected:
	Particle(uint32_t _memid, const std::string& type,
		olc::vf2d position, olc::vf2d velocity, olc::vf2d scale,
		float alpha, float lifetime, float fadetime, float friction, float angle, float animationSpeed, float fadeIn); // do not directly instantiate particle

	olc::vf2d velocity;
	float alpha, starttime, lifetime, fadetime, friction, angle, animationSpeed, fadeIn;
	Clock nextFrame;

	virtual void Update(float delta);

	static std::shared_ptr<Particle> CreateParticle(const std::string& type,
		olc::vf2d position, olc::vf2d velocity, olc::vf2d scale,
		float alpha, float lifetime, float fadetime, float friction, float angle, float animationSpeed, float fadeIn);

public:
	static void GenerateParticle(const std::string& type,
		olc::vf2d position, olc::vf2d velocity, olc::vf2d scale,
		float alpha, float lifetime, float fadetime, float friction, float angle = 0.f, float animationSpeed = -1.f);

	static void GenerateParticle(const ParticleType& ptype);
	static void GenerateParticleDelayed(const ParticleType& ptype, float delayTime);

	static void DrawParticles();
	static void UpdateParticles(float delta);
	static void GarbageCollectParticles();
	static void ClearAllParticles();
	static bool InViewRange(olc::vf2d position); // returns if the point is within range of the particle draw area


public:
	virtual ~Particle();

	 //virtual void Draw();

	void Destroy(); // destroy particle
};
*/