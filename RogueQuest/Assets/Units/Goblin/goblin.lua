return 
{
	Name = "Goblin", -- used for Engine::GenerateUnit()
	Files = 
	{
		{
			Name = "Walking",
			SpriteOrder= { 1 , 4 , 2 , 3 },--1 = North, 2 = South , 3 =East , 4 = West  
			TileSize = { 64 , 64},
			SpriteSize = {576 , 256 },
			AnimationLength = 448/ 64, 
			FileName = "Assets/Units/Goblin/Walk.png" --in Assets
		},
		{
			Name = "Attacking",
			SpriteOrder= { 1 , 4 , 2 , 3 },
			TileSize = { 64 , 64},
			SpriteSize = {448, 256},
			AnimationLength = 256 / 64,
			FileName = "Assets/Units/Goblin/Attack.png"
		},
		{
			Name = "Dead",
			TileSize = { 64 , 64 },
			SpriteSize = { 384 , 64 },
			AnimationLength = 320 / 64,
			FileName = "Assets/Units/Goblin/Dead.png"
		}
	},
	Parameters =
	{
		CollisionRadius = 10, -- is this really a stat? e.g. something that the player could "upgrade and change" based on power ups in game?
	},
	Stats =
	{
		Health = 10,
		MaxHealth = 10,
		Mana = 20,
		MaxMana = 20,
		Ammo = 0, 
		MaxAmmo = 0,
		MoveSpeed = 80,       
		AttackRange = 40,
		AttackDamage = 2,
		AttackSpeed = 1,
		SpellCooldown = 1,
		KnockBackResist = 1
	},
}