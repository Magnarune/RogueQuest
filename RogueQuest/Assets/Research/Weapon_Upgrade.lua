return 
{
	Name = "Weapon", -- used for Engine::GetResearch()
	offset = {0,0},
	Icon =
	{
		FileName = "Assets/Research/Icons/WeaponUpgradeIcon.png",--Maybe Have multi_Icon based on lvl of upgrade?
		size = {32,32},
		FileSize = {64,64}
	},	
	Parameters =
	{
		Oncer = false, --Only get one upgrade
		InitialCost = 120,
		CostScale = 2, --How Much more it costs per upgrade??? Use Math Equation
		ResearchTime = 22 --Seconds
	},
	Requirements =
	{
		Buildings = {"Castle","StoneTower"}
	},
	Reward = 
	{
		Stats =	{Damage = 5, Armor = 5,Health = 10, MoveSpeed= 4},
		Unlocks = {"Archer"}
	}
}
