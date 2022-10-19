#pragma once
#include "Leaders.h"
#include "olcPixelGameEngine.h"
class Highlighting{
public:
	Highlighting();
	virtual ~Highlighting();

	olc::Pixel OwnerColor(int Color);
	void unitRing(int Owner);
	olc::Pixel GoldIndicator();

	enum {
		Grey,
		Blue,
		Red,
		Green,
		Yellow,
		Dark_Red

	}LeaderColors;
	std::map<int, olc::Pixel> Color{
		{Grey, olc::Pixel(255,195,195,255)},
		{Blue,olc::Pixel(195,195,255,255)},
		{Red, olc::Pixel(195,255,195,255)},
		{Green,olc::Pixel(255,195,255,255)},
	
		{Dark_Red, olc::DARK_RED}//edit later for better values	
	};

};

