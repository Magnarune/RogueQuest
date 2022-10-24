return 
{
	Name = "Goblin", -- used for Engine::GenerateUnit()
	SpriteOrder= { 2 , 3 , 0 , 1 },--0 = North, 1 = West , 2 =South , 3 = East  
	Files = 
	{
		{
			Name = "Walking",			
			TileSize = { 64 , 64},
			TargetSize = { 32, 32 },
			SpriteSize = {576 , 256 },
			Origin = {16,16},
			AnimationLength = math.floor(448/ 64), 
			FileName = "Assets/Units/Goblin/Walk.png", --in Assets
			HeadImage = {
				tl = {16,0},
				size = {32,32}
			}
		},
		{
			Name = "Attacking",
			TileSize = { 64 , 64},
			TargetSize = { 32, 32 },
			SpriteSize = {448, 256},
			Origin = {16,16},
			AnimationLength =  math.floor(256 / 64),
			FileName = "Assets/Units/Goblin/Attack.png"
		},
		{
			Name = "Dead",
			TileSize = { 64 , 64 },
			TargetSize = { 32, 32 },
			SpriteSize = { 384 , 64 },
			Origin = {16,16},
			AnimationLength =  math.floor(320 / 64),
			FileName = "Assets/Units/Goblin/Dead.png"
		}
	},
	Parameters =
	{
		Food = 5,
		Cost = 12,
		Requirements = {"HellCircle",},
		ProductionTime = 12,
		Ranged = false,
		CollisionRadius = 10, 
		Abilities = {
			Tasks = {"Move", "Attack"}, -- task names allowed to be delegated to this unit
		},
	},
	Stats =
	{
		Health = 15,
		MaxHealth = 20,
		Mana = 20,
		MaxMana = 20,
		Ammo = 0, 
		MaxAmmo = 0,
		MoveSpeed = 40,       
		AttackRange = 40,
		AttackDamage = 2,
		AttackSpeed = 1,
		SpellCooldown = 1,
		KnockBackResist = 1
	},
}