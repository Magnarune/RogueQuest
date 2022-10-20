return 
{
	Name = "Castle", -- used for Engine::GenerateBuilding()
	offset = {0,0},
	Icon = {
		FileName = "Assets/Buildings/Castle/castleicon.png",
		size = {32,32},
		FileSize = {368,345}
	},
	Files = 
	{
		{
			Name = "Normal",
			TargetSize = { 128, 128 },--Size of Building
			Origin = {64,64},
			FileName = "Assets/Buildings/Castle/Castle.png",
			LevelOffsets = {
				{
					Name = "Construction",
					Offset = {0, 0},
					TileSize = {2100, 2048}
				},
				{
					Name = "Level one",
					Offset = {2100, 0},
					TileSize = {2100, 2048}
				},
				{
					Name = "Level two",
					Offset = {4200, 1703},
					TileSize = {368, 345}
				},
				{
					Name = "Level three",
					Offset = {4568, 1626},
					TileSize = {500, 422}
				}
			},
		},
	},
	Parameters =
	{
		
		Cost = 500,
		Requirements = {},
		Interactable = false,
		CollisionSize = {128,128},
		BuildTime = 10
	},
	Stats =
	{
		isMine = false,
		Health = 550,
		MaxHealth = 550,      
		AttackRange = 0, 
		AttackDamage = 0,
		AttackSpeed = 1
	},
	Production = 
	{
		Units = {"Mage", "Sara", "Builder", "Archer","Goblin","Imp","Spider"},
		Research = {"Armor", "Weapon", "Boots"}
	},
	Projectiles ={"","Arrow","ThrowingAxe","Bolt"}
}