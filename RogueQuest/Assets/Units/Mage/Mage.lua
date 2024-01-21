return 
{
	Name = "Mage", -- used for Engine::GenerateUnit()
	Description = "Mage: A spell slinger good in numbers and alone. Costs: 12 Gold Foodcost: 5 Requires: Wands",
	SpriteOrder= { 0 , 1 , 2 , 3 },--1 = North, 2 = South , 3 =East , 4 = West
	Files = 
	{
		{
			Name = "Walking",			
			TileSize = { 64 , 64 },
			TargetSize = { 32, 32 },
			SpriteSize = {576 , 256 },
			Origin = {16,16},
			AnimationLength = math.floor(576 / 64), 
			FileName = "Assets/Units/Mage/MageWalk.png",
			HeadImage = {
				tl = {16,136},
				size = {32,32}
			}
		},
		{
			Name = "Attacking",
			TileSize = { 64 , 64 },
			TargetSize = { 32, 32 },
			SpriteSize = {448, 256 },
			Origin = {16,16},
			AnimationLength = math.floor(448 / 64),
			FileName = "Assets/Units/Mage/MageAttack.png"
		},
		{
			Name = "Dead",
			TileSize = { 64 , 64 },
			TargetSize = { 32, 32 },
			SpriteSize = { 384 , 64 },
			Origin = {16,16},
			AnimationLength = math.floor(384 / 64),
			FileName = "Assets/Units/Mage/MageDead.png"
		}
	},
	Parameters = {
		Food = 5,
		Cost = 12,
		Requirements = {"House","Wands"},
		ProductionTime = 8,
		Ranged = true,--un needed
		Projectile = "ThrowingAxe",
		CollisionRadius = 10,
		Abilities = {
			Tasks = {"Move", "Attack"}, -- task names allowed to be delegated to this unit
		},
	},
	Stats =
	{
		Health = 10,
		MaxHealth = 10,
		Mana = 20,
		MaxMana = 20,
		Ammo = 0, 
		MaxAmmo = 0,
		MoveSpeed = 40,       
		AttackRange = 50, 
		AttackDamage = 2,
		AttackSpeed = 0.2,
		SpellCooldown = 0.2,
		KnockBackResist = 1
	},
	Sounds = 
	{
		Death = {"Assets/Units/Mage/Sounds/Dead.wav"},
		Hurt = {""},
		Selected = {"Assets/Units/Mage/Sounds/What.wav","Assets/Units/Mage/Sounds/What1.wav","Assets/Units/Mage/Sounds/What2.wav", "Assets/Units/Mage/Sounds/What3.wav"},
		Attack = {"Assets/Units/Archer/Sounds/Attack.wav"},
		Swing = {""},
		Move = {""},
		Meme = {""}

	},
}