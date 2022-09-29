#pragma once

#include "olcPixelGameEngine.h"
#include "olcPGEX_TransformedView.h"
#include "clock.h"
#include <variant>
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

/// Collidable v Collidable

class Collidable : public WorldObject {
public:
	typedef std::variant<olc::vf2d, float> Mask;
protected:
	Mask mask;

	bool CheckCollision(float delta);
public:

	Collidable();
	virtual ~Collidable();


	virtual void Update(float fElapsedTime) override;
	virtual bool OnCollision(std::shared_ptr<Collidable> other) = 0;

	/*

	olc::vf2d vPotentialPosition = object.vPOS + object.vVel * fElapsedTime;
		olc::vi2d vCurrentCell = object.vPOS.floor();
		olc::vi2d vTargetCell = vPotentialPosition;
		olc::vi2d vAreaTL = (vCurrentCell.min(vTargetCell) - olc::vi2d(1, 1)).max({ 0,0 });
		olc::vi2d vAreaBR = (vCurrentCell.max(vTargetCell) + olc::vi2d(1, 1)).min(vWorldSize);
		olc::vf2d vRayToNearest;
		olc::vi2d vCell;
		for (vCell.y = vAreaTL.y; vCell.y <= vAreaBR.y; vCell.y++)		{
			for (vCell.x = vAreaTL.x; vCell.x <= vAreaBR.x; vCell.x++)			{
				// Check if the cell is actually solid...
				if (sWorldMap[vCell.y * vWorldSize.x + vCell.x] == '#')				{
					// ...it is! So work out nearest point to future player position, around perimeter
					// of cell rectangle. We can test the distance to this point to see if we have
					// collided.

					olc::vf2d vNearestPoint;
					vNearestPoint.x = std::max(float(vCell.x), std::min(vPotentialPosition.x, float(vCell.x + 1)));
					vNearestPoint.y = std::max(float(vCell.y), std::min(vPotentialPosition.y, float(vCell.y + 1)));

					olc::vf2d vRayToNearest = vNearestPoint - vPotentialPosition;
					float fOverlap = object.fRadius - vRayToNearest.mag();
					if (std::isnan(fOverlap)) fOverlap = 0;
					// If overlap is positive, then a collision has occurred, so we displace backwards by the
					// overlap amount. The potential position is then tested against other tiles in the area
					// therefore "statically" resolving the collision
					if (fOverlap > 0){
						// Statically resolve the collision
						vPotentialPosition = vPotentialPosition - vRayToNearest.norm() * fOverlap;
					}
				}
			}
		}
		object.vPOS = vPotentialPosition;
		*/
};