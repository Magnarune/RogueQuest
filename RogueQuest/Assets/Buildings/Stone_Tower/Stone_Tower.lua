return 
{
	Name = "StoneTower", -- used for Engine::GenerateBuilding()
	Icon = {
		FileName = "Assets/Buildings/Stone_Tower/tower.png",
		size = {32,32},
		FileSize = {239,486}
	},
	Files = 
	{
		{
			Name = "Normal",
			TargetSize = { 64, 64 },--Size of Building
			FileName = "Assets/Buildings/Stone_Tower/Stone_Tower.png",
			LevelOffsets = {
				{
					Name = "Construction",
					Offset = {0, 200},
					TileSize = {243, 481}
				},
				{
					Name = "Level one",
					Offset = {243, 200},
					TileSize = {240, 481}
				},
				{
					Name = "Level two",
					Offset = {491, 200},
					TileSize = {251, 481}
				},
				{
					Name = "Level three",
					Offset = {729, 0},
					TileSize = {139, 681}
				}
			},
		},
	},
	Parameters =
	{
		CollisionSize = {64,64},
		BuildTime = 10
	},
	Stats =
	{
		Health = 100,
		MaxHealth = 100,      
		AttackRange = 0, 
		AttackDamage = 20,
		AttackSpeed = 0.5
	},
	Production = 
	{
		Units = {""}
	},
	Projectiles = {"Arrow","Axe","Bolt"	}
}