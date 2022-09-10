return 
{
	Name = "Mage", -- used for Engine::GenerateUnit()
	Files = 
	{
		{
			Name = "Walking",
			SpriteOrder= { 1 , 4 , 2 , 3 },--1 = North, 2 = South , 3 =East , 4 = West
			TileSize = { 64 , 64 },
			TargetSize = { 32, 32 },
			SpriteSize = {576 , 256 },
			AnimationLength = math.floor(576 / 64), 
			FileName = "Assets/Units/Mage/MageWalk.png" --in Assets
		},
		{
			Name = "Attacking",
			SpriteOrder= { 1 , 4 , 2 , 3 },
			TileSize = { 64 , 64 },
			TargetSize = { 32, 32 },
			SpriteSize = {448, 256 },
			AnimationLength = math.floor(448 / 64),
			FileName = "Assets/Units/Mage/MageAttack.png"
		},
		{
			Name = "Dead",
			TileSize = { 64 , 64 },
			TargetSize = { 32, 32 },
			SpriteSize = { 384 , 64 },
			AnimationLength = math.floor(384 / 64),
			FileName = "Assets/Units/Mage/MageDead.png"
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
		MoveSpeed = 5000,       --Need to find a good value for this
		AttackRange = 10,
		AttackDamage = 2,
		AttackSpeed = 1,
		SpellCooldown = 1,
		KnockBackResist = 1
	},
}