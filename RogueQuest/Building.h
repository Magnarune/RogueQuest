#pragma once
#include "olcPixelGameEngine.h"
#include "olcPGEX_TransformedView.h"
#include "WorldObject.h"
#include "Assets.h"


class Building: public Collidable {
	Building();
public:
	bool OnCollision(std::shared_ptr<Collidable> other, olc::vf2d vOverlap) override;
	void BuildingBehaviour();
	void Draw(olc::TileTransformedView* gfx) override;
	void Update(float delta) override;

	virtual ~Building();
	std::string name;
	olc::vf2d pos;
	olc::vi2d Size; //Width and height
	//       stage  offset
	std::string curStage = "Level Three"; // current stage - oof good luck with this

	int buildtime;
	float health;
	float maxHealth;
	int Stage;//What stage is this in? lvl 1 house lvl 2 house etc
	int hurtstage;//if hurt show burning flames effects	
	float m_fTimer;		//Graphics timer

	enum GFXState {
		Normal
	} Graphic_State = Normal;

private:
	std::map<GFXState, std::unique_ptr<olc::Decal>> decals; // multiple textures for different states
	std::map<GFXState, cAssets::BuildingType::TextureMetaData> textureMetadata;
	friend class WorldManager;
	friend class BuildingManager;
};

