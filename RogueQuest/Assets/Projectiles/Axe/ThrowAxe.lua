return 
{
	Name = "ThrowingAxe", -- used for Engine::GenerateProjectile()
	Files = 
	{
		{
			Name = "Axe",
			TargetSize = { 16, 64 },--Size of Arrow
			FileName = "Assets/Projectiles/Axe/ThrowAxe.png",
		},
	},
	Parameters =
	{
		CollisionSize = {64,64},
	},
	Stats =
	{	
		Spin = true,
		Damage = 25,
		Speed = 80
	},
}