return 
{
	Name = "House", -- used for Engine::GenerateUnit()
	SpriteOrder= { 0 , 1 , 2 , 3 },--1 = North, 2 = South , 3 =East , 4 = West
	Files = 
	{
		{
			Name = "Construction",			
			TileSize = { 64 , 64 },
			TargetSize = { 32, 32 },
			SpriteSize = {576 , 256 },
			AnimationLength = math.floor(576 / 64), 
			FileName = "Assets/Building/House.png" ,
			HeadImage = {
				tl = {16,136},
				size = {32,32}
			}
		},
		{
			Name = "Level one",
			TileSize = { 64 , 64 },
			TargetSize = { 32, 32 },
			SpriteSize = {448, 256 },
			AnimationLength = math.floor(448 / 64),
			FileName = "Assets/Units/Sara/Attack.png"
		},
		{
			Name = "Level two",
			TileSize = { 64 , 64 },
			TargetSize = { 32, 32 },
			SpriteSize = { 384 , 64 },
			AnimationLength = math.floor(384 / 64),
			FileName = "Assets/Units/Sara/Dead.png"
		}
	},
	Parameters =
	{
		CollisionRadius = 10,
	},
	Stats =
	{
		Health = 10,
		MaxHealth = 10,
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