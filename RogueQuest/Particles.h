#pragma once
#include "olcPixelGameEngine.h"
#include <iostream>
#include <vector>
class Particles {
public:
	void CreateParticles(olc::vf2d Spawn);
	void UpdateParticles(float delta);
	void DrawDestroyParticles();



	Particles();
	~Particles();

	std::vector<float>::const_iterator a;

	std::vector<olc::vf2d> pos;

	std::vector<float> timer;


};