return 
{
	Name = "Tree",--Tells Type for import
	Description = "A Tree that gives lumber",	--1 = North, 2 = South , 3 =East , 4 = West
	firstgid = 230,
	TileMapWidth= 32,
	Tilesize = 32,
	FileName = "Assets/MapObjects/Trees/trees-green.png",
	Types = --Renamed to types 
	{  
		{
			Name = "BasicTree",	 -- used for Engine::GenerateTree()	
			Tileids =  {0,1,32,33}, --Tile ids starting at 0 
			TileShape = {2,2},
			CollisionOrigin = {10, 4},
			CollisionBox= {20,36},	
			
		},
		{
			Name = "Bussy Tree", -- used for Engine::GenerateTree()
			Tileids =  {2,3,34,35}, --Tile ids starting at 0 
			TileShape = {2,2},
			CollisionOrigin = {10, 4},
			CollisionBox= {20,36},
		},
		{
			Name = "Thin Tree", -- used for Engine::GenerateTree()
			Tileids =  {4,5,36,37}, --Tile ids starting at 0 
			TileShape = {2,2},
			CollisionOrigin = {10, 4},
			CollisionBox= {20,36},
		},
		{
			Name = "Stick Tree",
			Tileids =  {6,7,38,39}, --Tile ids starting at 0 
			TileSize = { 64 , 64 },
			TileShape = {2,2},
			CollisionOrigin = {10, 4},
			CollisionBox= {20,36},
		},
		{
			Name = "Fat Tree",
			Tileids =  {8,9,40,41}, --Tile ids starting at 0 			
			TileShape = {2,2},
			CollisionOrigin = {10, 4},
			CollisionBox= {20,36},
		},
	},
	Parameters = {
		Lumber = 100,
	},
	Stats =
	{
		
	},
}