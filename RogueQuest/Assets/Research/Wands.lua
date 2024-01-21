return 
{
	Name = "Wands", -- used for Engine::GetResearch()
	Description = "Wands: A wizards most trusted friend. Unlocks: Mage, Sara  Costs: 1500 Gold  Requires: Castle, Stone Tower",
	ResearchAmount = 1,
	offset = {0,0},
	Icon =
	{
		FileName = "Assets/Research/Icons/Wizards_Unlock.png",--Maybe Have multi_Icon based on lvl of upgrade?
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
		Unlocks = {"Mage", "Sara", "Wands","HellCircle"}
	}
}
