#define OLC_PGE_APPLICATION
#define OLC_PGEX_TRANSFORMEDVIEW
#define OLC_SOUNDWAVE
#include "Engine.h"

int main()
{//1024, 960 , 1 ,1 // 512, 480, 2, 2 //256 , 240 , 4 ,4
    {

       
        Game_Engine Game;
        if (Game.Construct(480 , 270, 4, 4,false)) {
            Game.Start();
        }
    }

    return 0;
}
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow){
    return main();
}