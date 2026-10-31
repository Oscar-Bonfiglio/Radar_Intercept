// ================================================================
//  main.cpp
//  Entry point.  Creates the Game and runs it.
// ================================================================

#include "engine/Game.h"
#include <iostream>

int main(int /*argc*/, char* /*argv*/[]) {
    std::cout << "=== Missile Simulation ===\n"
              << "  Arrow keys : move player aircraft\n"
              << "  SPACE      : fire missile at nearest enemy\n"
              << "  D          : toggle debug overlay\n"
              << "  ESC        : quit\n\n";

    Game game(900, 700);
    game.run();

    return 0;
}