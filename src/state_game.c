#include "t3f/t3f.h"
#include "t3f/draw.h"
#include "t3f/sound.h"

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

/* sound panning info */
static float rw_pan_eight_ways[8] = 
{
	-0.25,
	  0.0,
	 0.25,
	  0.5,
	 0.25,
	  0.0,
	-0.25,
	 -0.5
};

/* level variables */
static int rw_threat_wait[RW_MAX_LEVELS] =
{
	90, 87, 84, 81, 78,
	75, 72, 69, 66, 63,
	60, 57, 54, 51, 48,
	45, 42, 39, 36, 33
};

static int rw_threat_variance[RW_MAX_LEVELS] =
{
	60, 60, 60, 60, 60,
	60, 60, 60, 60, 60,
	60, 60, 60, 60, 60,
	60, 60, 60, 60, 60
};

static float rw_level_threat_base_speed[RW_MAX_LEVELS] = 
{
	  1.6,  1.625,   1.65,  1.675,    1.7,
	1.725,   1.75,  1.775,    1.8,  1.825,
	 1.85,  1.875,    1.9,  1.925,   1.95,
	1.975,    2.0,  2.025,   2.05,  2.075
};

static float rw_level_threat_speed[RW_MAX_LEVELS] = 
{
	0.6, 0.7, 0.8, 0.9, 1.0,
	1.1, 1.2, 1.3, 1.4, 1.5,
	1.6, 1.7, 1.8, 1.9, 2.0,
	2.1, 2.2, 2.3, 2.4, 2.5
};

static float rw_level_camera_z[RW_MAX_LEVELS] =
{
	   0.0,   10.0,   20.0,   30.0,   40.0,
	  50.0,   60.0,   70.0,   80.0,   90.0,
	 100.0,  110.0,  120.0,  130.0,  140.0,
	 150.0,  160.0,  170.0,  180.0,  190.0
};

static int rw_big_threat_wait[RW_MAX_LEVELS] =
{
	30, 20, 20, 20, 20,
	18, 18, 16, 16, 14,
	14, 12, 12, 11, 11,
	10, 10,  9,  8,  7
};

static int rw_get_new_threat(RW_INSTANCE * ip)
{
	int i;

	for(i = 0; i < RW_MAX_THREATS; i++)
	{
		if(!ip->threat[i].active)
		{
			return i;
		}
	}
	return -1;
}

