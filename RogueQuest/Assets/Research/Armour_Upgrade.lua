return 
{
	Name = "Armour", -- used for Engine::GetResearch()
	ResearchAmount = 5,
	offset = {0,0},
	Icon =
	{
		FileName = "Assets/Research/Icons/Defense_Icon.png",--Maybe Have multi_Icon based on lvl of upgrade?
		size = {32,32},
		FileSize = {64,64}
	},	
	Parameters =
	{
		Oncer = false, --Only get one upgrade
		InitialCost = {100,200,400, 800, 1500},
		ResearchTime = 22 --Seconds
	},
	Requirements =
	{
		Buildings = {"Castle","StoneTower"}
	},
	Reward = 
	{
		Armour = {5, 10,20,50,100},
	}
}
