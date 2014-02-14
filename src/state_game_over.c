#include "instance.h"

#include "state_game.h"

void rw_state_game_over_logic(RW_INSTANCE * ip)
{
	int i;
	int dt = 0;
	int pcount = 0;

	rw_state_game_particle_logic(ip);
	rw_state_game_shield_logic(ip);
	for(i = 0; i < RW_MAX_PARTICLES; i++)
	{
		if(ip->particle[i].active)
		{
			pcount++;
		}
	}
	if(pcount <= 0)
	{
		if(ip->music_on)
		{
			t3f_play_music("data/title.xm");
		}
		ip->state = RW_STATE_GAME_OUT;
	}
	
	switch(ip->damage)
	{
		case 0:
		{
			dt = 0;
			break;
		}
		case 1:
		{
			dt = 5;
			break;
		}
		case 2:
		{
			dt = 10;
			break;
		}
		case 3:
		{
			dt = 15;
			break;
		}
		case 4:
		{
			dt = 20;
			break;
		}
	}
	if(ip->damage_time > dt)
	{
		ip->damage_time--;
	}
}
