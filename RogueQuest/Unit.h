#pragma once

#include "olcPixelGameEngine.h"
#include "olcPGEX_TransformedView.h"
#include "sol/sol.hpp"
#include "Assets.h"
#include "WorldObject.h"
#include "clock.h"
#include <map>


class Unit : public WorldObject {
	Unit();

	void CheckCollision(); // change eventually
	void UnitBehaviour();

	void PerformAttack(); //Kill this fool
	void MarchingtoTarget(olc::vf2d Target);
	
	void Update(float fElapsedTime) override;
	void Draw(olc::TileTransformedView* gfx) override;
public:
	virtual ~Unit();
	// Data
	Clock execTimeout; //Delay In unit thinking time
	std::string sUnitName; //Name
	bool bFriendly;		//Will i attack the Player?
	bool bOwnership;	//Does the player own me
	bool bSelected;		//Did the player select me?
	bool bIsRanged;		// Am i Ranged?
	bool bIsAttackable; //Can you Attack me
	bool bDead;			// Am I dead?
	bool bLoot;			//If dead then drop some loot?
	bool bAttacked;    //Was I attacked

	olc::vf2d vRubberBand;   // X , Y, Position to go back to
	olc::vf2d vTarget;       //Where to move after the player clicked on screen
	olc::vf2d AttackTarget;  // Target you are chasing
	olc::vf2d HitVelocity;	// velocity for getting kicked
	float AgroRange = 100; //How far will you look to kill somthing
	std::weak_ptr<WorldObject> Hunted; // currently attacking a world object
	// bool bHunting;				//I am hunting prey stop walking to your clicked location
	float fUnitAngle;        // Angle of the player 0->2pi Radians

	// Parameters
	float Unit_Collision_Radius;//How big is my collision circle?

	// Stats
	float fHealth;		 // How much health do i have?
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
	
	std::vector<int> Direction;
	
	enum UnitLogic{
		Attack, //If you say attack a pos Search and Kill anything within agro-range
		Neutral,//if attacked switch to attack mode
		Passive //stay
	} ULogic;
public:
	void Destroy() override;
	void KnockBack(float damage, olc::vf2d velocity); //I have benn hit!

protected:

	enum GFXState{
		Walking,
		Attacking,
		Dead
	} Graphic_State, Last_State;
	enum{
		North,
		West,		
		South,
		East		
	} FacingDirection;

	float m_fTimer;		//Graphics timer
	float m_fStateTick;// How long unil i can do action tick again
	float fAttackCD; //simple timer before unit can attack

private:
	int curFrame;//current frame of the animation
	std::map<GFXState, std::unique_ptr<olc::Decal>> decals; // multiple textures for different states
	std::map<GFXState, cAssets::UnitType::TextureMetaData> textureMetadata;
	Clock sClock; // slow clock for expensive update operations
	float PI = 3.14159265358f;

	friend class Engine;
	friend class cAssets;
	friend class UnitManager;
	friend class WorldManager;
};

class Builder : public Unit {


};