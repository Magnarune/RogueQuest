#pragma once
#include "olcPixelGameEngine.h"
#include "olcPGEX_TransformedView.h"
#include "clock.h"
#include <variant>
#include <map>


class WorldObject : std::enable_shared_from_this<WorldObject> {
public:

	WorldObject();
	virtual ~WorldObject();

	olc::vf2d Position {};
	olc::vf2d Velocity {};
	
	float m_fTimer {};		
	float drawDepth;
	float updatePriority;

	virtual void Draw(olc::TileTransformedView* gfx);
	virtual void Update(float delta);
	virtual void AfterUpdate(float delta);
	virtual void Destroy();

	// to do, consider adding collision method here or maybe use a different class for that
	// virtual void Interaction() {};//If two world objects interact??? IDK Hmm i'll think about this - perhaps
};
/// Collidable v Collidable
class Collidable : public WorldObject {
public:
	struct Mask {
		enum MaskType { MASK_RECTANGLE, MASK_CIRCLE, MASK_NONE } type;
		union {			
			olc::vf2d rect;
			float radius;
		};
		olc::vf2d origin;

		Mask();
		Mask(olc::vf2d rect);
		Mask(float radius);
	} mask;

protected:
	bool CheckCollision(float delta);
public:
	enum {
		isUnit,
		isBuilding,
		isProjectile,
		isMapObject
	} cType;
	olc::vf2d predPosition;
	
	Collidable();
	virtual ~Collidable();

	void SetMask(const Mask& mask);
	virtual void Update(float delta) override;
	virtual void AfterUpdate(float delta) override;
	virtual bool OnCollision(std::shared_ptr<Collidable> other, olc::vf2d vOverlap) = 0;
};