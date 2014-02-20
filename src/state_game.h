#ifndef RW_STATE_GAME_H
#define RW_STATE_GAME_H

#include "instance.h"

#define RW_GAME_MODE_NORMAL 0
#define RW_GAME_MODE_EASY   1
#define RW_GAME_MODE_RHYTHM 2

void rw_initialize_game(RW_INSTANCE * ip);

void rw_state_game_particle_logic(RW_INSTANCE * ip);
void rw_state_game_shield_logic(RW_INSTANCE * ip);

void rw_state_game_logic(RW_INSTANCE * ip);
void rw_state_game_render(RW_INSTANCE * ip);

#endif
