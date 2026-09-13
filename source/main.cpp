#include "Game.h"
#include <eng.h>

int main() 
{
    const int width = 1280;
    const int height = 720;

    Game* game = new Game();
    eng::Engine& engine = eng::Engine::GetInstance();

    engine.SetApplication(game);

    if (engine.Init(width, height))
    {
        engine.Run();
    }

    engine.Destroy();

    return 0;
}