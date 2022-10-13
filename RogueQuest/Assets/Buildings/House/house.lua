return 
{
	Name = "House", -- used for Engine::GenerateBuilding()
	Icon = {
		FileName = "Assets/Buildings/House/House_Icon.png",
		size = {32,32},
		FileSize = {169,140}
	},
	Files = 
	{
		{
			Name = "Normal",
			TargetSize = { 64, 64 },--Size of Building
			FileName = "Assets/Buildings/House/House.png",
			LevelOffsets = {
				{
					Name = "Construction",
					Offset = {22, 80},
					TileSize = {123, 110}
				},
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
					Name = "Level three",
					Offset = {515, 47},
					TileSize = {139, 138}
				},
				{
					Name = "Level four",
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
		Health = 550,
		MaxHealth = 550,      
		AttackRange = 0, 
		AttackDamage = 0,
		AttackSpeed = 1
	},
	Production = 
	{
		Units = {"Mage", "Sara", "Builder"},
		Research = {"Armor", "Weapon", "Boots"}
	},
}