#pragma once
#include <iostream>
#include <string>
#include "olcPixelGameEngine.h"
#include "olcPGEX_TransformedView.h"
#include "sol/sol.hpp"
#include "Unit.h"
#include "Assets.h"
#include "clock.h"
class Game_Engine : public olc::PixelGameEngine
{
public:
    Game_Engine();
	olc::TileTransformedView tv;
    //lua_State* state;
	sol::table mapLayers; //Mine
    sol::state lua;
	sol::table mapData, tileSets;
	olc::vi2d vTileSize;
    std::vector<olc::Renderable*> renderGraphics;
	//Attempt two
	std::vector<int> vLayer;
	std::vector<std::vector<int>> vLayerData;
	sol::table layer;
	sol::table data;
	olc::vi2d offset;
	olc::vi2d layerSize;
	olc::vi2d pos;
	std::vector<std::string> vTileSet;
	std::vector<int> vFirstGid;
	//Attempt 3
	olc::vi2d TopleftTile;
	olc::vi2d BottomeRightTile;
	//Units 0.0
	std::vector<Unit*> vUnits;
    //CAMERA
	olc::vf2d CameraPos;
	olc::vi2d vVisabletiles;
    olc::vi2d POS;
    olc::vi2d SpriteOffset; 
    olc::vi2d fTileOffset;
	olc::vi2d Initial;
	bool Clicked = false;
    struct TileSet {
        olc::Renderable* Rfx;
        int gid;
    };
private:
    enum MapModes
    {
        MODE_LOCAL_MAP
    };
private:
    int m_nGameMode = MODE_LOCAL_MAP;
    TileSet GetTileSet(int id);
    olc::vi2d GetTile(int id);
    virtual void UpdateRect(olc::vi2d Initial, olc::vi2d Finial);
protected:
    bool OnUserCreate() override;
    bool OnUserUpdate(float fElapsedTime) override;
    bool OnUserDestroy() override;
    bool UpdateLocalMap(float fElapsedTime);
protected:

public:
    struct BouncyBall
    {
        olc::vf2d POSITION;
        olc::vf2d VELOCITY;
        bool Selected = false;
        bool Targeted = false;
        olc::vf2d Facing;
        olc::vf2d  Target;
        olc::vf2d Pathing;
    };
	std::vector<BouncyBall> Balls;
	//Debug Decals
	olc::Sprite* Demo = nullptr;
	olc::Decal* DemoDecal = nullptr;
	olc::Sprite* Erzaspr = nullptr;
	olc::Decal* ErzaDcl = nullptr;
	olc::Sprite* Flagspr = nullptr;
	olc::Decal* FlagDcl = nullptr;
	olc::Sprite* Backroundspr = nullptr;
	olc::Decal* Backrounddcl = nullptr;
	olc::vf2d Rotator;
	float fAngle;
	//Debug Decals
	float TimeofDay= 101;
	float FTIME = 0;
	bool DayTime = true;
	bool NightTime = false;


