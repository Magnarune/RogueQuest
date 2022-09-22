return 
{
	Name = "Builder", -- used for Engine::GenerateUnit()
	SpriteOrder= { 0 , 1 , 2 , 3 },--1 = North, 2 = South , 3 =East , 4 = West
	Files = 
	{
		{
			Name = "Walking",			
			TileSize = { 64 , 64 },
			TargetSize = { 32, 32 },
			SpriteSize = {576 , 256 },
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
			TargetSize = { 32, 32 },
			SpriteSize = {448, 256 },
			AnimationLength = math.floor(384 / 64),
			FileName = "Assets/Units/Builder/Attack.png"
		},
		{
			Name = "Dead",
			TileSize = { 64 , 64 },
			TargetSize = { 32, 32 },
			SpriteSize = { 384 , 64 },
			AnimationLength = math.floor(384 / 64),
			FileName = "Assets/Units/Builder/Dead.png"
		}
	},
	Parameters = {
		CollisionRadius = 10,
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
		AttackRange = 82, 
		AttackDamage = 2,
		AttackSpeed = 1,
		SpellCooldown = 1,
		KnockBackResist = 1
	},
}