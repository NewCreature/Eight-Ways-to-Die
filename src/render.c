#include "instance.h"

#include "state_intro.h"
#include "state_title.h"
#include "state_privacy.h"
#include "state_game.h"
#include "state_game_over.h"
#include "state_game_in.h"
#include "state_game_out.h"
#include "state_exit.h"

void rw_render(void * data)
{
	RW_INSTANCE * ip = (RW_INSTANCE *)data;
	
	al_hold_bitmap_drawing(true);
	switch(ip->state)
	{
		case RW_STATE_INTRO:
		{
			rw_state_intro_render(ip);
			break;
		}
		case RW_STATE_TITLE:
		{
			rw_state_title_render(ip);
			break;
		}
		case RW_STATE_PRIVACY:
		{
			rw_state_privacy_render(ip);
			break;
		}
		case RW_STATE_EXIT:
		{
			rw_state_exit_render(ip);
			break;
		}
		case RW_STATE_GAME_IN:
		case RW_STATE_GAME_OUT:
		{
			rw_state_game_in_render(ip);
			break;
		}
		case RW_STATE_GAME:
		case RW_STATE_GAME_OVER:
		{
			rw_state_game_render(ip);
			break;
		}
	}
	
	al_hold_bitmap_drawing(false);
}
