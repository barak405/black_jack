#include <stdio.h>
#include "data.h"

int main() {
    print_menu();
    GameState game;
    game_init(&game);
    run_game(&game);

    return 0;
}
