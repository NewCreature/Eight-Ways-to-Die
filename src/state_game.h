#ifndef RW_STATE_GAME_H
#define RW_STATE_GAME_H

#include "instance.h"

void rw_initialize_game(RW_INSTANCE * ip);

void rw_state_game_logic(RW_INSTANCE * ip);
void rw_state_game_render(RW_INSTANCE * ip);

#endif
