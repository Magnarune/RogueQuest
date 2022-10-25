return 
{
	Name = "Imp", -- used for Engine::GenerateUnit()
	Description = "Demon: A master level deamon with high health and high damage. Costs: 150 Gold Foodcost: 75 Requires: HellCircle",
	SpriteOrder= { 1 , 3 , 0 , 2 },--0 = North, 1 = West , 2 =South , 3 = East  
	Files = 
	{
		{
			Name = "Walking",			
			TileSize = { 64 , 64},
			TargetSize = { 32, 32 },
			SpriteSize = {576 , 256 },
			Origin = {16,16},
			AnimationLength = math.floor(256/ 64), 
			FileName = "Assets/Units/Imp/Walk.png", --in Assets
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
			FileName = "Assets/Units/Imp/Attack.png"
		},
		{
			Name = "Dead",
			TileSize = { 64 , 64 },
			SpriteSize = { 384 , 64 },
			TargetSize = { 32, 32 },
			Origin = {16,16},
			AnimationLength =  math.floor(448 / 64),
			FileName = "Assets/Units/Imp/Dead.png"
		}
	},
	Parameters =
	{
		Food = 75,
		Cost = 150,
		Requirements = {"HellCircle",},
		ProductionTime = 15,
		Ranged = false,
		CollisionRadius = 10, 
		Abilities = {
			Tasks = {"Move", "Attack"}, -- task names allowed to be delegated to this unit
		},
	},
	Stats =
	{
		Health = 500,
		MaxHealth = 500,
		Mana = 20,
		MaxMana = 20,
		Ammo = 0, 
		MaxAmmo = 0,
		MoveSpeed = 40,       
		AttackRange = 21,
		AttackDamage = 2,
		AttackSpeed = 1,
		SpellCooldown = 5,
		KnockBackResist = 1
	},
}