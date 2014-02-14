#include "instance.h"

#include "state_intro.h"
#include "state_title.h"
#include "state_privacy.h"
#include "state_game.h"
#include "state_game_over.h"
#include "state_game_in.h"
#include "state_game_out.h"
#include "state_exit.h"

void rw_logic(void * data)
{
	RW_INSTANCE * ip = (RW_INSTANCE *)data;
	
	switch(ip->state)
	{
		case RW_STATE_INTRO:
		{
			rw_state_intro_logic(ip);
			break;
		}
		case RW_STATE_TITLE:
		{
			rw_state_title_logic(ip);
			break;
		}
		case RW_STATE_PRIVACY:
		{
			rw_state_privacy_logic(ip);
			break;
		}
		case RW_STATE_EXIT:
		{
			rw_state_exit_logic(ip);
			break;
		}
		case RW_STATE_GAME_IN:
		{
			rw_state_game_in_logic(ip);
			break;
		}
		case RW_STATE_GAME_OUT:
		{
			rw_state_game_out_logic(ip);
			break;
		}
		case RW_STATE_GAME_OVER:
		{
			rw_state_game_over_logic(ip);
			break;
		}
		case RW_STATE_GAME:
		{
			rw_state_game_logic(ip);
			break;
		}
	}
	if(ip->quit)
	{
		t3f_exit();
	}
}
