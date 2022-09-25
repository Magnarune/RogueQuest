#define OLC_PGE_APPLICATION
#define OLC_PGEX_TRANSFORMEDVIEW
#include "Engine.h"

int main()
{//1024, 960 , 1 ,1 // 512, 480, 2, 2
    {

       
        Game_Engine Game;
        if (Game.Construct(256 , 240, 4, 4)) {
            Game.Start();
        }
    }

    return 0;
}
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow){
    return main();
}