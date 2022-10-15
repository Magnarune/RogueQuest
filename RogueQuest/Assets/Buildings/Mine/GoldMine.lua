return 
{
	Name = "GoldMine", -- used for Engine::GenerateBuilding()
	Icon = {
		FileName = "Assets/Buildings/Mine/GoldMineIcon.png",
		size = {32,32},
		FileSize = {95,95}
	},
	Files = 
	{
		{
			Name = "Normal",
			TargetSize = { 64, 64 },--Size of Building
			FileName = "Assets/Buildings/Mine/goldmine.png",
			LevelOffsets = {
				{
					Name = "Construction",
					Offset = {0, 0},
					TileSize = {95, 96}
				},
				{
					Name = "Level one",
					Offset = {95, 0},
					TileSize = {96, 96}
				},
				{
					Name = "Level two",
					Offset = {191, 0},
					TileSize = {97, 96}
				}		
			},
		},
	},
	Parameters =
	{
		Interactable = true,
		CollisionSize = {60,60},
		BuildTime = 10
	},
	Stats =
	{
		isMine = true,
		Health = 550,
		MaxHealth = 550,      
		AttackRange = 0, 
		AttackDamage = 0,
		AttackSpeed = 1
	},
	Production = 
	{
		Gold = 25000,
		Units = {""},
		Research = {""}
	},
	Projectiles ={""}
}