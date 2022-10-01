return 
{
	Name = "House", -- used for Engine::GenerateUnit()
	Icon = {
		FileName = "Assets/Buildings/House/House_Icon.png",
		size = {32,32}
	},
	Files = 
	{
		{
			Name = "Normal",
			TargetSize = { 64, 64 },--Size of Building
			FileName = "Assets/Buildings/House/House.png",
			LevelOffsets = {
				{
					Name = "Level one",
					Offset = {171, 64},
					TileSize = {144, 132}
				},
				{
					Name = "Level two",
					Offset = {349, 54},
					TileSize = {167, 140}
				},
				{
					Name = "Level Three",
					Offset = {515, 47},
					TileSize = {139, 138}
				},
				{
					Name = "Level Four",
					Offset = {671, 0},
					TileSize = {140, 185}
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
		Health = 150,
		MaxHealth = 150,      
		AttackRange = 0, 
		AttackDamage = 0,
		AttackSpeed = 1
	},
}