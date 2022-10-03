#pragma once

#include "olcPixelGameEngine.h"
#include "olcPGEX_TransformedView.h"
#include "WorldObject.h"
#include "Assets.h"
#include "Unit.h"


class Building: public Collidable {
	Building();
public:
	bool OnCollision(std::shared_ptr<Collidable> other, olc::vf2d vOverlap) override;
	void ProduceUnit(std::string unit);
	void SentUnitlocation(olc::vf2d Mouse);
	void SendUnit(std::shared_ptr<Unit> unit);
	void BuildingBehaviour();
	void Draw(olc::TileTransformedView* gfx) override;
	void Update(float delta) override;

	virtual ~Building();
	bool bFriendly;
	std::string name;
	olc::vi2d Size; //Width and height
	//
	olc::vf2d sentUnitPos;
	//       stage  offset
	bool building;
	bool startbuilding;
	float productiontime;
	std::queue<std::string> productionQue;
	std::string unitproduced;
	std::vector<std::string> unitproduction;
	std::string curStage = "Level Four"; // current stage - oof good luck with this
	bool bSelected;
	int buildtime;
	float health;
	float maxHealth;
	int Stage;//What stage is this in? lvl 1 house lvl 2 house etc
	int hurtstage;//if hurt show burning flames effects	
	float m_fTimer;		//Graphics timer

	enum GFXState {//?
		Normal
	} Graphic_State = Normal;

private:
	std::map<GFXState, std::unique_ptr<olc::Decal>> decals; // multiple textures for different states
	std::map<GFXState, cAssets::BuildingType::TextureMetaData> textureMetadata;
	friend class Engine;
	friend class cAssets;
	friend class WorldManager;
	friend class BuildingManager;
};

