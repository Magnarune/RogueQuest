#pragma once

#include "olcPixelGameEngine.h"
#include "olcPGEX_TransformedView.h"
#include "WorldObject.h"
#include "Assets.h"
#include "Unit.h"
#include "UnitManager.h"
#include "TaskManager.h"
#include <queue>


class Building: public Collidable {
	Building();
public:
	bool OnCollision(std::shared_ptr<Collidable> other, olc::vf2d vOverlap) override;
	void ProduceUnit(const std::string& unit);
	void SentUnitlocation(olc::vf2d Mouse);
	void SendUnit(std::shared_ptr<Unit> unit);
	void BuildingBehaviour();
	void Draw(olc::TileTransformedView* gfx) override;
	void Update(float delta) override;
	void Stop();

	virtual ~Building();
	bool bFriendly;
	std::string name;
	olc::vi2d Size; //Width and height
	//
	olc::vf2d sentUnitPos;
	// unit production vars
	bool building;
	bool startbuilding;
	float productiontime;
	int buildtime;
	// building development vars
	float buildProgress = 0.f;

	std::queue<std::string> productionQue;
	std::string unitproduced;
	std::vector<std::string> unitproduction;
	//       stage  offset
	std::string curStage = "Construction"; // current stage - oof good luck with this
	
	bool bSelected;
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
	friend class UnitManager;
	friend class TaskManager;
	friend class Unit;
};

