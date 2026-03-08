#include "Game.h"

int main(int argc, char** argv)
{
    Scene game{};
    game.Init(argv[0]);
    game.Run(KGR::Tools::Chrono<float>::Time::CreateFromValue(1.0f/60.0f));
}