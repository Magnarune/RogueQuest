return 
{
	Name = "Bolt", -- used for Engine::GenerateProjectile()
	Files = 
	{
		{
			Name = "Normal",
			TargetSize = { 16, 64 },--Size of Arrow
			FileName = "Assets/Projectiles/Bolt/Bolt.png",
		},
	},
	Parameters =
	{
		CollisionSize = {64,64},
	},
	Stats =
	{
		Spin = false,
		Damage = 50,
		Speed = 80
	},
}