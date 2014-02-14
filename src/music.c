#include "instance.h"

void rw_toggle_music(RW_INSTANCE * ip)
{
	if(ip->music_on)
	{
		if(t3f_stream)
		{
			al_stop_timer(t3f_timer);
			t3f_stop_music();
			al_start_timer(t3f_timer);
		}
		ip->music_on = false;
	}
	else
	{
		ip->music_on = true;
		switch(ip->state)
		{
			case RW_STATE_INTRO:
			case RW_STATE_TITLE:
			case RW_STATE_GAME_OUT:
			{
				t3f_play_music("data/title.xm");
				break;
			}
			case RW_STATE_GAME_IN:
			case RW_STATE_GAME:
			case RW_STATE_GAME_OVER:
			{
				t3f_play_music("data/bgm.it");
				break;
			}
		}
	}
}
