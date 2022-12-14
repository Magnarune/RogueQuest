#pragma once

#include "olcPixelGameEngine.h"
#include "olcPGEX_TransformedView.h"
#include "WorldObject.h"
#include "Assets.h"
#include "Unit.h"
#include "UnitManager.h"
#include "CollisionMapObject.h"
#include "TaskManager.h"
#include <queue>


class Building: public Collidable {
	Building();
public:
	bool OnCollision(std::shared_ptr<Collidable> other, olc::vf2d vOverlap) override;

	void UpgradeBuilding();
	void UnProduceObject();
	void InitializeProduction(const std::string& object);
	void SendUnit(std::shared_ptr<Unit> unit);
	void BuildingBehaviour();
	void CheckResearch(std::string name);
	void Draw(olc::TileTransformedView* gfx) override;
	void Update(float delta) override;
	void AfterUpdate(float delta) override;
	
	void BuildingEffect();
	void Stop();
	virtual ~Building();
	bool bFriendly;
	std::string name;
	olc::vi2d Size; //Width and height
	//
	std::queue<int> Refunds;
	olc::vf2d sentUnitPos;
	// unit production vars
	olc::vi2d offset;
	bool building;
	bool startbuilding;
	float productiontime;
	int buildtime;
	// building development vars
	float buildProgress = 0.f;
	float AttackSpeed;
	float AttackCD;
	bool CanAttack;
	std::weak_ptr<Unit> UnitTarget;
	std::weak_ptr<Building> BuildTarget;

	std::queue<std::string> productionQue;
	std::string researchproduced;
	std::string unitproduced;
	
	std::vector<std::string> researchproduction;//Types of Research this building can proform

	std::vector<std::string> unitproduction;//Type of units that can be made
	std::vector<std::string> AttackTypes;
	bool MakingUnit=true;
	//       stage  offset

	//std::vector<std::string> Stage;
	std::map<int, std::string> Level{
		{0,"Construction"},
		{1, "Level one"},
		{2, "Level two"},
		{3, "Level three"},
		{4, "Level four"}
	};
	enum {
		isNone,
		isUnit,
		isResearch
	}production= isNone;


	int curStage = 0; // current stage - oof good luck with this
	bool isMine;
	bool Interactable;
	Clock execTimeout;
	int Owner;
	std::vector<int> FriendList;
	int Gold;
	bool fire;
	bool effect;

	bool bSelected;
	float maxHealth;
	float Health; // Buildings have health
	int Stage; //What stage is this in? lvl 1 house lvl 2 house etc
	int hurtstage; //if hurt show burning flames effects	
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
	friend class UserInputManager;
	friend class HudManager;
	
};

