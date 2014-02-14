#include "instance.h"

RW_INSTANCE * rw_create_instance(void)
{
	RW_INSTANCE * ip;
	int i;
	
	ip = al_malloc(sizeof(RW_INSTANCE));
	if(ip)
	{
		ip->intro_z = -700.0;
		ip->intro_planet_z = 40.0;
		ip->intro_planet_angle = 0.0;
		ip->intro_planet_vangle = 0.0;
		ip->intro_state = 0;
		ip->intro_ticker = 0;
		ip->start_alpha = 0.0;
		ip->logo_z = 0.0;

		/* game data */
		ip->ticks = 0;
		ip->quit = 0;
		for(i = 0; i < RW_MAX_BITMAPS; i++)
		{
			ip->bitmap[i] = NULL;
		}
		ip->font = NULL;
		for(i = 0; i < RW_MAX_SAMPLES; i++)
		{
			ip->sample[i] = NULL;
		}
		ip->atlas = NULL;
		ip->game_mode = RW_GAME_MODE_NORMAL;
		ip->control_mode = RW_CONTROL_MODE_SWIPE;

		/* swipe data */
		ip->swipe_state = RW_SWIPE_INACTIVE;
		ip->score = 0;
		ip->high_score = 1800;
		ip->new_high_score = false;
		ip->threat_count = 0;
		ip->level = 0;
		ip->damage = 0;
		ip->damage_time = 0;
		ip->alert_ticks = 0;
		ip->alert_audio_ticks = 0;
		ip->camera_z = 0.0;
		ip->camera_target_z = 0.0;
		ip->state = RW_STATE_INTRO;
		ip->music_on = true;
		ip->vertical_scale = 1.0;
		ip->third = 160.0;
	}
	return ip;
}

void rw_destroy_instance(RW_INSTANCE * ip)
{
	al_free(ip);
}
