return 
{
	Name = "MageAssets"
	tileSize= { 64 , 64 },
	Multifile = 1, -- 1 = Yes, 0 = No
	files = 
	{
		{
		name = "Magewalking",
		SpriteOrder= { 1 , 4 , 2 , 3 },--1 = North, 2 = South , 3 =East , 4 = West  
		TileSize = { 64 , 64},
		SpriteSize = {576 , 256 },
		Animation = SpriteSize[1] / TileSize[1], 
		filename = "C:/Users/BloodworthJr/source/repos/RogueQuest/RogueQuest/MageWalk.png"
		},
		{
		name = "MageAttacking",
		SpriteOrder= { 1 , 4 , 2 , 3 },
		TileSize = { 64 , 64},
		SpriteSize = {448, 256},
		Animation = SpriteSize[1] / TileSize[1] ,
		filename = "C:/Users/BloodworthJr/source/repos/RogueQuest/RogueQuest/MageAttack.png"
		}
		{
		name = "MageDead",
		TileSize = { 64 , 64 },
		SpriteSize = { 384 , 64 },
		Animation = SpriteSize[1] / TileSize[1] ,
		filename = "C:/Users/BloodworthJr/source/repos/RogueQuest/RogueQuest/MageDead.png"
		}	
	}	
	Stats =
	{
		Name = "Simple Mage",
		Health = 10,
		MaxHealth = 10,
		mana = 20,
		MaxMana = 20,
		ammo = 0, 
		Maxammo = 0,
		MoveSpeed = 5,       --Need to find a good value for this
		AttackRange = 10,
		AttackDamage = 2,
		AttackSpeed = 1,
		SpellCooldown = 1,
		CollisionRadius = 10,
		KnockBackResist = 1
	}	
}