static int rw_generate_threat(RW_INSTANCE * ip, int type)
{
	int i;
	int r;
	
	i = rw_get_new_threat(ip);
	if(i >= 0)
	{
		if(!ip->threat[i].active)
		{
			switch(type)
			{
				case RW_THREAT_BASIC:
				{
					r = t3f_rand(&ip->rng_state) % 8;
//					ip->threat[i].gen_speed = 1.6 + (float)(t3f_rand(&ip->rng_state) % 100) / 40.0;
					ip->threat[i].gen_speed = rw_level_threat_base_speed[ip->level] + t3f_drand(&ip->rng_state) * rw_level_threat_speed[ip->level];
					if(r == 1 && r == 5)
					{
						ip->threat[i].gen_speed *= ip->vertical_scale;
					}
					ip->threat[i].gen_angle = rw_eight_ways[r] + (t3f_drand(&ip->rng_state) - 0.5) / 10.0;
					ip->threat[i].x = 320.0 + cos(ip->threat[i].gen_angle) * 400.0;
					ip->threat[i].y = 240.0 + sin(ip->threat[i].gen_angle) * 400.0;
					ip->threat[i].vx = cos(ip->threat[i].gen_angle) * -ip->threat[i].gen_speed;
					ip->threat[i].vy = sin(ip->threat[i].gen_angle) * -ip->threat[i].gen_speed;
					ip->threat[i].angle = 0.0;
					ip->threat[i].vangle = (float)((float)(t3f_rand(&ip->rng_state) % 100) / 400.0 - 0.125);
					ip->threat[i].pos = r;
					ip->threat[i].size = 0;
					ip->threat[i].type = type;
					ip->threat[i].active = true;
					break;
				}
				case RW_THREAT_LARGE:
				{
					r = t3f_rand(&ip->rng_state) % 8;
					ip->threat[i].gen_speed = rw_level_threat_base_speed[ip->level] + t3f_drand(&ip->rng_state) * rw_level_threat_speed[ip->level];
					if(r == 1 && r == 5)
					{
						ip->threat[i].gen_speed *= ip->vertical_scale;
					}
					ip->threat[i].gen_angle = rw_eight_ways[r] + (t3f_drand(&ip->rng_state) - 0.5) / 10.0;
					ip->threat[i].x = 320.0 + cos(ip->threat[i].gen_angle) * 400.0;
					ip->threat[i].y = 240.0 + sin(ip->threat[i].gen_angle) * 400.0;
					ip->threat[i].vx = cos(ip->threat[i].gen_angle) * -ip->threat[i].gen_speed;
					ip->threat[i].vy = sin(ip->threat[i].gen_angle) * -ip->threat[i].gen_speed;
					ip->threat[i].angle = 0.0;
					ip->threat[i].vangle = (float)((float)(t3f_rand(&ip->rng_state) % 100) / 400.0 - 0.125);
					ip->threat[i].pos = r;
					ip->threat[i].size = 0;
					ip->threat[i].type = type;
					ip->threat[i].active = true;
					break;
				}
			}
		}
	}
	return i;
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
		t3f_play_sample(ip->sample[RW_SAMPLE_DAMAGE], 0.5, 0.0, 1.0);
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
	ip->level = 0;
	ip->damage = 0;
	ip->damage_time = 0;
	ip->alert_ticks = 0;
	ip->camera_z = 0.0;
	ip->camera_target_z = 0.0;
	ip->planet_z = 0.0;
	ip->alert_audio_ticks = 0;
	ip->threat_wait = rw_threat_wait[ip->level] + (t3f_rand(&ip->rng_state) % rw_threat_variance[ip->level]) - 30;
	ip->big_threat_wait = rw_big_threat_wait[ip->level] + t3f_rand(&ip->rng_state) % 5;
	for(i = 0; i < T3F_MAX_TOUCHES; i++)
	{
		ip->swipe[i].state = RW_SWIPE_INACTIVE;
	}
	if(ip->music_on)
	{
		t3f_play_music("data/bgm.it");
	}
}

static void rw_deal_damage(RW_INSTANCE * ip)
{
	int i;

	ip->damage++;
	ip->damage_time = 20;
	if(ip->damage > 4)
	{
		rw_destroy_everything(ip);
		ip->state = RW_STATE_GAME_OVER;
		ip->intro_planet_z = ip->camera_z;
		ip->alert_ticks = 0;
		for(i = 0; i < T3F_MAX_TOUCHES; i++)
		{
			ip->swipe[i].state = RW_SWIPE_INACTIVE;
		}
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

static void rw_state_game_control_logic(RW_INSTANCE * ip)
{
	int key, touch_key;
	
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
			t3f_play_sample(ip->sample[RW_SAMPLE_SHIELD], 0.5, rw_pan_eight_ways[0], 1.0);
			ip->shield_generator.shield[0].life = RW_SHIELD_MAX_LIFE;
			ip->shield_generator.shield[0].active = true;
			break;
		}
		case '8':
		{
			t3f_play_sample(ip->sample[RW_SAMPLE_SHIELD], 0.5, rw_pan_eight_ways[1], 1.0);
			ip->shield_generator.shield[1].life = RW_SHIELD_MAX_LIFE;
			ip->shield_generator.shield[1].active = true;
			break;
		}
		case '9':
		{
			t3f_play_sample(ip->sample[RW_SAMPLE_SHIELD], 0.5, rw_pan_eight_ways[2], 1.0);
			ip->shield_generator.shield[2].life = RW_SHIELD_MAX_LIFE;
			ip->shield_generator.shield[2].active = true;
			break;
		}
		case '6':
		{
			t3f_play_sample(ip->sample[RW_SAMPLE_SHIELD], 0.5, rw_pan_eight_ways[3], 1.0);
			ip->shield_generator.shield[3].life = RW_SHIELD_MAX_LIFE;
			ip->shield_generator.shield[3].active = true;
			break;
		}
		case '3':
		{
			t3f_play_sample(ip->sample[RW_SAMPLE_SHIELD], 0.5, rw_pan_eight_ways[4], 1.0);
			ip->shield_generator.shield[4].life = RW_SHIELD_MAX_LIFE;
			ip->shield_generator.shield[4].active = true;
			break;
		}
		case '2':
		{
			t3f_play_sample(ip->sample[RW_SAMPLE_SHIELD], 0.5, rw_pan_eight_ways[5], 1.0);
			ip->shield_generator.shield[5].life = RW_SHIELD_MAX_LIFE;
			ip->shield_generator.shield[5].active = true;
			break;
		}
		case '1':
		{
			t3f_play_sample(ip->sample[RW_SAMPLE_SHIELD], 0.5, rw_pan_eight_ways[6], 1.0);
			ip->shield_generator.shield[6].life = RW_SHIELD_MAX_LIFE;
			ip->shield_generator.shield[6].active = true;
			break;
		}
		case '4':
		{
			t3f_play_sample(ip->sample[RW_SAMPLE_SHIELD], 0.5, rw_pan_eight_ways[7], 1.0);
			ip->shield_generator.shield[7].life = RW_SHIELD_MAX_LIFE;
			ip->shield_generator.shield[7].active = true;
			break;
		}
	}
}

