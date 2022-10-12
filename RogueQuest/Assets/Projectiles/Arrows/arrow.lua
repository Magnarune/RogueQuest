return 
{
	Name = "Arrow", -- used for Engine::GenerateProjectile()
	Files = 
	{
		{
			Name = "Normal",
			TargetSize = { 16, 64 },--Size of Arrow
			FileName = "Assets/Projectiles/Arrows/arrow.png",
		},
	},
	Parameters =
	{
		CollisionSize = {64,64},
	},
	Stats =
	{
		Spin = false,
		Damage = 5,
		Speed = 60
	},
}