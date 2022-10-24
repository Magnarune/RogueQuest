return 
{
	Name = "HellCircle", -- used for Engine::GetResearch()
	ResearchAmount = 1,
	offset = {0,0},
	Icon =
	{
		FileName = "Assets/Research/Icons/HellCircleIcon.png",--Maybe Have multi_Icon based on lvl of upgrade?
		size = {32,32},
		FileSize = {64,64}
	},	
	Parameters =
	{
		InitialCost = {5000},
		ResearchTime = 22 --Seconds
	},
	Requirements =
	{
		Buildings = {"Castle","StoneTower","Wands","BowCraft"}
	},
	Reward = 
	{	
		Unlocks = {"Imp","Spider","Goblin"}
	}
}