static void rw_break_large_threat(RW_INSTANCE * ip, int i)
{
	int j, r, tr;
	double vd[2] = {4.0, 3.7};
	double va[2] = {0.005, 0.006};
	
	tr = t3f_rand(&ip->rng_state) % 2;
	j = rw_get_new_threat(ip);
	if(j >= 0)
	{
		r = ip->threat[i].pos - 1;
		if(r < 0)
		{
			r = 7;
		}
		ip->threat[j].gen_speed = -1.6;
		ip->threat[j].gen_angle = rw_eight_ways[ip->threat[i].pos];
		ip->threat[j].gen_vangle = -va[tr];
		ip->threat[j].d = t3f_distance(320, 240, ip->threat[i].x, ip->threat[i].y);
		ip->threat[j].vd = vd[tr];
		ip->threat[j].x = ip->threat[i].x;
		ip->threat[j].y = ip->threat[i].y;
		ip->threat[j].vx = cos(ip->threat[j].gen_angle) * -ip->threat[j].gen_speed;
		ip->threat[j].vy = sin(ip->threat[j].gen_angle) * -ip->threat[j].gen_speed;
		ip->threat[j].angle = 0.0;
		ip->threat[j].vangle = (float)((float)(t3f_rand(&ip->rng_state) % 100) / 400.0 - 0.125);
		ip->threat[j].pos = r;
		ip->threat[j].size = 0;
		ip->threat[j].type = RW_THREAT_PIECE;
		ip->threat[j].active = true;
	}
	tr = (tr + 1) % 2;
	j = rw_get_new_threat(ip);
	if(j >= 0)
	{
		r = ip->threat[i].pos + 1;
		r = r % 8;
		ip->threat[j].gen_speed = -1.6;
		ip->threat[j].gen_angle = rw_eight_ways[ip->threat[i].pos];
		ip->threat[j].gen_vangle = va[tr];
		ip->threat[j].d = t3f_distance(320, 240, ip->threat[i].x, ip->threat[i].y);
		ip->threat[j].vd = vd[tr];
		ip->threat[j].x = ip->threat[i].x;
		ip->threat[j].y = ip->threat[i].y;
		ip->threat[j].vx = cos(ip->threat[j].gen_angle) * -ip->threat[j].gen_speed;
		ip->threat[j].vy = sin(ip->threat[j].gen_angle) * -ip->threat[j].gen_speed;
		ip->threat[j].angle = 0.0;
		ip->threat[j].vangle = (float)((float)(t3f_rand(&ip->rng_state) % 100) / 400.0 - 0.125);
		ip->threat[j].pos = r;
		ip->threat[j].size = 0;
		ip->threat[j].type = RW_THREAT_PIECE;
		ip->threat[j].active = true;
	}
}

