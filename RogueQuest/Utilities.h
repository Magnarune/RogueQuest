#pragma once
#include "olcPixelGameEngine.h"
#include <memory>

class Circle {
	std::unique_ptr<olc::Sprite> sprite;
	std::unique_ptr<olc::Decal> decal;//no touchy
	const float resolution;
public:
	olc::vf2d position;
	olc::vf2d radius;
	olc::Pixel col = 0x70FFFFFF;//Hex 

	Circle(float res);

	void Draw() const;

};