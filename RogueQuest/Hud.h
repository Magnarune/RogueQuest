#pragma once
#include "olcPixelGameEngine.h"
#include "Building.h"
#include "Map.h"
#include "clock.h"

#include <array>

class Hud {
	void loadImage(const std::string& name, const std::string& path);
	void loadImage(const std::string& name, size_t tex_id);
public:
	std::map<std::string, std::unique_ptr<olc::Decal>> decals;
	Hud();
	virtual ~Hud();
	void ImportHudAssets();

	void Update(float delta);
	void RefreshMiniMap();//Always here
	void PreRenderMiniMap(const Map& current_map);	
	void DrawMiniMap();
	void DrawLeaderInfoHud();
	void DrawLeaderHud();
	void DrawHud();

	void DrawBuild();

	bool LeaderInfo = false;

	void DrawDescription(const std::string& vecText, olc::vi2d Mouse);
	void DrawDescriptionBorder(olc::Decal* box, const olc::vf2d& pos, const olc::vf2d& size, const olc::vf2d& borderScale = { 1.f,1.f });

	std::string WrapText(const std::string& str, int width, bool proportional, olc::vd2d scale);
	bool BuildMode = false;
	std::shared_ptr<Building> potBuilding;

	Clock minimapRefreshTimer;
	std::unique_ptr<olc::Sprite> minimapCanvas, minimapTileCanvas;
	std::unique_ptr<olc::Decal> minimapDecal;

	//olc::vf2d tl_MMap{11 , (float)gfx.ScreenHeight() * 0.823f - 10.f };
	//olc::vf2d br_MMap{57 , (float)gfx.ScreenHeight() * 0.823f + 36.f };

	//olc::vf2d tl_Zbox{ 11.f + 1.4375f * (float)worldManager->curMap().topleftTile.x , (float)gfx.ScreenHeight() * 0.823f - 10.0f + float(worldManager->curMap().topleftTile.y) * 1.5333f };
	//olc::vf2d br_Zbox{ 11.f + 1.4375f * float(worldManager->curMap().bottomRightTile.x,(float)gfx.ScreenHeight() * 0.823f - 10.0f + float(worldManager->curMap().bottomRightTile.y) * 1.5333f };

	//olc::vf2d cp_MMap{ 11.f + 1.4375f * Camera.vPOS.x / 32.f , ((float)gfx.ScreenHeight() * 0.823f - 10.0f + Camera.vPOS.y / 32.f * 1.5333f) };	olc::Sprite* MiniMap;

	olc::vf2d minimapSize;
	olc::vf2d frameSize;
	olc::vf2d minimapScale;
	olc::vf2d minimapPosition;
	
	friend class Game_Engine;
	friend class HudManager;
};