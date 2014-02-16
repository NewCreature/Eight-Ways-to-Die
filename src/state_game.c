#include "t3f/t3f.h"
#include "t3f/draw.h"

#include "instance.h"

#include "control_keyboard.h"
#include "control_swipe.h"
#include "control_hotspot.h"

#include "threat.h"

/* the eight ways from which to die */
static float rw_eight_ways[8] = 
{
	((2.0 * ALLEGRO_PI) / 8.0) * 5.0,
	((2.0 * ALLEGRO_PI) / 8.0) * 6.0,
	((2.0 * ALLEGRO_PI) / 8.0) * 7.0,
	((2.0 * ALLEGRO_PI) / 8.0) * 8.0,
	((2.0 * ALLEGRO_PI) / 8.0) * 9.0,
	((2.0 * ALLEGRO_PI) / 8.0) * 10.0,
	((2.0 * ALLEGRO_PI) / 8.0) * 11.0,
	((2.0 * ALLEGRO_PI) / 8.0) * 12.0,
};

/* level variables */
static int rw_level_threat_ticks[RW_MAX_LEVELS] =
{
	60, 58, 56, 54, 52,
	50, 48, 46, 44, 42,
	40, 38, 36, 34, 32,
	30, 28, 26, 24, 20
};

static int rw_level_prob_total[RW_MAX_LEVELS] =
{
	100, 100, 100, 100, 100,
	100, 100, 100, 100, 100,
	100, 100, 100, 100, 100,
	100, 100, 100, 100, 100
};

static int rw_level_prob[RW_MAX_LEVELS] =
{
	50, 51, 52, 53, 54,
	55, 56, 57, 58, 59,
	60, 61, 62, 63, 64,
	65, 66, 67, 68, 70
};

static float rw_level_camera_z[RW_MAX_LEVELS] =
{
	   0.0,  -10.0,  -20.0,  -30.0,  -40.0,
	 -50.0,  -60.0,  -70.0,  -80.0,  -90.0,
	-100.0, -110.0, -120.0, -130.0, -140.0,
	-150.0, -160.0, -170.0, -180.0, -190.0
};

static void rw_generate_threat(RW_INSTANCE * ip)
{
	int i;
	int r;
	float rs;
	float a;
	float ar;
	
	for(i = 0; i < RW_MAX_THREATS; i++)
	{
		if(!ip->threat[i].active)
		{
			r = t3f_rand(&ip->rng_state) % 8;
			rs = 1.6 + (float)(t3f_rand(&ip->rng_state) % 100) / 40.0;
			if(r == 1 && r == 5)
			{
				rs *= ip->vertical_scale;
			}
			ar = (float)(t3f_rand(&ip->rng_state) % 100) / 1000.0 - 0.05;
			a = rw_eight_ways[r] + (t3f_drand(&ip->rng_state) - 0.5) / 10.0;
			ip->threat[i].x = 320.0 + cos(a) * 400.0;
			ip->threat[i].y = 240.0 + sin(a) * 400.0;
			ip->threat[i].vx = cos(a) * -rs;
			ip->threat[i].vy = sin(a) * -rs;
			ip->threat[i].angle = 0.0;
			ip->threat[i].vangle = (float)((float)(t3f_rand(&ip->rng_state) % 100) / 400.0 - 0.125);
			ip->threat[i].type = r;
			ip->threat[i].size = 0;
			ip->threat[i].active = true;
			ip->threat_count++;
			break;
		}
	}
}

static void rw_generate_particle(RW_INSTANCE * ip, float x, float y, float min_angle, float max_angle)
{
	int i;
	float a, r;
	
	if(min_angle > max_angle)
	{
		min_angle -= ALLEGRO_PI * 2.0;
	}
	for(i = 0; i < RW_MAX_PARTICLES; i++)
	{
		if(!ip->particle[i].active)
		{
			ip->particle[i].x = x;
			ip->particle[i].y = y;
			a = t3f_drand(&ip->rng_state) * (max_angle - min_angle) + min_angle;
			r = t3f_drand(&ip->rng_state) * (1.5 - 0.5) + 0.5;
			ip->particle[i].vx = cos(a) * r;
			ip->particle[i].vy = sin(a) * r;
			ip->particle[i].life = 30 + t3f_rand(&ip->rng_state) % 15;
			ip->particle[i].tlife = ip->particle[i].life;
			ip->particle[i].active = true;
			break;
		}
	}
}

