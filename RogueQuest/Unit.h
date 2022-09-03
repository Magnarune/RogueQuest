#include "olcPixelGameEngine.h"
#include "sol/sol.hpp"

class Unit
{
	Unit(std::string name, sol::table Unitdata);
	virtual ~Unit();
public:
	std::string sUnitName;
	sol::table  tUnitdata;
	bool bFriendly;
	bool bOwnership;
	bool bSelected;
	bool bIsRanged;
	bool bIsAttackable;
	bool bDead;
	bool bLoot;
	//Unit vPOS + vVel +Θ<-float
	float Unit_Collision_Radius;
	olc::vf2d vUnitPosition;
	olc::vf2d vUnitVelocity;
	olc::vf2d vRubberBand;
	olc::vf2d vTarget;
	float fUnitAngle;
	//
	float fAttackSpeed;
	float fAttackDamage;
	float fHealth;
	float fMaxHealth;
	float fMana;
	float fMaxMana;
	float fAmmo;
	float fMaxAmmo;
	float fMoveSpeed;
	float fAttackRange;
	float m_fKnockBackTimer;
	float m_fKnockBackDX;
	float m_fKnockBackDY;
	float fKBPower;
	
	olc::vi2d SpriteSheetTileSize;//ForExample 64x64 or 32x64 ect..
	olc::vi2d SpriteSheetSize;//Total Sprite Size say 640 x 256
	int AnimationLength; //length say = 9 or = SpriteSheetSize.x / SpriteSheetTileSize.x  
	enum
	{
		SONorth,
		SOSouth,
		SOEast,
		SOWest
	}SpriteOrder;//Order of sprite facing directions in sprite sheet


	olc::Sprite* UnitSpriteSheet;
	olc::Sprite* Weapon;
	enum UnitLogic
	{
		Attack, //If you say attack a pos Search and Kill anything within agro-range
		Neutral,//if attacked switch to attack mode
		Passive //stay
	};
public:
	void DrawUnit(olc::PixelGameEngine* gfx, olc::vf2d Offset, float fElapsedTime); //Decal not pge :/
	void MovementUpdate(float fElapsedTime, UnitLogic Logic);	  //Behaviour inside this Logic;
	void PerformAttack();
	void KnockBack(float power,float dx, float dy, float dist); 
protected:
	enum 
	{
		Idle,
		Walking,
		Dead
	} Graphic_State;
	enum
	{
		North,
		South,
		East,
		West
	}FacingDirection;
	float m_fTimer;
	int m_nGraphicCounter;
	float m_fStateTick;

};
/*
MagaData:
"MageWalk.png"
Sprite offset {64x64}
Maybe an enum [N,W,S,E];
Sprite Length =  9 ie
Sprite Down = 4.
Idle Down = 0, 




*/





/*
Class Unit = {}

//For colision detection
enum...
	Circle
	BigCircle
	...
	bFriendly = //Is that computer a baddy
	bOwnership = //Do you own it
	bSelected = //Did you select the unit
	bRanged = //Is this unit a ranged unit I.E. ATTACK LOGIC
	bDead = //Is Dead
	bLoot = // Drop Loot if able
	std::string Name//Name of Unit

	enum ...
	Attack
	Neutal
	Stop
	...//how far will it search to attack target

	olc::vf2d RubberBand = //Where will unit go back to if you get out of range
	olc::vf2d Position = //Position
	olc::vf2d Velocity = // Movement
	olc::vf2d Target = //Moves to targeted location

	float Radius = //Unit Collision Radius
	float fAngle = //Unit Angle
	float AttackSpeed = //attack per ...
	float AttackDamage = //Damage per attack
	float Health = //Health
	float MaxHealth = //Max Health
	float Mana = //Mana
	float MaxMana = //Max Mana
	float Speed = //Movement Speed
	float Range = // if ranged how far can it attack
	olc::Sprite * Sprite = //Spritesheet for Unit <These should be Decals
	olc::Sprite * Weapon = //WeaponSheet for Unit <These should be Decals
	void Attack Animation() < -Takes name of unit and give currect attack sprite
	void MovementUpdate() < -Takes data above and gives currect move animation
*/