return 
{
	Name = "Builder", -- used for Engine::GenerateUnit()
	Description = "Builder: A weak unit capable of making most structures in the town. Costs: 12 Gold Foodcost: 5 ",
	SpriteOrder= { 0 , 1 , 2 , 3 },--1 = North, 2 = South , 3 =East , 4 = West
	Files = 
	{
		{
			Name = "Walking",			
			TileSize = { 64 , 64 },
			SpriteSize = {576 , 256 },
			TargetSize = { 32, 32 },
			Origin = {16,16},
			AnimationLength = math.floor(576 / 64), 
			FileName = "Assets/Units/Builder/Walk.png",
			HeadImage = {
				tl = {16,136},
				size = {32,32}
			}
		},
		{
			Name = "Attacking",
			TileSize = { 64 , 64 },
			SpriteSize = {448, 256 },
			TargetSize = { 32, 32 },
			Origin = {16,16},
			AnimationLength = math.floor(384 / 64),
			FileName = "Assets/Units/Builder/Attack.png"
		},
		{
			Name = "Dead",
			TileSize = { 64 , 64 },
			SpriteSize = { 384 , 64 },
			TargetSize = { 32, 32 },
			Origin = {16,16},
			AnimationLength = math.floor(384 / 64),
			FileName = "Assets/Units/Builder/Dead.png"
		},
		{
		Name = "Build",
			TileSize = { 64 , 64 },
			SpriteSize = {448, 256 },
			TargetSize = { 32, 32 },
			Origin = {16,16},
			AnimationLength = math.floor(384 / 64),
			FileName = "Assets/Units/Builder/Build.png"
		}
	},
	Parameters = {
		Food = 5,
		Cost = 12,
		ProductionTime = 10,
		Requirements = {},
		Ranged = false,
		CollisionRadius = 5,
		Abilities = {
			Tasks = {"Move", "Build", "Attack","Repair","Harvest", "Gather"}, -- task names allowed to be delegated to this unit
			Buildables = {"House", "StoneTower", "Castle"},
		}
	},
	Stats =
	{
		Health = 5,
		MaxHealth = 6,
		Mana = 20,
		MaxMana = 20,
		Ammo = 0, 
		MaxAmmo = 0,
		MoveSpeed = 40,       
		AttackRange = 21, 
		AttackDamage = 2,
		AttackSpeed = 5,
		SpellCooldown = 5,
		KnockBackResist = 1
	},	
	Sounds = 
	{
		Death = {""},
		Hurt = {""},
		Selected = {"Assets/Units/Builder/Sounds/What.wav","Assets/Units/Builder/Sounds/What1.wav","Assets/Units/Builder/Sounds/What2.wav"},
		Attack = {""},
		Swing = {""},
		Move = {""},
		Meme = {""}

	},
}