static void rw_destroy_everything(RW_INSTANCE * ip)
{
	int i, j;
	int c = 0;
	
	for(i = 0; i < RW_MAX_THREATS; i++)
	{
		if(ip->threat[i].active)
		{
			for(j = 0; j < 8; j++)
			{
				rw_generate_particle(ip, ip->threat[i].x, ip->threat[i].y, 0.0, ALLEGRO_PI * 2.0);
			}
			c++;
			ip->threat[i].active = false;
		}
	}
	for(i = 0; i < RW_MAX_SHOTS; i++)
	{
		if(ip->shot[i].active)
		{
			for(j = 0; j < 6; j++)
			{
				rw_generate_particle(ip, ip->shot[i].x, ip->shot[i].y, 0.0, ALLEGRO_PI * 2.0);
			}
			c++;
			ip->shot[i].active = false;
		}
	}
	for(i = 0; i < RW_MAX_SHIPS; i++)
	{
		if(ip->ship[i].active)
		{
			for(j = 0; j < 16; j++)
			{
				rw_generate_particle(ip, ip->ship[i].x, ip->ship[i].y, 0.0, ALLEGRO_PI * 2.0);
			}
			c++;
			ip->ship[i].active = false;
		}
	}
	if(c)
	{
		al_play_sample(ip->sample[RW_SAMPLE_DAMAGE], 0.5, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
	}
}

void rw_initialize_game(RW_INSTANCE * ip)
{
	int i;
	
	for(i = 0; i < RW_MAX_SHIELDS; i++)
	{
		ip->shield_generator.shield[i].life = 0;
		ip->shield_generator.shield[i].charge = 0;
	}
	ip->shield_generator.power = 0;
	
	for(i = 0; i < RW_MAX_THREATS; i++)
	{
		ip->threat[i].active = false;
	}
	
	for(i = 0; i < RW_MAX_SHIPS; i++)
	{
		ip->ship[i].active = false;
	}
	
	for(i = 0; i < RW_MAX_SHOTS; i++)
	{
		ip->shot[i].active = false;
	}
	
	for(i = 0; i < RW_MAX_PARTICLES; i++)
	{
		ip->particle[i].active = false;
	}
	
	ip->score = 0;
	ip->new_high_score = false;
	ip->threat_count = 0;
	ip->level = 0;
	ip->damage = 0;
	ip->damage_time = 0;
	ip->alert_ticks = 0;
	ip->camera_z = 0.0;
	ip->camera_target_z = 0.0;
	ip->alert_audio_ticks = 0;
	ip->swipe_state = RW_SWIPE_INACTIVE;
}

static void rw_deal_damage(RW_INSTANCE * ip)
{
	ip->damage++;
	ip->damage_time = 20;
	if(ip->damage > 4)
	{
		rw_destroy_everything(ip);
		ip->state = RW_STATE_GAME_OVER;
		ip->intro_planet_z = ip->camera_z;
		ip->alert_ticks = 0;
		ip->swipe_state = RW_SWIPE_INACTIVE;
	}
}

void rw_state_game_particle_logic(RW_INSTANCE * ip)
{
	int i;

	for(i = 0; i < RW_MAX_PARTICLES; i++)
	{
		if(ip->particle[i].active)
		{
			ip->particle[i].x += ip->particle[i].vx;
			ip->particle[i].y += ip->particle[i].vy;
			ip->particle[i].life--;
			if(ip->particle[i].life <= 0)
			{
				ip->particle[i].active = false;
			}
		}
	}
}

void rw_state_game_shield_logic(RW_INSTANCE * ip)
{
	int i;
	
	for(i = 0; i < RW_MAX_SHIELDS; i++)
	{
		if(ip->shield_generator.shield[i].life > 0)
		{
			ip->shield_generator.shield[i].life--;
			if(ip->shield_generator.shield[i].life <= 0)
			{
				ip->shield_generator.shield[i].active = false;
			}
		}
	}
}

void rw_state_game_logic(RW_INSTANCE * ip)
{
	int key = 0;
	int touch_key = 0;
	int dt = 0;
	float min_angle, max_angle;
	int i, j, r;
	
	key = rw_keyboard_logic(ip);
	
	if(ip->control_mode == RW_CONTROL_MODE_SWIPE)
	{
		touch_key = rw_swipe_logic(ip);
		if(touch_key)
		{
			key = touch_key;
		}
	}
	else if(ip->control_mode == RW_CONTROL_MODE_HOT_SPOTS)
	{
		touch_key = rw_hotspot_logic(ip);
		if(touch_key)
		{
			key = touch_key;
		}
	}
	switch(key)
	{
		case 27:
		{
			rw_destroy_everything(ip);
			ip->state = RW_STATE_GAME_OVER;
			ip->intro_planet_z = ip->camera_z;
			break;
		}
		case '7':
		{
			al_play_sample(ip->sample[RW_SAMPLE_SHIELD], 0.5, 0.25, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
			ip->shield_generator.shield[0].life = RW_SHIELD_MAX_LIFE;
			ip->shield_generator.shield[0].active = true;
			break;
		}
		case '8':
		{
			al_play_sample(ip->sample[RW_SAMPLE_SHIELD], 0.5, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
			ip->shield_generator.shield[1].life = RW_SHIELD_MAX_LIFE;
			ip->shield_generator.shield[1].active = true;
			break;
		}
		case '9':
		{
			al_play_sample(ip->sample[RW_SAMPLE_SHIELD], 0.5, 0.75, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
			ip->shield_generator.shield[2].life = RW_SHIELD_MAX_LIFE;
			ip->shield_generator.shield[2].active = true;
			break;
		}
		case '6':
		{
			al_play_sample(ip->sample[RW_SAMPLE_SHIELD], 0.5, 0.85, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
			ip->shield_generator.shield[3].life = RW_SHIELD_MAX_LIFE;
			ip->shield_generator.shield[3].active = true;
			break;
		}
		case '3':
		{
			al_play_sample(ip->sample[RW_SAMPLE_SHIELD], 0.5, 0.75, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
			ip->shield_generator.shield[4].life = RW_SHIELD_MAX_LIFE;
			ip->shield_generator.shield[4].active = true;
			break;
		}
		case '2':
		{
			al_play_sample(ip->sample[RW_SAMPLE_SHIELD], 0.5, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
			ip->shield_generator.shield[5].life = RW_SHIELD_MAX_LIFE;
			ip->shield_generator.shield[5].active = true;
			break;
		}
		case '1':
		{
			al_play_sample(ip->sample[RW_SAMPLE_SHIELD], 0.5, 0.25, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
			ip->shield_generator.shield[6].life = RW_SHIELD_MAX_LIFE;
			ip->shield_generator.shield[6].active = true;
			break;
		}
		case '4':
		{
			al_play_sample(ip->sample[RW_SAMPLE_SHIELD], 0.5, 0.15, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
			ip->shield_generator.shield[7].life = RW_SHIELD_MAX_LIFE;
			ip->shield_generator.shield[7].active = true;
			break;
		}
	}
	/* camera logic */
	if(ip->camera_z > rw_level_camera_z[ip->level])
	{
		ip->camera_z -= 0.5;
	}
	
	rw_state_game_particle_logic(ip);
	rw_state_game_shield_logic(ip);
	
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
	if(ip->damage >= 4)
	{
		ip->alert_ticks++;
		if(ip->alert_ticks > 20)
		{
			ip->alert_ticks = 0;
		}
		if(ip->alert_audio_ticks == 0)
		{
			al_play_sample(ip->sample[RW_SAMPLE_ALERT], 0.5, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
		}
		ip->alert_audio_ticks++;
		if(ip->alert_audio_ticks >= 60)
		{
			ip->alert_audio_ticks = 0;
		}
	}
	for(i = 0; i < RW_MAX_THREATS; i++)
	{
		if(ip->threat[i].active)
		{
			ip->threat[i].x += ip->threat[i].vx;
			ip->threat[i].y += ip->threat[i].vy;
			ip->threat[i].angle += ip->threat[i].vangle;
			if(t3f_distance(320.0, 240.0, ip->threat[i].x, ip->threat[i].y) < 36.0)
			{
				al_play_sample(ip->sample[RW_SAMPLE_DAMAGE], 0.5, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				if(ip->threat[i].type == 0)
				{
					min_angle = rw_eight_ways[7];
					max_angle = rw_eight_ways[1];
				}
				else if(ip->threat[i].type == 7)
				{
					min_angle = rw_eight_ways[6];
					max_angle = rw_eight_ways[0];
				}
				else
				{
					min_angle = rw_eight_ways[ip->threat[i].type - 1];
					max_angle = rw_eight_ways[ip->threat[i].type + 1];
				}
				for(j = 0; j < 8; j++)
				{
					rw_generate_particle(ip, ip->threat[i].x, ip->threat[i].y, min_angle, max_angle);
				}
				ip->threat[i].active = 0;
				rw_deal_damage(ip);
			}
			else if(t3f_distance(320.0, 240.0, ip->threat[i].x, ip->threat[i].y) < 48.0)
			{
				if(ip->shield_generator.shield[ip->threat[i].type].active)
				{
					al_play_sample(ip->sample[RW_SAMPLE_HIT], 0.5, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
					if(ip->threat[i].type == 0)
					{
						min_angle = rw_eight_ways[7];
						max_angle = rw_eight_ways[1];
					}
					else if(ip->threat[i].type == 7)
					{
						min_angle = rw_eight_ways[6];
						max_angle = rw_eight_ways[0];
					}
					else
					{
						min_angle = rw_eight_ways[ip->threat[i].type - 1];
						max_angle = rw_eight_ways[ip->threat[i].type + 1];
					}
					for(j = 0; j < 5; j++)
					{
						rw_generate_particle(ip, ip->threat[i].x, ip->threat[i].y, min_angle, max_angle);
					}
					ip->threat[i].active = 0;
					ip->shield_generator.shield[ip->threat[i].type].active = 0;
					ip->shield_generator.shield[ip->threat[i].type].life = RW_SHIELD_MAX_LIFE;
				}
			}
		}
	}
	
	for(i = 0; i < RW_MAX_SHOTS; i++)
	{
		if(ip->shot[i].active)
		{
			ip->shot[i].x += ip->shot[i].vx;
			ip->shot[i].y += ip->shot[i].vy;
			if(t3f_distance(320.0, 240.0, ip->shot[i].x, ip->shot[i].y) < 36.0)
			{
				al_play_sample(ip->sample[RW_SAMPLE_HIT], 0.5, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				if(ip->shot[i].type == 0)
				{
					min_angle = rw_eight_ways[7];
					max_angle = rw_eight_ways[1];
				}
				else if(ip->shot[i].type == 7)
				{
					min_angle = rw_eight_ways[6];
					max_angle = rw_eight_ways[0];
				}
				else
				{
					min_angle = rw_eight_ways[ip->shot[i].type - 1];
					max_angle = rw_eight_ways[ip->shot[i].type + 1];
				}
				for(j = 0; j < 5; j++)
				{
					rw_generate_particle(ip, ip->shot[i].x, ip->shot[i].y, min_angle, max_angle);
				}
				ip->shot[i].active = 0;
				rw_deal_damage(ip);
			}
			else if(t3f_distance(320.0, 240.0, ip->shot[i].x, ip->shot[i].y) < 48.0)
			{
				if(ip->shield_generator.shield[ip->shot[i].type].active)
				{
					al_play_sample(ip->sample[RW_SAMPLE_HIT], 0.5, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
					if(ip->shot[i].type == 0)
					{
						min_angle = rw_eight_ways[7];
						max_angle = rw_eight_ways[1];
					}
					else if(ip->shot[i].type == 7)
					{
						min_angle = rw_eight_ways[6];
						max_angle = rw_eight_ways[0];
					}
					else
					{
						min_angle = rw_eight_ways[ip->shot[i].type - 1];
						max_angle = rw_eight_ways[ip->shot[i].type + 1];
					}
					for(j = 0; j < 3; j++)
					{
						rw_generate_particle(ip, ip->shot[i].x, ip->shot[i].y, min_angle, max_angle);
					}
					ip->shot[i].active = 0;
					ip->shield_generator.shield[ip->shot[i].type].active = 0;
					ip->shield_generator.shield[ip->shot[i].type].life = RW_SHIELD_MAX_LIFE;
//					ip->score += 25;
				}
			}
		}
	}
	
	for(i = 0; i < RW_MAX_SHIPS; i++)
	{
		if(ip->ship[i].active)
		{
			ip->ship[i].angle += ip->ship[i].vangle;
			if(ip->ship[i].vangle > 0.0)
			{
				if(ip->ship[i].way == 0)
				{
					if(ip->ship[i].angle >= rw_eight_ways[ip->ship[i].way] + ALLEGRO_PI * 2.0)
					{
						ip->ship[i].angle = rw_eight_ways[ip->ship[i].way];
						ip->ship[i].vangle = 0.0;
					}
				}
				else
				{
					if(ip->ship[i].angle >= rw_eight_ways[ip->ship[i].way])
					{
						ip->ship[i].angle = rw_eight_ways[ip->ship[i].way];
						ip->ship[i].vangle = 0.0;
					}
				}
			}
			else if(ip->ship[i].vangle < 0.0)
			{
				if(ip->ship[i].way == 7)
				{
					if(ip->ship[i].angle <= rw_eight_ways[ip->ship[i].way] - ALLEGRO_PI * 2.0)
					{
						ip->ship[i].angle = rw_eight_ways[ip->ship[i].way];
						ip->ship[i].vangle = 0.0;
					}
				}
				else
				{
					if(ip->ship[i].angle <= rw_eight_ways[ip->ship[i].way])
					{
						ip->ship[i].angle = rw_eight_ways[ip->ship[i].way];
						ip->ship[i].vangle = 0.0;
					}
				}
			}
			ip->ship[i].dist += ip->ship[i].vdist;
			if(ip->ship[i].dist <= ip->ship[i].dest)
			{
				ip->ship[i].vdist = 0.0;
			}
			ip->ship[i].x = 320.0 + cos(ip->ship[i].angle) * ip->ship[i].dist;
			ip->ship[i].y = 240.0 + sin(ip->ship[i].angle) * ip->ship[i].dist;
			if(ip->ship[i].vangle == 0.0 && ip->ship[i].vdist == 0.0)
			{
				ip->ship[i].ticks++;
				if(ip->ship[i].ticks == 15)
				{
					r = t3f_rand(&ip->rng_state) % 100;
					if(r < 75)
					{
						for(j = 0; j < RW_MAX_SHOTS; j++)
						{
							if(!ip->shot[j].active)
							{
								ip->shot[j].x = 320.0 + cos(ip->ship[i].angle) * ip->ship[i].dist;
								ip->shot[j].y = 240.0 + sin(ip->ship[i].angle) * ip->ship[i].dist;
								ip->shot[j].vx = cos(ip->ship[i].angle) * -3.0;
								ip->shot[j].vy = sin(ip->ship[i].angle) * -3.0;
								ip->shot[j].type = ip->ship[i].way;
								ip->shot[j].active = true;
								break;
							}
						}
					}
				}
				if(ip->ship[i].ticks > 30)
				{
					r = t3f_rand(&ip->rng_state) % 100;
					if(r < 75)
					{
						ip->ship[i].way++;
						if(ip->ship[i].way > 7)
						{
							ip->ship[i].way = 0;
						}
						ip->ship[i].vangle = 0.005;
					}
					else
					{
						ip->ship[i].way--;
						if(ip->ship[i].way < 0)
						{
							ip->ship[i].way = 7;
						}
						ip->ship[i].vangle = -0.005;
					}
					ip->ship[i].ticks = 0;
				}
			}
		}
	}
	
	ip->ticks++;
	ip->score++;
	if(ip->score > ip->high_score)
	{
		ip->high_score = ip->score;
		if(!ip->new_high_score)
		{
			al_play_sample(ip->sample[RW_SAMPLE_HIGH_SCORE], 0.5, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
			ip->new_high_score = true;
		}
	}
	
	/* generate enemies */
	if(ip->ticks > rw_level_threat_ticks[ip->level] && t3f_rand(&ip->rng_state) % rw_level_prob_total[ip->level] < rw_level_prob[ip->level])
	{
		rw_generate_threat(ip);
		if(ip->threat_count > 15 && ip->level < 19)
		{
			ip->threat_count = 0;
			al_play_sample(ip->sample[RW_SAMPLE_LEVEL_UP], 0.5, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
			ip->level++;
			if(ip->level == 9)
			{
				ip->ship[0].way = t3f_rand(&ip->rng_state) % 8;
				ip->ship[0].angle = rw_eight_ways[ip->ship[0].way];
				ip->ship[0].vangle = 0.0;
				ip->ship[0].dist = 320.0;
				ip->ship[0].dest = 200.0;
				ip->ship[0].vdist = -2.5;
				ip->ship[0].ticks = 0;
				ip->ship[0].active = true;
			}
			if(ip->level == 14)
			{
				ip->ship[1].way = t3f_rand(&ip->rng_state) % 8;
				ip->ship[1].angle = rw_eight_ways[ip->ship[1].way];
				ip->ship[1].vangle = 0.0;
				ip->ship[1].dist = 320.0;
				ip->ship[1].dest = 160.0;
				ip->ship[1].vdist = -2.5;
				ip->ship[1].ticks = 0;
				ip->ship[1].active = true;
			}
		}
		ip->ticks = 0;
	}
	if(t3f_key[ALLEGRO_KEY_ESCAPE] || t3f_key[ALLEGRO_KEY_BACK])
	{
		ip->state = RW_STATE_GAME_OVER;
	}
}

void rw_state_game_render(RW_INSTANCE * ip)
{
	int i;
	ALLEGRO_COLOR color;
	float alpha, beta;

	al_clear_to_color(al_map_rgb(0, 0, 0));
	if(ip->damage >= 4)
	{
		alpha = 1.0 - (float)ip->alert_ticks / 80.0;
		color = al_map_rgba_f(alpha, (1.0 - (float)ip->damage_time / 20.0) * alpha, (1.0 - (float)ip->damage_time / 20.0) * alpha, alpha);
	}
	else
	{
		color = al_map_rgba_f(1.0, 1.0 - (float)ip->damage_time / 20.0, 1.0 - (float)ip->damage_time / 20.0, 1.0);
	}
	t3f_draw_bitmap(ip->bitmap[RW_BITMAP_WORLD], color, 640 / 2 - al_get_bitmap_width(ip->bitmap[RW_BITMAP_WORLD]) / 2, 480 / 2 - al_get_bitmap_height(ip->bitmap[RW_BITMAP_WORLD]) / 2, ip->camera_z, 0);
	for(i = 0; i < 8; i++)
	{
		if(ip->shield_generator.shield[i].life > 0)
		{
			alpha = (float)ip->shield_generator.shield[i].life / (float)(RW_SHIELD_MAX_LIFE);
			beta = ip->shield_generator.shield[i].active ? 1.0 : 0.0;
			t3f_draw_bitmap(ip->bitmap[RW_BITMAP_SHIELD_0 + i], al_map_rgba_f(alpha, beta * alpha, beta * alpha, alpha), 640 / 2 - al_get_bitmap_width(ip->bitmap[RW_BITMAP_SHIELD_0 + i]) / 2, 480 / 2 - al_get_bitmap_height(ip->bitmap[RW_BITMAP_SHIELD_0 + i]) / 2, ip->camera_z, 0);
		}
	}
	for(i = 0; i < RW_MAX_PARTICLES; i++)
	{
		if(ip->particle[i].active)
		{
			alpha = (float)ip->particle[i].life / (float)ip->particle[i].tlife;
			t3f_draw_bitmap(ip->bitmap[RW_BITMAP_PARTICLE], al_map_rgba_f(alpha, alpha, alpha, alpha), ip->particle[i].x, ip->particle[i].y, ip->camera_z, 0);
		}
	}
	for(i = 0; i < RW_MAX_THREATS; i++)
	{
		if(ip->threat[i].active)
		{
			t3f_draw_rotated_bitmap(ip->bitmap[RW_BITMAP_THREAT], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), al_get_bitmap_width(ip->bitmap[RW_BITMAP_THREAT]) / 2, al_get_bitmap_height(ip->bitmap[RW_BITMAP_THREAT]) / 2, ip->threat[i].x, ip->threat[i].y, ip->camera_z, ip->threat[i].angle, 0);
		}
	}
	for(i = 0; i < RW_MAX_SHOTS; i++)
	{
		if(ip->shot[i].active)
		{
			t3f_draw_bitmap(ip->bitmap[RW_BITMAP_SHOT], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), ip->shot[i].x - (float)al_get_bitmap_width(ip->bitmap[RW_BITMAP_SHOT]) / 2, ip->shot[i].y - (float)al_get_bitmap_height(ip->bitmap[RW_BITMAP_SHOT]) / 2, ip->camera_z, 0);
		}
	}
	for(i = 0; i < RW_MAX_SHIPS; i++)
	{
		if(ip->ship[i].active)
		{
			t3f_draw_rotated_bitmap(ip->bitmap[RW_BITMAP_SHIP], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), al_get_bitmap_width(ip->bitmap[RW_BITMAP_SHIP]) / 2, al_get_bitmap_height(ip->bitmap[RW_BITMAP_SHIP]) / 2, ip->ship[i].x, ip->ship[i].y, ip->camera_z, -ip->ship[i].angle, 0);
		}
	}
	al_draw_textf(ip->font, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 320 - al_get_text_width(ip->font, "HIGH SCORE - 0:00:00") / 2, t3f_display_top, 0, "HIGH SCORE - %d:%02d:%02d", ip->high_score / 3600, (ip->high_score / 60) % 60, (int)(((float)(ip->high_score % 60) / 60.0) * 100.0) % 100);
	al_draw_textf(ip->font, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 320 - al_get_text_width(ip->font, "SCORE - 0:00:00") / 2, t3f_display_top + 16, 0, "SCORE - %d:%02d:%02d", ip->score / 3600, (ip->score / 60) % 60, (int)(((float)(ip->score % 60) / 60.0) * 100.0) % 100);
	al_draw_textf(ip->font, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 320, t3f_display_top + 32, ALLEGRO_ALIGN_CENTRE, "LEVEL - %d", ip->level + 1);
	if(ip->control_mode == RW_CONTROL_MODE_HOT_SPOTS)
	{
		rw_render_hot_spots(ip);
	}
	else if(ip->control_mode == RW_CONTROL_MODE_SWIPE)
	{
		rw_render_swipe(ip);
	}
//	al_font_textprintf_centre(rw_font, 320, 32, "%d : %f : %f : %f", rw_ship[0].way, rw_eight_ways[rw_ship[0].way], rw_ship[0].angle, rw_ship[0].vangle);
}
