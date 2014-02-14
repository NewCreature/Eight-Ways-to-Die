#include "instance.h"

void rw_state_game_out_logic(RW_INSTANCE * ip)
{
	ip->intro_planet_z -= 10.0;
	ip->intro_planet_angle += 0.01;
	if(ip->intro_planet_z <= -400.0)
	{
		ip->intro_planet_z = -400.0;
		ip->state = RW_STATE_TITLE;
	}
}
