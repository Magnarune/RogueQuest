/*
#include "tempParticles.h"
#include "Engine.h"
// #include "particle.h"
//#include "gamecontrol.h"



namespace ParticleMemory {
	using ParticleMemoryBuffer = std::array<uint8_t[sizeof(Particle)], 1024 * 10>;

	static ParticleMemoryBuffer* part_memory = nullptr; // pre-allocate memory
	static std::vector<bool> parts_used;
	//static olc::AnimationAsset particleAnimation;
	static size_t particleCount = 0;

	// Public:

	const size_t& activeParticles = particleCount; // const reference

	void InitParticleMemory() {//load resources 
		if (part_memory != nullptr) return;

		//particleAnimation = GameAssets::manager().GetAnimation("particles.ani");
		part_memory = new ParticleMemoryBuffer{};
		parts_used.resize(part_memory->size(), false); // initialize
		particleCount = 0;
	}

	void FreeParticleMemory() {//free resourses
		if (part_memory == nullptr) return;

		Particle::ClearAllParticles(); // make sure to free particles before deallocating memory block!
		delete part_memory;
		part_memory = nullptr;
		//particleAnimation.reset();
		parts_used.clear();
		parts_used.shrink_to_fit();
	}
}

std::vector<std::shared_ptr<Particle>> Particle::particles;
std::vector<Particle::DelayedParticle> Particle::delayedParticles;

Particle::Particle(uint32_t ind, const std::string& type,
	olc::vf2d position, olc::vf2d velocity, olc::vf2d scale,
	float alpha, float lifetime, float fadetime, float friction, float angle, float animationSpeed, float fadeIn) :

	////Animation(*ParticleMemory::particleAnimation), _memid(ind),
	//alpha(alpha), starttime(lifetime), lifetime(lifetime), fadetime(fadetime), velocity(velocity), friction(friction), angle(angle), animationSpeed(animationSpeed), fadeIn(fadeIn) {
	//setFrameRegion(type);

	//if (animationSpeed < 0.f) {
	//	setFrame(static_cast<int>(getRandomFloat() * getFrameCount()));
	//} else if (lifetime < 0.f) {
	//	this->lifetime = float(getFrameCount()) / animationSpeed;
	//}

	//++ParticleMemory::particleCount;
	this->position = position;
	this->scale = scale;				//THIS IS WRONG BAD BAD CREATOR NO
	this->rotation = -angle;
	blend.a = uint8_t(alpha * 255);
	origin = frameLocation.size / 2.f; // center aligned
}

Particle::~Particle() {
	ParticleMemory::parts_used[_memid] = false; // free memory
	--ParticleMemory::particleCount;
}


void Particle::Update(float delta) {
	lifetime -= delta;
	bool moving = std::abs(velocity.x) > 0.f || std::abs(velocity.y) > 0.f;
	if (moving) {
		velocity *= std::pow(friction, delta);
		if (std::abs(velocity.x) < 0.1f && std::abs(velocity.y) < 0.1f) velocity = { 0.f, 0.f };
		position += velocity * delta;
	}
	if (lifetime <= fadetime) {
		blend.a = uint8_t(255 * std::clamp(lifetime / fadetime * alpha, 0.f, alpha)); // change ( / v.f ) where v is seconds to start fading
	} else {
		blend.a = uint8_t(255 * std::clamp(fadeIn <= 0.f ? 1.f : ((starttime - lifetime) / fadeIn) * alpha, 0.f, alpha));
	}
	if (animationSpeed >= 0.f) {
		if (nextFrame.getSeconds() > 1.f / animationSpeed) {
			setFrame(getFrame() + 1);
			nextFrame.restart();
		}
	}

	if (lifetime < 0.f) {
		Destroy();	//calling a destroy inside the Update Loop not in my function....
	}
}

void Particle::Destroy() {
	using namespace ParticleMemory;
	if (part_memory == nullptr) return;

	auto itr = std::find_if(particles.begin(), particles.end(), [&](auto part) { return part.get() == this; });
	if (itr != particles.end()) itr->reset();
}


// static methods

std::shared_ptr<Particle> Particle::CreateParticle(const std::string& type,
	olc::vf2d position, olc::vf2d velocity, olc::vf2d scale,
	float alpha, float lifetime, float fadetime, float friction, float angle, float animationSpeed, float fadeIn) {
	using namespace ParticleMemory;
	if (part_memory == nullptr || particleCount >= part_memory->size()) return nullptr;

	auto& mem = (*part_memory); // particle memory

	size_t ind = 0;
	for (bool x : parts_used) { if (!x) break; ++ind; } // find index of first free mem

	if (ind == parts_used.size()) throw std::runtime_error("GenerateParticle() - failed to find next free mem slot"); // something is wrong - this means free memory is marked as used

	parts_used[ind] = true;

	std::shared_ptr<Particle> part(
		new (reinterpret_cast<Particle*>(mem.data()) + ind) Particle(ind, type, position, velocity, scale, alpha, lifetime, fadetime, friction, angle, animationSpeed, fadeIn)
		//          ^^^Address^^^            ^^^Offset^^^    ^^^Object^^^
		, [](Particle* p) { p->~Particle(); });

	particles.emplace_back(std::move(part));

	return particles.back();
}

// this is going to be deprecated - please use the ParticleType generator overload
void Particle::GenerateParticle(const std::string& type, olc::vf2d position, olc::vf2d velocity, olc::vf2d scale, float alpha, float lifetime, float fadetime, float friction, float angle, float animationSpeed) {

	CreateParticle(type, position, velocity, scale, alpha, lifetime, fadetime, friction, angle, animationSpeed, 0.f);

}

void Particle::GenerateParticle(const ParticleType& ptype) {

	CreateParticle(ptype.type, ptype.position, ptype.velocity, ptype.scale, ptype.alpha,
		ptype.lifetime, ptype.fadetime, ptype.friction, ptype.angle,
		ptype.animationSpeed, ptype.fadeIn);

}

void Particle::GenerateParticleDelayed(const ParticleType& ptype, float delaySeconds) {
	DelayedParticle dp{};
	dp.particle = ptype; // copy
	dp.length = delaySeconds;

	delayedParticles.emplace_back(std::move(dp)); // move into vector
}

void Particle::DrawParticles() {
	auto& engine = Game_Engine::Current();
	
	olc::vf2d tl{}, br{};
	tl = engine.tv.GetWorldTL();
	br = engine.tv.GetWorldBR();
	

	for (auto part : particles) {
		if (!part) continue;

		if (tl != br) { // if view has boundaries, check position of particle
			auto postl = part->position - part->origin,
				 posbr = part->position + part->origin;

			if (posbr.x < tl.x || posbr.y < tl.y || postl.x > br.x || postl.y > br.y) continue; // out of view
		}

		part->Draw(view);
	}
}

void Particle::UpdateParticles(float delta) {

	for (auto& [timer, length, part, expired] : delayedParticles) {
		if (expired || timer.getSeconds() < length) continue;
		expired = true;
		GenerateParticle(part); // generate delayed particle
	}

	for (auto part : particles) {
		if (!part) continue;
		part->Update(delta);
	}
}

void Particle::GarbageCollectParticles() {
	if (!ParticleMemory::particleCount) particles.clear();
	if (ParticleMemory::particleCount < particles.size()) { // optimization - move particles into new vector
		decltype(particles) moved(std::move(particles));
		particles.clear();
		particles.reserve(ParticleMemory::particleCount);
		for (auto& part : moved) if (part) particles.emplace_back(std::move(part));
	} else { // default clean

		for (size_t i = particles.size(); i > size_t(std::max(0LL, int64_t(particles.size())) - 500LL); --i) {
			if (!particles[i - 1]) particles.erase(particles.begin() + i - 1);
		}
	}
	if (delayedParticles.size()) { // clean-up delayed particles
		for (size_t i = delayedParticles.size(); i > size_t(std::max(0LL, int64_t(delayedParticles.size())) - 500LL); --i) {
			if (delayedParticles[i - 1].expired) delayedParticles.erase(delayedParticles.begin() + i - 1);
		}
	}
}

void Particle::ClearAllParticles() {
	particles.clear();
	delayedParticles.clear();
}

bool Particle::InViewRange(olc::vf2d position) {
	if (!GameControl::current().IsRunning()) return false;

	View& view = GameControl::current().view;
	olc::vf2d tl = view.GetWorldTL(),
		br = view.GetWorldBR(),
		offset = { 1024.f, 1024.f },
		postl = position - offset,
		posbr = position + offset;

	return (posbr.x > tl.x && posbr.y > tl.y && postl.x < br.x&& postl.y < br.y);
}
*/