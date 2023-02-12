return 
{
	Name = "Archer", -- used for Engine::GenerateUnit()
	Description = "Archer: A long ranged Unit that specializes in High damage but at the cost of low health. Costs: 22 Gold  Requires: BowCraft",
	SpriteOrder= { 0 , 1 , 2 , 3 },--1 = North, 2 = South , 3 =East , 4 = West
	Files = 
	{
		{
			Name = "Walking",			
			TileSize = { 64 , 64 },
			TargetSize = { 32, 32 },
			SpriteSize = {576 , 256 },
			Origin = {16,16},
			AnimationLength = math.floor(576 / 64), 
			FileName = "Assets/Units/Archer/Walk.png",
			HeadImage = {
				tl = {16,136},
				size = {32,32}
			}
		},
		{
			Name = "Attacking",
			TileSize = { 64 , 64 },
			TargetSize = { 32, 32 },
			SpriteSize = {448, 256 },
			Origin = {16,16},
			AnimationLength = math.floor(384 / 64),
			FileName = "Assets/Units/Archer/Attack.png"
		},
		{
			Name = "Dead",
			TileSize = { 64 , 64 },
			TargetSize = { 32, 32 },
			SpriteSize = { 384 , 64 },
			Origin = {16,16},
			AnimationLength = math.floor(384 / 64),
			FileName = "Assets/Units/Archer/Dead.png"
		},
	},
	Parameters = {
		Food = 5,
		Cost = 22,
		ProductionTime = 6,
		Requirements = {"BowCraft"},
		Ranged = true,
		Projectile = "Arrow",
		CollisionRadius = 8,
		Abilities = {
			Tasks = {"Move", "Attack"} -- task names allowed to be delegated to this unit			
		}
	},
	Stats =
	{
		Health = 80,
		MaxHealth = 80,
		Mana = 20,
		MaxMana = 20,
		Ammo = 0, 
		MaxAmmo = 0,
		MoveSpeed = 80,       
		AttackRange = 80, 
		AttackDamage = 2,
		AttackSpeed = .7,
		SpellCooldown = 5,
		KnockBackResist = 1
	},
	Sounds = 
	{
		Death = {"./Sounds/Dead.wav", "./Sounds/Dead1.wav","./Sounds/Dead2.wav"},
		Hurt = {""},
		Selected = {""},
		Attack = {""},
		Swing = {""},
		Move = {""},
		Meme = {""}

	},
}