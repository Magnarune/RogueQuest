﻿#pragma once
#include "olcPixelGameEngine.h"
#include "olcPGEX_TransformedView.h"
#include "sol/sol.hpp"
#include "Assets.h"
#include "WorldObject.h"
#include "clock.h"
#include "TaskManager.h"
#include "Building.h"
#include <map>
#include <vector>
#include <queue>
#include <iostream>
#include "Projectiles.h"
#include "Utilities.h"
#include "CollisionMapObject.h"



class Unit : public Collidable   {
protected:
	void DrawCircleDecal(olc::vf2d pos, float radius, olc::Pixel col,olc::TileTransformedView * gfx);
	bool OnCollision(std::shared_ptr<Collidable> other, olc::vf2d vOverlap) override;
	void Update(float fElapsedTime) override; //remake
	void AfterUpdate(float delta) override;
	void Draw(olc::TileTransformedView* gfx) override;//good

	virtual	void RepairBuilding();

	virtual void TrytoBuild(const std::string& name, const olc::vf2d& Target);
	virtual void UnitBehaviour();
	virtual void UnitSearch();
	virtual void UnitHunting();
	virtual void PerformAttack();//good
	virtual void UnitGraphicUpdate(float delta);
	virtual void UpdatePosition(float felapstedtime);
	virtual void Stop();//fine
	
	virtual	void Gather();
	virtual void Deliver();

public:
	Unit();
	virtual ~Unit();
//	const cAssets::UnitType& unitType; // internal unit type reference
	//Testing Some Build Logic Gates	
	olc::vf2d buildlocation;//Where is the building going to be made
	olc::vf2d buildingSize; //Size of Building | Given by construct
	std::string buildName; //Name of what Im building
	std::weak_ptr<Building> repairedbuilding; //pointer to building being repaired
	//Searched units
	bool attackTarget;
	std::weak_ptr<Building> targetBuilding;//parsing pointer
	std::weak_ptr<Unit> targetUnit;//parsing pointer
	std::queue<std::shared_ptr<TaskManager::Task>> taskQueue; // queue of tasks
	std::shared_ptr<TaskManager::Task> currentTask; // current task
	std::weak_ptr<TaskManager::Task> trashtask;
	std::shared_ptr<TaskManager::Task> HoldTask;
	bool Taskpaused;
	//int sound_id;
	float Health; // units have health
	int Gold;
	int Lumber;
	std::weak_ptr<Building> HomeBase;
	std::weak_ptr<Building> MineTarget;
	std::weak_ptr<CollisionMapObject> TreeObject;
	olc::vf2d treeSearchPos;
	olc::vf2d ActionZone; //Zone where to stop moving || Perform action or Both

	// Data
	Clock execTimeout; //Delay In unit thinking time
	std::string sUnitName; //Name
	bool bFriendly;		//Will i attack the Player?
	bool ActionMode;//Square if true circle if false
	// Who Owns me,   Who are my friends;
	int Owner;
	std::vector<int> FriendList;
	 //These could be a map? maybe that is wasteful
	int food;//food added to Leader
	bool bSelected;		//Did the player select me?
	bool bIsRanged;		// Am i Ranged?
	bool bIsAttackable; //Can you Attack me
	bool bDead;			// Am I dead?
	bool bLoot;	  	   //If dead then drop some loot?
	bool bAttacked;    //Was I attacked
	float prevHealth; // I was Hit!
	// Vars For Task Manager System	
	olc::vf2d vRubberBand;   // X , Y, Position to go back to
	olc::vf2d AttackTarget;  // Target you are chasing
	olc::vf2d HitVelocity;	// velocity for getting kicked
	float AgroRange = 120.f; //How far will you look to kill somthing
	float fUnitAngle;        // Angle of the player 0->2pi Radians

	// Parameters
	float Unit_Collision_Radius;//How big is my collision circle?

	// Stats	
	float fMaxHealth;	 // Whats my max health?
	float fMana;		 // mana
	float fMaxMana;		 // maximum mana
	float fAmmo;		 // ammo
	float fMaxAmmo;		 // max-ammo
	float fMoveSpeed;	 // Move speed
	float fAttackRange;	 // Range of attack if ranged
	float fAttackDamage; // How much damage do I do?
	float fAttackSpeed;  // Speed of Projectile
	float fSpellCooldown;// How fast do I attack per second

	float fKnockBackResist;

	float m_fKnockBackTimer; //KnockBack
	float fKBPower;			 //How much knockBack This unit will incure
	
	
	int	SONorth = 0;
	int	SOWest = 1;
	int	SOSouth = 2;
	int	SOEast = 3;	
	std::vector<int> Direction;//Sprite order direction

	olc::vf2d Distance {}; // distance to target (this is where this belongs)
	std::optional<olc::vf2d> Target {}; // position for unit to move to.
	
	enum UnitLogic {
		Aggressive, //If you say attack a pos Search and Kill anything within agro-range
		Neutral,//if attacked switch to attack mode
		Passive //stay
	} ULogic;

	enum Unit_is_Doing {//What is this Unit Doing RIGHT NOW AT THE MOMENT
		isBuilding,	//if close enough and in build state
		isRepairing,//if close enough to repair
		isMinning, //if close gather material 
		isHunting,// Move toward enemy target | What if I tell unit to attack a specific target?
		isPetroling, //Going back and forth
		isMoving	// Move toward move target
	} UTask;

public:
	void KnockBack(float damage, olc::vf2d velocity);

public:

	enum GFXState {
		Walking,
		Attacking,
		Dead,
		Build
	} Graphic_State, Last_State;

	enum {
		North,
		West,		
		South,
		East		
	} FacingDirection;

	float m_fTimer;		//Graphics timer
	float m_fStateTick;// How long unil i can do action tick again
	float fAttackCD =0.1f; //simple timer before unit can attack
	bool bAnimating = false; // is animating

public:
	int curFrame;//current frame of the animation
	std::map<GFXState, std::unique_ptr<olc::Decal>> decals; // multiple textures for different states
	std::map<GFXState, cAssets::UnitType::TextureMetaData> textureMetadata;
	Clock sClock; // slow clock for expensive update operations
	float PI = 3.14159265358f;
	friend class UserInput;
	friend class Hud;
	friend class Engine;
	friend class cAssets;
	friend class UnitManager;
	friend class WorldManager;
	friend class HudManager;
	friend class TaskManager;
	friend class Building;
	friend class BuildingManager;
	//friend class Hero;
};