	void UpdateAndDraw(const float fElapsedTime)
	{
		for (auto& ball : Balls)
		{
			ball.Pathing = { ball.Target.x - ball.POSITION.x,ball.Target.y - ball.POSITION.y };
			ball.POSITION += ball.VELOCITY * fElapsedTime;

			float NormilizeVector = sqrtf(ball.VELOCITY.x * ball.VELOCITY.x + ball.VELOCITY.y * ball.VELOCITY.y);
			ball.Facing = { 4.0f * (ball.VELOCITY.x) / NormilizeVector  ,  4.0f * (ball.VELOCITY.y) / NormilizeVector };
			//NormilizeVector = NormilizeVector / NormilizeVector;

			if (ball.POSITION.x < 4.0f || ball.POSITION.x >(ScreenWidth() - 4.0f))
			{
				ball.POSITION.x = std::clamp(ball.POSITION.x, 4.0f, (ScreenWidth() - 4.0f));
				ball.VELOCITY.x *= -1.0f;
			}
			if (ball.POSITION.y < 4 || ball.POSITION.y >(ScreenHeight() - 4))
			{
				ball.POSITION.y = std::clamp(ball.POSITION.y, 4.0f, (ScreenHeight() - 4.0f));
				ball.VELOCITY.y *= -1.0f;
			}

			if (ball.Targeted == true)
			{
				ball.VELOCITY.x = ball.Target.x * 0.6f + (rand() % 9) / 2.0f;
				ball.VELOCITY.y = ball.Target.y * 0.6f + (rand() % 9) / 2.0f;
				ball.Targeted = false;
				//ball.VELOCITY = { 0.0f,0.0f };
			}
		}
		for (const auto& ball : Balls)
		{

			if (ball.Selected == true)
			{
				tv.DrawDecal({ ball.POSITION.x - float(Erzaspr->width) / 10.0f, ball.POSITION.y - float(Erzaspr->height) / 10.0f }, ErzaDcl, { 0.2f, 0.2f });
				//DrawLine(ball.POSITION.x, ball.POSITION.y, ball.Facing.x + ball.POSITION.x, ball.Facing.y + ball.POSITION.y, olc::GREEN);
				for (int i = 1; i <= 6; i++)
				{
					int S = 2;
					int R = 30;

					float Angle = 3.14159f * i / 3.0f;
					Rotator = { ball.POSITION.x + R * cosf(S * fAngle + Angle) ,ball.POSITION.y + R * sinf(S * fAngle + Angle) };
					tv.DrawRotatedDecal(Rotator, DemoDecal, S * fAngle + 0.785f + Angle, { float(Demo->width) / 2.0f, float(Demo->height) / 2.0f });
				}
			}
			else
			{
				tv.DrawDecal({ ball.POSITION.x - float(Erzaspr->width) / 10.0f, ball.POSITION.y - float(Erzaspr->height) / 10.0f }, ErzaDcl, { 0.2f, 0.2f }, olc::DARK_GREY);
				//DrawLine(ball.POSITION.x, ball.POSITION.y, ball.Facing.x + ball.POSITION.x, ball.Facing.y + ball.POSITION.y, olc::GREEN);
			}

		}

	}
	void AddBall(float radius = 4.0f)
	{
		float fAngle = float(rand()) / float(RAND_MAX) * 2.0f * 3.14159f - 0.3f;
		float fSpeed = float(rand()) / float(RAND_MAX) * 100.0f + 20.0f;
		Balls.push_back(
			{
			olc::vf2d(float(ScreenWidth()) / 2.0f, float(ScreenHeight()) / 2.0f) ,
			olc::vf2d(fSpeed * cos(fAngle), fSpeed * sin(fAngle))
			//bool Selected = false
			});
	}
	struct Player
	{
			olc::vf2d vPOS;
			olc::vf2d vVel;
			float fRadius = 10.0f;	
	};
	Player object;
	bool bFollowObject = false;
	//Rain
	struct Rain
	{
		float Rains;
		olc::vf2d Position;	
	};
	std::vector<Rain> RainDrops;
	olc::Sprite* Rain;
	olc::Decal* RainDcl;
	void UpdateRain(float FelapsedTime)
	{
		for (auto& Raindrops : RainDrops)
		{
			//Raindrops.PositionX += 1.0;
			//Raindrops.PositionY += 1.0;
			Raindrops.Position += {1.0, 1.0};
			//olc::vi2d Screen = tv.GetVisibleTiles();
			if (Raindrops.Position.x >tv.GetWorldBR().x)
			{
				Raindrops.Position.x = tv.GetWorldTL().x;
			}
			if (Raindrops.Position.y > tv.GetWorldBR().y)
			{
				Raindrops.Position.y = tv.GetWorldTL().y;
			}

			tv.DrawRotatedDecal( Raindrops.Position, RainDcl, 7 * 3.14159f / 4, {0,0}, { 0.01f, 0.01f },olc::Pixel((int)TimeofDay, (int)TimeofDay, (int)TimeofDay, 255));
		}
	}
   //Debug Commands
   bool OnConsoleCommand(const std::string& stext) override
   {
	   std::stringstream ss;
	   ss << stext;
	   std::string c1;
	   ss >> c1;
	   if (c1 == "addhead")
	   {
		   AddBall();
		   std::cout << "Head added\n";
	   }
	   
	   ss << stext;
	   std::string c2;
	   ss >> c2;
	   if (c2 == "add")
	   {
		   //for (int i = 0; i > 100; i++)
		   {
			//   AddBall();
		   }
		   std::cout << "Head!!! added\n";
	   }
	   std::string c3;
	   ss << stext;
	   ss >> c3;
	   if (c3 == "data")
	   {
		  // std::cout << "Data: " +StringifyObject(mapData) + "\n";
		   std::cout << "Data";
	   }

       return true;
   }
};