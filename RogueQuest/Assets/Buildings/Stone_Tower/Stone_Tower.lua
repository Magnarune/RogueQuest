return 
{
	Name = "StoneTower", -- used for Engine::GenerateBuilding()
	offset = {0,32},
	Icon = {
		FileName = "Assets/Buildings/Stone_Tower/tower.png",
		size = {32,32},
		FileSize = {239,486}
	},
	Files = 
	{
		{
			Name = "Normal",
			TargetSize = { 32, 64 },--Size of Building
			Origin =  {0,32},
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
					TileSize = {231, 481}
				},
				{
					Name = "Level three",
					Offset = {724, 0},
					TileSize = {200, 681}
				}
			},
		},
	},
	Parameters =
	{
		Cost = 350,
		Requirements = {"Castle"},
		Interactable = false,
		CollisionSize = {32,32},
		BuildTime = 10
	},
	Stats =
	{
		isMine = false,
		Health = 100,
		MaxHealth = 100,      
		AttackRange = 0, 
		AttackDamage = 20,
		AttackSpeed = 0.5
	},
	Production = 
	{
		Units = {}
	},
	Projectiles = {"","Arrow","ThrowingAxe","Bolt"	}
}