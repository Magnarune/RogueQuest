#pragma once

#include "olcPixelGameEngine.h"
#include "sol/sol.hpp"
#include "Assets.h"
#include <map>



class Unit
{
	Unit(); // keep private

public:
	virtual ~Unit();

	// Data
	std::string sUnitName; //Name
	bool bFriendly; //Will i attack the Player?
	bool bOwnership; //Does the player own me
	bool bSelected; //Did the player select me?
	bool bIsRanged; // Am i Ranged?
	bool bIsAttackable; //Can you Attack me
	bool bDead; // Am I dead?
	bool bLoot; //If dead then drop some loot?
	//Unit vPOS + vVel +Θ<-float
	olc::vf2d vUnitPosition; // X, Y Position
	olc::vf2d vUnitVelocity; // Vx, Vy Velocity
	olc::vf2d vRubberBand;   // X, Y, Position to go back to
	olc::vf2d vTarget;       //Where to move after the player clicked on screen
	float fUnitAngle;        // Angle of the player 0->2pi Radians

	// Parameters
	float Unit_Collision_Radius;//How big is my collision circle?

	// Stats
	float fHealth; // How much health do i have?
	float fMaxHealth;//Whats my max health?
	float fMana;//mana
	float fMaxMana;//maximum mana
	float fAmmo;//ammo
	float fMaxAmmo;//max-ammo
	float fMoveSpeed;//Move speed
	float fAttackRange;//Range of attack if ranged
	float fAttackDamage;//How much damage do I do?
	float fAttackSpeed;      //How fast do I attack
	float fSpellCooldown;//How fast do I attack
	float fKnockBackResist;
	float m_fKnockBackTimer;//KnockBack

	float fKBPower;//How much knockBack This unit will incure
	
	olc::vi2d SpriteSheetTileSize;//For Example 64x64 or 32x64 ect..
	olc::vi2d SpriteSheetSize;//Total Sprite Size say 640 x 256


	enum
	{
		SONorth,
		SOSouth,
		SOEast,
		SOWest
	}SpriteOrder;//Order of sprite facing directions in sprite sheet
	enum UnitLogic
	{
		Attack, //If you say attack a pos Search and Kill anything within agro-range
		Neutral,//if attacked switch to attack mode
		Passive //stay
	};

public:
	void DrawUnit(olc::PixelGameEngine* gfx, olc::vf2d Offset, float fElapsedTime); //renderable not pge :/
	void MovementUpdate(float fElapsedTime, UnitLogic Logic); //Behaviour inside this Logic;
	void PerformAttack();//Kill this fool
	void KnockBack(float power,float dx, float dy, float dist); //I have benn hit!

protected:
	enum GFXState
	{
		Walking,
		Attacking,
		Dead
	} Graphic_State;

	enum
	{
		North,
		South,
		East,
		West
	} FacingDirection;

	float m_fTimer;//Graphics timer
	float m_fStateTick;// How long unil i can do action tick again

private:
	int curFrame;//current frame of the animation
	std::map<GFXState, std::unique_ptr<olc::Decal>> decals; // multiple textures for different states
	std::map<GFXState, size_t> AnimationLengths; //length say = 9 or = SpriteSheetSize.x / SpriteSheetTileSize.x

	friend class Engine;
	friend class cAssets;
	friend class UnitManager;
	
};