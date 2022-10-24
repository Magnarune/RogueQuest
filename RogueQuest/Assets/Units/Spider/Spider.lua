return 
{
	Name = "Spider", -- used for Engine::GenerateUnit()
	SpriteOrder= { 0 , 1 , 2 , 3 },--1 = North, 2 = South , 3 =East , 4 = West
	Files = 
	{
		{
			Name = "Walking",
			TileSize = { 64 , 64 },
			TargetSize = { 32, 32 },
			SpriteSize = {384 , 256 },
			Origin = {16,16},
			AnimationLength = math.floor(384 / 64), 
			FileName = "Assets/Units/Spider/Walk.png" ,
			HeadImage = {
				tl = {16,136},
				size = {32,32}
			}
		},
		{
			Name = "Attacking",
			TileSize = { 64 , 64 },
			TargetSize = { 32, 32 },
			SpriteSize = {256, 256 },
			Origin = {16,16},
			AnimationLength = math.floor(256 / 64),
			FileName = "Assets/Units/Spider/Attack.png"
		},
		{
			Name = "Dead",
			TileSize = { 64 , 64 },
			TargetSize = { 32, 32 },
			SpriteSize = { 384 , 64 },
			Origin = {16,16},
			AnimationLength = math.floor(384 / 64),
			FileName = "Assets/Units/Spider/Dead.png"
		}
	},
	Parameters =
	{
		Food = 5,
		Cost = 12,
		Requirements = {"HellCircle",},
		ProductionTime = 11,
		Ranged = false,
		CollisionRadius = 10,
		Abilities = {
			Tasks = {"Move", "Attack"}, -- task names allowed to be delegated to this unit
		},
	},
	Stats =
	{
		Health = 20,
		MaxHealth = 20,
		Mana = 20,
		MaxMana = 20,
		Ammo = 0, 
		MaxAmmo = 0,
		MoveSpeed = 40,       
		AttackRange = 42, 
		AttackDamage = 2,
		AttackSpeed = 1.6,
		SpellCooldown = 1.6,
		KnockBackResist = 1
	},
}