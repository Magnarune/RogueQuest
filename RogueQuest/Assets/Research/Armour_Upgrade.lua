return 
{
	Name = "Armour", -- used for Engine::GetResearch()
	Description = "Armour Upgrade: Increases Armour of all Units by {5, 10,20,50,100} and Costs: {100,200,400, 800, 1500}",
	ResearchAmount = 5, --5 levels
	offset = {0,0},
	Icon =
	{
		FileName = "Assets/Research/Icons/Defense_Icon.png",--Maybe Have multi_Icon based on lvl of upgrade?
		size = {32,32},
		FileSize = {64,64}
	},	
	Parameters =
	{
		InitialCost = {100,200,400, 800, 1500}, --Cost Per LVL
		ResearchTime = 22 --Seconds
	},
	Requirements =
	{
		Buildings = {"Castle","StoneTower"} --What it checks for
	},
	Reward = 
	{
		Armour = {5, 10,20,50,100}, --What generated Units get + base armour
	}
}
