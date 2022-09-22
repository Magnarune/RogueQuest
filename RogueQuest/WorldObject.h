#pragma once

#include "olcPixelGameEngine.h"
#include "olcPGEX_TransformedView.h"

#include "clock.h"
#include <map>

class WorldObject {
public:

	WorldObject();
	virtual ~WorldObject();
	
	olc::vf2d Position;
	olc::vf2d Velocity;
	
	float m_fTimer;		//Graphics timer

	virtual void Draw(olc::TileTransformedView* gfx);
	virtual void Update(float fElapsedTime);
	virtual void Destroy();

	// to do, consider adding collision method here or maybe use a different class for that
	// virtual void Interaction() {};//If two world objects interact??? IDK Hmm i'll think about this - perhaps
};