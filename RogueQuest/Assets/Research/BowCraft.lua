return 
{
	Name = "BowCraft", -- used for Engine::GetResearch()
	ResearchAmount = 1,
	offset = {0,0},
	Icon =
	{
		FileName = "Assets/Research/Icons/Archer_Unlock.png",--Maybe Have multi_Icon based on lvl of upgrade?
		size = {32,32},
		FileSize = {64,64}
	},	
	Parameters =
	{		
		InitialCost = {1500},
		ResearchTime = 22 --Seconds
	},
	Requirements =
	{
		Buildings = {"Castle","StoneTower"}
	},
	Reward = 
	{
		Unlocks = {"Archer",}
	}
}
