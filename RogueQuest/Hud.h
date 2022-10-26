#pragma once
#include "olcPixelGameEngine.h"
#include "Building.h"
#include "Map.h"

class Hud {


	void loadImage(const std::string& name, const std::string& path);
	void loadImage(const std::string& name, size_t tex_id);
public:
	std::map<std::string, std::unique_ptr<olc::Decal>> decals;
	Hud();
	virtual ~Hud();

	void ImportHudAssets();
	void Update(float delta);
	void RefreshMiniMap();
	void CalculateMiniMap(int x, int y, const Map& current_map);
	void DrawCenteredStringDecal(olc::vf2d pos, std::string str, olc::Pixel col = olc::WHITE, olc::vf2d scale = { 1,1 });
	void DrawMiniMap();
	void DrawHud();
	void DrawLeaderInfoHud();
	void DrawLeaderHud();
	void DrawBuild();
	bool LeaderInfo = false;
	void DrawDescription(std::string vecText, olc::vi2d Mouse);
	void DrawDescriptionBorder(olc::Decal* box, const olc::vf2d& pos, const olc::vf2d& size, const olc::vf2d& borderScale = { 1.f,1.f });
	std::string WrapText(std::string str, int width, bool proportional, olc::vd2d scale);
	bool BuildMode = false;
	std::shared_ptr<Building> potBuilding;
	//olc::vf2d tl_MMap{11 , (float)gfx.ScreenHeight() * 0.823f - 10.f };
	//olc::vf2d br_MMap{57 , (float)gfx.ScreenHeight() * 0.823f + 36.f };

	//olc::vf2d tl_Zbox{ 11.f + 1.4375f * (float)worldManager->curMap().topleftTile.x , (float)gfx.ScreenHeight() * 0.823f - 10.0f + float(worldManager->curMap().topleftTile.y) * 1.5333f };
	//olc::vf2d br_Zbox{ 11.f + 1.4375f * float(worldManager->curMap().bottomRightTile.x,(float)gfx.ScreenHeight() * 0.823f - 10.0f + float(worldManager->curMap().bottomRightTile.y) * 1.5333f };

	//olc::vf2d cp_MMap{ 11.f + 1.4375f * Camera.vPOS.x / 32.f , ((float)gfx.ScreenHeight() * 0.823f - 10.0f + Camera.vPOS.y / 32.f * 1.5333f) };	olc::Sprite* MiniMap;

	olc::vf2d Pixscale { 1.4375f,1.5333f };
	
	friend class Game_Engine;
	friend class HudManager;
};