static void rw_state_game_threat_logic(RW_INSTANCE * ip, int i)
{
	float min_angle, max_angle;
	int j;

	if(ip->threat[i].active)
	{
		switch(ip->threat[i].type)
		{
			case RW_THREAT_BASIC:
			{
				ip->threat[i].x += ip->threat[i].vx;
				ip->threat[i].y += ip->threat[i].vy;
				ip->threat[i].angle += ip->threat[i].vangle;
				if(t3f_distance(320.0, 240.0, ip->threat[i].x, ip->threat[i].y) < 36.0)
				{
					t3f_play_sample(ip->sample[RW_SAMPLE_DAMAGE], 0.5, rw_pan_eight_ways[ip->threat[i].pos], 1.0);
					if(ip->threat[i].pos == 0)
					{
						min_angle = rw_eight_ways[7];
						max_angle = rw_eight_ways[1];
					}
					else if(ip->threat[i].pos == 7)
					{
						min_angle = rw_eight_ways[6];
						max_angle = rw_eight_ways[0];
					}
					else
					{
						min_angle = rw_eight_ways[ip->threat[i].pos - 1];
						max_angle = rw_eight_ways[ip->threat[i].pos + 1];
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
					if(ip->shield_generator.shield[ip->threat[i].pos].active)
					{
						t3f_play_sample(ip->sample[RW_SAMPLE_HIT], 0.5, rw_pan_eight_ways[ip->threat[i].pos], 1.0);
						if(ip->threat[i].pos == 0)
						{
							min_angle = rw_eight_ways[7];
							max_angle = rw_eight_ways[1];
						}
						else if(ip->threat[i].pos == 7)
						{
							min_angle = rw_eight_ways[6];
							max_angle = rw_eight_ways[0];
						}
						else
						{
							min_angle = rw_eight_ways[ip->threat[i].pos - 1];
							max_angle = rw_eight_ways[ip->threat[i].pos + 1];
						}
						for(j = 0; j < 5; j++)
						{
							rw_generate_particle(ip, ip->threat[i].x, ip->threat[i].y, min_angle, max_angle);
						}
						ip->threat[i].active = 0;
						ip->shield_generator.shield[ip->threat[i].pos].active = 0;
						ip->shield_generator.shield[ip->threat[i].pos].life = RW_SHIELD_MAX_LIFE;
					}
				}
				break;
			}
			case RW_THREAT_LARGE:
			{
				ip->threat[i].x += ip->threat[i].vx;
				ip->threat[i].y += ip->threat[i].vy;
				ip->threat[i].angle += ip->threat[i].vangle;
				if(t3f_distance(320.0, 240.0, ip->threat[i].x, ip->threat[i].y) < 44.0)
				{
					t3f_play_sample(ip->sample[RW_SAMPLE_DAMAGE], 0.5, rw_pan_eight_ways[ip->threat[i].pos], 1.0);
					if(ip->threat[i].pos == 0)
					{
						min_angle = rw_eight_ways[7];
						max_angle = rw_eight_ways[1];
					}
					else if(ip->threat[i].pos == 7)
					{
						min_angle = rw_eight_ways[6];
						max_angle = rw_eight_ways[0];
					}
					else
					{
						min_angle = rw_eight_ways[ip->threat[i].pos - 1];
						max_angle = rw_eight_ways[ip->threat[i].pos + 1];
					}
					for(j = 0; j < 8; j++)
					{
						rw_generate_particle(ip, ip->threat[i].x, ip->threat[i].y, min_angle, max_angle);
					}
					rw_break_large_threat(ip, i);
					ip->threat[i].active = 0;
					rw_deal_damage(ip);
				}
				else if(t3f_distance(320.0, 240.0, ip->threat[i].x, ip->threat[i].y) < 52.0)
				{
					if(ip->shield_generator.shield[ip->threat[i].pos].active)
					{
						t3f_play_sample(ip->sample[RW_SAMPLE_HIT], 0.5, rw_pan_eight_ways[ip->threat[i].pos], 1.0);
						if(ip->threat[i].pos == 0)
						{
							min_angle = rw_eight_ways[7];
							max_angle = rw_eight_ways[1];
						}
						else if(ip->threat[i].pos == 7)
						{
							min_angle = rw_eight_ways[6];
							max_angle = rw_eight_ways[0];
						}
						else
						{
							min_angle = rw_eight_ways[ip->threat[i].pos - 1];
							max_angle = rw_eight_ways[ip->threat[i].pos + 1];
						}
						for(j = 0; j < 5; j++)
						{
							rw_generate_particle(ip, ip->threat[i].x, ip->threat[i].y, min_angle, max_angle);
						}
						rw_break_large_threat(ip, i);
						ip->threat[i].active = 0;
						ip->shield_generator.shield[ip->threat[i].pos].active = 0;
						ip->shield_generator.shield[ip->threat[i].pos].life = RW_SHIELD_MAX_LIFE;
					}
				}
				break;
			}
			case RW_THREAT_PIECE:
			{
				ip->threat[i].gen_angle += ip->threat[i].gen_vangle;
				ip->threat[i].vd += -0.05;
				ip->threat[i].d += ip->threat[i].vd;
				ip->threat[i].x = 320.0 + cos(ip->threat[i].gen_angle) * ip->threat[i].d;
				ip->threat[i].y = 240. + sin(ip->threat[i].gen_angle) * ip->threat[i].d;
				ip->threat[i].angle += ip->threat[i].vangle;
				if(t3f_distance(320.0, 240.0, ip->threat[i].x, ip->threat[i].y) < 36.0)
				{
					t3f_play_sample(ip->sample[RW_SAMPLE_DAMAGE], 0.5, rw_pan_eight_ways[ip->threat[i].pos], 1.0);
					if(ip->threat[i].pos == 0)
					{
						min_angle = rw_eight_ways[7];
						max_angle = rw_eight_ways[1];
					}
					else if(ip->threat[i].pos == 7)
					{
						min_angle = rw_eight_ways[6];
						max_angle = rw_eight_ways[0];
					}
					else
					{
						min_angle = rw_eight_ways[ip->threat[i].pos - 1];
						max_angle = rw_eight_ways[ip->threat[i].pos + 1];
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
					if(ip->shield_generator.shield[ip->threat[i].pos].active && ip->threat[i].vd <= 0.0)
					{
						t3f_play_sample(ip->sample[RW_SAMPLE_HIT], 0.5, rw_pan_eight_ways[ip->threat[i].pos], 1.0);
						if(ip->threat[i].pos == 0)
						{
							min_angle = rw_eight_ways[7];
							max_angle = rw_eight_ways[1];
						}
						else if(ip->threat[i].pos == 7)
						{
							min_angle = rw_eight_ways[6];
							max_angle = rw_eight_ways[0];
						}
						else
						{
							min_angle = rw_eight_ways[ip->threat[i].pos - 1];
							max_angle = rw_eight_ways[ip->threat[i].pos + 1];
						}
						for(j = 0; j < 5; j++)
						{
							rw_generate_particle(ip, ip->threat[i].x, ip->threat[i].y, min_angle, max_angle);
						}
						ip->threat[i].active = 0;
						ip->shield_generator.shield[ip->threat[i].pos].active = 0;
						ip->shield_generator.shield[ip->threat[i].pos].life = RW_SHIELD_MAX_LIFE;
					}
				}
				break;
			}
		}
	}
}

static void rw_state_game_shot_logic(RW_INSTANCE * ip, int i)
{
	float min_angle, max_angle;
	int j;
	
	if(ip->shot[i].active)
	{
		ip->shot[i].x += ip->shot[i].vx;
		ip->shot[i].y += ip->shot[i].vy;
		if(t3f_distance(320.0, 240.0, ip->shot[i].x, ip->shot[i].y) < 36.0)
		{
			t3f_play_sample(ip->sample[RW_SAMPLE_HIT], 0.5, rw_pan_eight_ways[ip->shot[i].pos], 1.0);
			if(ip->shot[i].pos == 0)
			{
				min_angle = rw_eight_ways[7];
				max_angle = rw_eight_ways[1];
			}
			else if(ip->shot[i].pos == 7)
			{
				min_angle = rw_eight_ways[6];
				max_angle = rw_eight_ways[0];
			}
			else
			{
				min_angle = rw_eight_ways[ip->shot[i].pos - 1];
				max_angle = rw_eight_ways[ip->shot[i].pos + 1];
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
			if(ip->shield_generator.shield[ip->shot[i].pos].active)
			{
				t3f_play_sample(ip->sample[RW_SAMPLE_HIT], 0.5, rw_pan_eight_ways[ip->shot[i].pos], 1.0);
				if(ip->shot[i].pos == 0)
				{
					min_angle = rw_eight_ways[7];
					max_angle = rw_eight_ways[1];
				}
				else if(ip->shot[i].pos == 7)
				{
					min_angle = rw_eight_ways[6];
					max_angle = rw_eight_ways[0];
				}
				else
				{
					min_angle = rw_eight_ways[ip->shot[i].pos - 1];
					max_angle = rw_eight_ways[ip->shot[i].pos + 1];
				}
				for(j = 0; j < 3; j++)
				{
					rw_generate_particle(ip, ip->shot[i].x, ip->shot[i].y, min_angle, max_angle);
				}
				ip->shot[i].active = 0;
				ip->shield_generator.shield[ip->shot[i].pos].active = 0;
				ip->shield_generator.shield[ip->shot[i].pos].life = RW_SHIELD_MAX_LIFE;
			}
		}
	}
}

static void rw_state_game_ship_logic(RW_INSTANCE * ip, int i)
{
	int j, r;

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
							ip->shot[j].pos = ip->ship[i].way;
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

void rw_state_game_logic(RW_INSTANCE * ip)
{
	int dt = 0;
	int i;
	
	rw_state_game_control_logic(ip);

	/* camera logic */
	if(ip->camera_z < rw_level_camera_z[ip->level])
	{
		ip->camera_z += 0.5;
	}
	if(ip->planet_z < 0)
	{
		ip->planet_z += 5.0;
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
			t3f_play_sample(ip->sample[RW_SAMPLE_ALERT], 0.5, 0.0, 1.0);
		}
		ip->alert_audio_ticks++;
		if(ip->alert_audio_ticks >= 60)
		{
			ip->alert_audio_ticks = 0;
		}
	}
	for(i = 0; i < RW_MAX_THREATS; i++)
	{
		rw_state_game_threat_logic(ip, i);
	}
	
	for(i = 0; i < RW_MAX_SHOTS; i++)
	{
		rw_state_game_shot_logic(ip, i);
	}
	
	for(i = 0; i < RW_MAX_SHIPS; i++)
	{
		rw_state_game_ship_logic(ip, i);
	}
	
	ip->ticks++;
	ip->score++;
	if(ip->score > ip->high_score)
	{
		ip->high_score = ip->score;
		if(!ip->new_high_score)
		{
			t3f_play_sample(ip->sample[RW_SAMPLE_HIGH_SCORE], 0.5, 0.0, 1.0);
			ip->new_high_score = true;
		}
	}
	
	/* generate enemies */
	if(ip->ticks > ip->threat_wait)
	{
		ip->big_threat_wait--;
		if(ip->big_threat_wait <= 0)
		{
			rw_generate_threat(ip, RW_THREAT_LARGE);
			ip->big_threat_wait = rw_big_threat_wait[ip->level] + t3f_rand(&ip->rng_state) % 5;
		}
		else
		{
			rw_generate_threat(ip, RW_THREAT_BASIC);
		}
		ip->threat_wait = rw_threat_wait[ip->level] + (t3f_rand(&ip->rng_state) % rw_threat_variance[ip->level]) - 30;
		ip->ticks = 0;
	}
	if(ip->score % 900 == 0 && ip->level < 19)
	{
		t3f_play_sample(ip->sample[RW_SAMPLE_LEVEL_UP], 0.5, 0.0, 1.0);
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
	t3f_draw_bitmap(ip->bitmap[RW_BITMAP_WORLD], color, 640 / 2 - al_get_bitmap_width(ip->bitmap[RW_BITMAP_WORLD]) / 2, 480 / 2 - al_get_bitmap_height(ip->bitmap[RW_BITMAP_WORLD]) / 2, ip->planet_z - ip->camera_z, 0);
	for(i = 0; i < 8; i++)
	{
		if(ip->shield_generator.shield[i].life > 0)
		{
			alpha = (float)ip->shield_generator.shield[i].life / (float)(RW_SHIELD_MAX_LIFE);
			beta = ip->shield_generator.shield[i].active ? 1.0 : 0.0;
			t3f_draw_bitmap(ip->bitmap[RW_BITMAP_SHIELD_0 + i], al_map_rgba_f(alpha, beta * alpha, beta * alpha, alpha), 640 / 2 - al_get_bitmap_width(ip->bitmap[RW_BITMAP_SHIELD_0 + i]) / 2, 480 / 2 - al_get_bitmap_height(ip->bitmap[RW_BITMAP_SHIELD_0 + i]) / 2, -ip->camera_z, 0);
		}
	}
	for(i = 0; i < RW_MAX_PARTICLES; i++)
	{
		if(ip->particle[i].active)
		{
			alpha = (float)ip->particle[i].life / (float)ip->particle[i].tlife;
			t3f_draw_bitmap(ip->bitmap[RW_BITMAP_PARTICLE], al_map_rgba_f(alpha, alpha, alpha, alpha), ip->particle[i].x, ip->particle[i].y, -ip->camera_z, 0);
		}
	}
	for(i = 0; i < RW_MAX_THREATS; i++)
	{
		if(ip->threat[i].active)
		{
			switch(ip->threat[i].type)
			{
				case RW_THREAT_BASIC:
				{
					t3f_draw_rotated_bitmap(ip->bitmap[RW_BITMAP_THREAT], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), al_get_bitmap_width(ip->bitmap[RW_BITMAP_THREAT]) / 2, al_get_bitmap_height(ip->bitmap[RW_BITMAP_THREAT]) / 2, ip->threat[i].x, ip->threat[i].y, -ip->camera_z, ip->threat[i].angle, 0);
					break;
				}
				case RW_THREAT_LARGE:
				{
					t3f_draw_scaled_rotated_bitmap(ip->bitmap[RW_BITMAP_BIG_THREAT], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), al_get_bitmap_width(ip->bitmap[RW_BITMAP_THREAT]) / 2, al_get_bitmap_height(ip->bitmap[RW_BITMAP_THREAT]) / 2, ip->threat[i].x, ip->threat[i].y, -ip->camera_z, ip->threat[i].angle, 2.0, 2.0, 0);
					break;
				}
				case RW_THREAT_PIECE:
				{
					t3f_draw_scaled_rotated_bitmap(ip->bitmap[RW_BITMAP_BIG_THREAT], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), al_get_bitmap_width(ip->bitmap[RW_BITMAP_THREAT]) / 2, al_get_bitmap_height(ip->bitmap[RW_BITMAP_THREAT]) / 2, ip->threat[i].x, ip->threat[i].y, -ip->camera_z, ip->threat[i].angle, 0.75, 0.75, 0);
					break;
				}
			}
		}
	}
	for(i = 0; i < RW_MAX_SHOTS; i++)
	{
		if(ip->shot[i].active)
		{
			t3f_draw_bitmap(ip->bitmap[RW_BITMAP_SHOT], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), ip->shot[i].x - (float)al_get_bitmap_width(ip->bitmap[RW_BITMAP_SHOT]) / 2, ip->shot[i].y - (float)al_get_bitmap_height(ip->bitmap[RW_BITMAP_SHOT]) / 2, -ip->camera_z, 0);
		}
	}
	for(i = 0; i < RW_MAX_SHIPS; i++)
	{
		if(ip->ship[i].active)
		{
			t3f_draw_rotated_bitmap(ip->bitmap[RW_BITMAP_SHIP], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), al_get_bitmap_width(ip->bitmap[RW_BITMAP_SHIP]) / 2, al_get_bitmap_height(ip->bitmap[RW_BITMAP_SHIP]) / 2, ip->ship[i].x, ip->ship[i].y, -ip->camera_z, ip->ship[i].angle, 0);
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
