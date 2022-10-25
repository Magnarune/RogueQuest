return 
{
	Name = "Boots", -- used for Engine::GetResearch()
	Description = "Boots Upgrade: Increases MoveSpeed of all Units by {5, 10,20,50,100} and Costs: {100,200,400, 800, 1500}",
	ResearchAmount = 5,
	offset = {0,0},
	Icon =
	{
		FileName = "Assets/Research/Icons/BootsIcons.png",--Maybe Have multi_Icon based on lvl of upgrade?
		size = {32,32},
		FileSize = {64,64}
	},	
	Parameters =
	{
		Oncer = false, --Only get one upgrade
		InitialCost = {100,200,400, 800, 1500},
		ResearchTime = 12 --Seconds
	},
	Requirements =
	{
		Buildings = {"Castle","StoneTower"}
	},
	Reward = 
	{
		MoveSpeed = {5, 10,20,50,100},
	}
}
