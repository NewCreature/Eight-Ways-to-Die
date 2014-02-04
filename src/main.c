#include "t3f/t3f.h"
#include "t3f/rng.h"
#include "t3f/draw.h"
#include "t3f/resource.h"

#include "main.h"
#include "ocdfont.h"
#include "shield.h"
#include "threat.h"
#include "ship.h"
#include "particle.h"

/* intro data */
float rw_intro_z = -700.0;
float rw_intro_planet_z = 40.0;
float rw_intro_planet_angle = 0.0;
float rw_intro_planet_vangle = 0.0;
int   rw_intro_state = 0;
int   rw_intro_ticker = 0;
float rw_start_alpha = 0.0;
float rw_logo_z = 0.0;

/* game data */
T3F_RNG_STATE rw_rng_state;
int rw_ticks = 0;
int rw_quit = 0;
ALLEGRO_BITMAP * rw_bitmap[RW_MAX_BITMAPS] = {NULL};
ALLEGRO_FONT * rw_font = NULL;
ALLEGRO_SAMPLE * rw_sample[RW_MAX_SAMPLES] = {NULL};
T3F_ATLAS * rw_atlas = NULL;
int rw_score = 0;
int rw_high_score = 1800;
bool rw_new_high_score = false;
int rw_threat_count = 0;
int rw_level = 0;
int rw_damage = 0;
int rw_damage_time = 0;
int rw_alert_ticks = 0;
int rw_alert_audio_ticks = 0;
float rw_camera_z = 0.0;
float rw_camera_target_z = 0.0;
int rw_state = RW_STATE_INTRO;
bool rw_music_on = true;
float rw_vertical_scale = 1.0;
float rw_third = 160.0;

RW_SHIELD_GENERATOR rw_shield_generator;
RW_THREAT rw_threat[RW_MAX_THREATS];
RW_SHIP rw_ship[RW_MAX_SHIPS];
RW_SHOT rw_shot[RW_MAX_SHOTS];
RW_PARTICLE rw_particle[RW_MAX_PARTICLES];

/* the eight ways from which to die */
float rw_eight_ways[8] = 
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
int rw_level_threat_ticks[RW_MAX_LEVELS] =  {60,   58,    56,    54,    52,    50,    48,    46,    44,    42,    40,    38,   36,    34,    32,    30,    28,    26,    24,    20};
int rw_level_prob_total[RW_MAX_LEVELS] =   {100,  100,   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,   100};
int rw_level_prob[RW_MAX_LEVELS] =          {50,   51,    52,    53,    54,    55,    56,    57,    58,    59,    60,    61,    62,    63,    64,    65,    66,    67,    68,    70};
float rw_level_camera_z[RW_MAX_LEVELS] =   {0.0, -10.0, -20.0, -30.0, -40.0, -50.0, -60.0, -70.0, -80.0, -90.0, -100.0, -110.0, -120.0, -130.0, -140.0, -150.0, -160.0, -170.0, -180.0, -190.0};

int rw_save_high_score(const char * fn)
{
	ALLEGRO_FILE * fp;
	const ALLEGRO_FILE_INTERFACE * old_interface;
	
	old_interface = al_get_new_file_interface();
	al_set_standard_file_interface();
	
	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		return 0;
	}
	al_fwrite32le(fp, rw_high_score);
	al_fputc(fp, rw_music_on);
	al_fclose(fp);
	al_set_new_file_interface(old_interface);
	return 1;
}

int rw_load_high_score(const char * fn)
{
	ALLEGRO_FILE * fp;
	const ALLEGRO_FILE_INTERFACE * old_interface;
	
	old_interface = al_get_new_file_interface();
	al_set_standard_file_interface();
	
	
	fp = al_fopen(fn, "rb");
	if(!fp)
	{
		return 0;
	}
	rw_high_score = al_fread32le(fp);
	rw_music_on = al_fgetc(fp);
	al_fclose(fp);
	al_set_new_file_interface(old_interface);
	return 1;
}

void rw_toggle_music(void)
{
	if(rw_music_on)
	{
		if(t3f_stream)
		{
			al_stop_timer(t3f_timer);
			t3f_stop_music();
			al_start_timer(t3f_timer);
		}
		rw_music_on = false;
	}
	else
	{
		rw_music_on = true;
		switch(rw_state)
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

float rw_distance(float x1, float y1, float x2, float y2)
{
	return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

void rw_event_handler(ALLEGRO_EVENT * event)
{
	switch(event->type)
	{
		case ALLEGRO_EVENT_DISPLAY_CLOSE:
		{
			if(rw_state == RW_STATE_TITLE)
			{
				rw_state = RW_STATE_EXIT;
			}
			else
			{
				t3f_event_handler(event);
			}
			break;
		}
		case ALLEGRO_EVENT_DISPLAY_RESIZE:
		{
			t3f_event_handler(event);
			rw_vertical_scale = (t3f_display_bottom - t3f_display_top) / 480.0;
			rw_third = (t3f_display_bottom - t3f_display_top) / 3.0;
		}
		/* pass the event through to T3F for handling by default */
		default:
		{
			t3f_event_handler(event);
			break;
		}
	}
}

void rw_generate_threat(void)
{
	int i;
	int r;
	float rs;
	float a;
	float ar;
	
	for(i = 0; i < RW_MAX_THREATS; i++)
	{
		if(!rw_threat[i].active)
		{
			r = t3f_rand(&rw_rng_state) % 8;
			rs = 1.6 + (float)(t3f_rand(&rw_rng_state) % 100) / 40.0;
			if(r == 1 && r == 5)
			{
				rs *= rw_vertical_scale;
			}
			ar = (float)(t3f_rand(&rw_rng_state) % 100) / 1000.0 - 0.05;
			a = rw_eight_ways[r] + (t3f_drand(&rw_rng_state) - 0.5) / 10.0;
			rw_threat[i].x = 320.0 + cos(a) * 400.0;
			rw_threat[i].y = 240.0 + sin(a) * 400.0;
			rw_threat[i].vx = cos(a) * -rs;
			rw_threat[i].vy = sin(a) * -rs;
			rw_threat[i].angle = 0.0;
			rw_threat[i].vangle = (float)((float)(t3f_rand(&rw_rng_state) % 100) / 400.0 - 0.125);
			rw_threat[i].type = r;
			rw_threat[i].active = true;
			rw_threat_count++;
			break;
		}
	}
}

void rw_generate_particle(float x, float y, float min_angle, float max_angle)
{
	int i;
	float a, r;
	
	if(min_angle > max_angle)
	{
		min_angle -= ALLEGRO_PI * 2.0;
	}
	for(i = 0; i < RW_MAX_PARTICLES; i++)
	{
		if(!rw_particle[i].active)
		{
			rw_particle[i].x = x;
			rw_particle[i].y = y;
			a = t3f_drand(&rw_rng_state) * (max_angle - min_angle) + min_angle;
			r = t3f_drand(&rw_rng_state) * (1.5 - 0.5) + 0.5;
			rw_particle[i].vx = cos(a) * r;
			rw_particle[i].vy = sin(a) * r;
			rw_particle[i].life = 30 + t3f_rand(&rw_rng_state) % 15;
			rw_particle[i].tlife = rw_particle[i].life;
			rw_particle[i].active = true;
			break;
		}
	}
}

void rw_destroy_everything(void)
{
	int i, j;
	int c = 0;
	
	for(i = 0; i < RW_MAX_THREATS; i++)
	{
		if(rw_threat[i].active)
		{
			for(j = 0; j < 8; j++)
			{
				rw_generate_particle(rw_threat[i].x, rw_threat[i].y, 0.0, ALLEGRO_PI * 2.0);
			}
			c++;
			rw_threat[i].active = false;
		}
	}
	for(i = 0; i < RW_MAX_SHOTS; i++)
	{
		if(rw_shot[i].active)
		{
			for(j = 0; j < 6; j++)
			{
				rw_generate_particle(rw_shot[i].x, rw_shot[i].y, 0.0, ALLEGRO_PI * 2.0);
			}
			c++;
			rw_shot[i].active = false;
		}
	}
	for(i = 0; i < RW_MAX_SHIPS; i++)
	{
		if(rw_ship[i].active)
		{
			for(j = 0; j < 16; j++)
			{
				rw_generate_particle(rw_ship[i].x, rw_ship[i].y, 0.0, ALLEGRO_PI * 2.0);
			}
			c++;
			rw_ship[i].active = false;
		}
	}
	if(c)
	{
		al_play_sample(rw_sample[RW_SAMPLE_DAMAGE], 0.5, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
	}
}

void rw_initialize_game(void)
{
	int i;
	
	for(i = 0; i < RW_MAX_SHIELDS; i++)
	{
		rw_shield_generator.shield[i].life = 0;
		rw_shield_generator.shield[i].charge = 0;
	}
	rw_shield_generator.power = 0;
	
	for(i = 0; i < RW_MAX_THREATS; i++)
	{
		rw_threat[i].active = false;
	}
	
	for(i = 0; i < RW_MAX_SHIPS; i++)
	{
		rw_ship[i].active = false;
	}
	
	for(i = 0; i < RW_MAX_SHOTS; i++)
	{
		rw_shot[i].active = false;
	}
	
	for(i = 0; i < RW_MAX_PARTICLES; i++)
	{
		rw_particle[i].active = false;
	}
	
	rw_score = 0;
	rw_new_high_score = false;
	rw_threat_count = 0;
	rw_level = 0;
	rw_damage = 0;
	rw_damage_time = 0;
	rw_alert_ticks = 0;
	rw_camera_z = 0.0;
	rw_camera_target_z = 0.0;
	rw_alert_audio_ticks = 0;
}

static void rw_title_logo_logic(void)
{
	rw_logo_z -= 0.1;
	if(rw_logo_z <= -10.0)
	{
		rw_logo_z = 0.0;
	}
}

void rw_logic(void)
{
	int i, j, r;
	float min_angle = 0.0, max_angle = 0.0;
	int dt = 0;
	int key = 0;
	
	key = t3f_read_key(0);
	if(key == 'q' || key == 'Q')
	{
		key = '7';
	}
	else if(key == 'w' || key == 'W')
	{
		key = '8';
	}
	else if(key == 'e' || key == 'E')
	{
		key = '9';
	}
	else if(key == 'a' || key == 'A')
	{
		key = '4';
	}
	else if(key == 'd' || key == 'D')
	{
		key = '6';
	}
	else if(key == 'z' || key == 'Z')
	{
		key = '1';
	}
	else if(key == '5' || key == 's' || key == 'S' || key == 'x' || key == 'X')
	{
		key = '2';
	}
	else if(key == 'c' || key == 'C')
	{
		key = '3';
	}
	
	/* read one touch at a time and simulate key press for first touch detected */
	for(i = 0; i < T3F_MAX_TOUCHES; i++)
	{
		if(t3f_touch[i].active)
		{
			if(t3f_touch[i].x <= 160)
			{
				if(t3f_touch[i].y < t3f_display_top + rw_third)
				{
					key = '7';
				}
				else if(t3f_touch[i].y < t3f_display_top + rw_third * 2.0)
				{
					key = '4';
				}
				else
				{
					key = '1';
				}
			}
			else if(t3f_touch[i].x >= 640 - 160)
			{
				if(t3f_touch[i].y < t3f_display_top + rw_third)
				{
					key = '9';
				}
				else if(t3f_touch[i].y < t3f_display_top + rw_third * 2.0)
				{
					key = '6';
				}
				else
				{
					key = '3';
				}
			}
			else
			{
				if(t3f_touch[i].y < 240)
				{
					key = '8';
				}
				else
				{
					key = '2';
				}
			}
			t3f_touch[i].active = false;
			break;
		}
	}

	switch(rw_state)
	{
		case RW_STATE_INTRO:
		{
			switch(rw_intro_state)
			{
				case 0:
				{
					rw_intro_z += 10.0;
					if(rw_intro_z >= 0.0)
					{
						rw_intro_z = 0.0;
						rw_intro_state = 1;
						rw_intro_ticker = 0;
					}
					break;
				}
				case 1:
				{
					rw_intro_ticker++;
					if(rw_intro_ticker >= 120)
					{
						al_play_sample(rw_sample[RW_SAMPLE_LOGO_OUT], 0.5, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
						rw_intro_state = 2;
						rw_intro_ticker = 30;
					}
					break;
				}
				case 2:
				{
					rw_intro_z += 10.0;
					rw_intro_ticker--;
					if(rw_intro_ticker <= 0)
					{
						rw_intro_state = 3;
					}
					break;
				}
				case 3:
				{
					rw_intro_planet_z -= 10.0;
					if(rw_intro_planet_z <= -400.0)
					{
						rw_intro_planet_z = -400.0;
						rw_state = RW_STATE_TITLE;
					}
					rw_intro_planet_angle += 0.01;
					break;
				}
			}
			if(t3f_key[ALLEGRO_KEY_ESCAPE] || t3f_key[ALLEGRO_KEY_BACK])
			{
				rw_quit = 1;
			}
			break;
		}
		case RW_STATE_TITLE:
		{
			rw_intro_planet_angle += 0.01;
			if(rw_intro_planet_angle >= ALLEGRO_PI * 2.0)
			{
				rw_intro_planet_angle -= ALLEGRO_PI * 2.0;
			}
			rw_start_alpha += 0.025;
			if(rw_start_alpha > 1.0)
			{
				rw_start_alpha = 0.0;
			}
			rw_title_logo_logic();
			if(key == '1')
			{
				rw_state = RW_STATE_PRIVACY;
			}
			else if(key == '2')
			{
				al_play_sample(rw_sample[RW_SAMPLE_GAME_START], 0.5, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				rw_intro_planet_vangle = rw_intro_planet_angle / (rw_intro_planet_z / 10.0);
				rw_initialize_game();
				rw_state = RW_STATE_GAME_IN;
				if(rw_music_on)
				{
					t3f_play_music("data/bgm.it");
				}
			}
			else if(key == '3')
			{
				rw_toggle_music();
			}
			if(t3f_key[ALLEGRO_KEY_ESCAPE] || t3f_key[ALLEGRO_KEY_BACK])
			{
				rw_state = RW_STATE_EXIT;
				t3f_key[ALLEGRO_KEY_ESCAPE] = 0;
				t3f_key[ALLEGRO_KEY_BACK] = 0;
			}
			break;
		}
		case RW_STATE_PRIVACY:
		{
			rw_title_logo_logic();
			if(t3f_key[ALLEGRO_KEY_ESCAPE] || t3f_key[ALLEGRO_KEY_BACK] || key)
			{
				rw_state = RW_STATE_TITLE;
				t3f_key[ALLEGRO_KEY_ESCAPE] = 0;
				t3f_key[ALLEGRO_KEY_BACK] = 0;
			}
			break;
		}
		case RW_STATE_EXIT:
		{
			rw_intro_planet_z += 10.0;
			rw_intro_planet_angle += 0.01;
			if(rw_intro_planet_z >= 0.0)
			{
				rw_quit = 1;
			}
			break;
		}
		case RW_STATE_GAME_IN:
		{
			rw_intro_planet_z += 10.0;
			if(rw_intro_planet_z >= 0.0)
			{
				rw_state = RW_STATE_GAME;
			}
			rw_intro_planet_angle += rw_intro_planet_vangle;
			break;
		}
		case RW_STATE_GAME_OUT:
		{
			rw_intro_planet_z -= 10.0;
			rw_intro_planet_angle += 0.01;
			if(rw_intro_planet_z <= -400.0)
			{
				rw_intro_planet_z = -400.0;
				rw_state = RW_STATE_TITLE;
			}
			break;
		}
		case RW_STATE_GAME_OVER:
		{
			int pcount = 0;
			for(i = 0; i < RW_MAX_PARTICLES; i++)
			{
				if(rw_particle[i].active)
				{
					rw_particle[i].x += rw_particle[i].vx;
					rw_particle[i].y += rw_particle[i].vy;
					rw_particle[i].life--;
					if(rw_particle[i].life <= 0)
					{
						rw_particle[i].active = false;
					}
					pcount++;
				}
			}
			if(pcount <= 0)
			{
				if(rw_music_on)
				{
					t3f_play_music("data/title.xm");
				}
				rw_state = RW_STATE_GAME_OUT;
			}
			
			for(i = 0; i < RW_MAX_SHIELDS; i++)
			{
				if(rw_shield_generator.shield[i].life > 0)
				{
					rw_shield_generator.shield[i].life--;
					if(rw_shield_generator.shield[i].life <= 0)
					{
						rw_shield_generator.shield[i].active = false;
					}
				}
			}
			switch(rw_damage)
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
			if(rw_damage_time > dt)
			{
				rw_damage_time--;
			}
			break;
		}
		case RW_STATE_GAME:
		{
			switch(key)
			{
				case 27:
				{
					rw_destroy_everything();
					rw_state = RW_STATE_GAME_OVER;
					rw_intro_planet_z = rw_camera_z;
					break;
				}
				case '7':
				{
					al_play_sample(rw_sample[RW_SAMPLE_SHIELD], 0.5, 0.25, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
					rw_shield_generator.shield[0].life = RW_SHIELD_MAX_LIFE;
					rw_shield_generator.shield[0].active = true;
					break;
				}
				case '8':
				{
					al_play_sample(rw_sample[RW_SAMPLE_SHIELD], 0.5, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
					rw_shield_generator.shield[1].life = RW_SHIELD_MAX_LIFE;
					rw_shield_generator.shield[1].active = true;
					break;
				}
				case '9':
				{
					al_play_sample(rw_sample[RW_SAMPLE_SHIELD], 0.5, 0.75, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
					rw_shield_generator.shield[2].life = RW_SHIELD_MAX_LIFE;
					rw_shield_generator.shield[2].active = true;
					break;
				}
				case '6':
				{
					al_play_sample(rw_sample[RW_SAMPLE_SHIELD], 0.5, 0.85, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
					rw_shield_generator.shield[3].life = RW_SHIELD_MAX_LIFE;
					rw_shield_generator.shield[3].active = true;
					break;
				}
				case '3':
				{
					al_play_sample(rw_sample[RW_SAMPLE_SHIELD], 0.5, 0.75, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
					rw_shield_generator.shield[4].life = RW_SHIELD_MAX_LIFE;
					rw_shield_generator.shield[4].active = true;
					break;
				}
				case '2':
				{
					al_play_sample(rw_sample[RW_SAMPLE_SHIELD], 0.5, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
					rw_shield_generator.shield[5].life = RW_SHIELD_MAX_LIFE;
					rw_shield_generator.shield[5].active = true;
					break;
				}
				case '1':
				{
					al_play_sample(rw_sample[RW_SAMPLE_SHIELD], 0.5, 0.25, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
					rw_shield_generator.shield[6].life = RW_SHIELD_MAX_LIFE;
					rw_shield_generator.shield[6].active = true;
					break;
				}
				case '4':
				{
					al_play_sample(rw_sample[RW_SAMPLE_SHIELD], 0.5, 0.15, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
					rw_shield_generator.shield[7].life = RW_SHIELD_MAX_LIFE;
					rw_shield_generator.shield[7].active = true;
					break;
				}
			}
			/* camera logic */
			if(rw_camera_z > rw_level_camera_z[rw_level])
			{
				rw_camera_z -= 0.5;
			}
			
			for(i = 0; i < RW_MAX_PARTICLES; i++)
			{
				if(rw_particle[i].active)
				{
					rw_particle[i].x += rw_particle[i].vx;
					rw_particle[i].y += rw_particle[i].vy;
					rw_particle[i].life--;
					if(rw_particle[i].life <= 0)
					{
						rw_particle[i].active = false;
					}
				}
			}
			
			for(i = 0; i < RW_MAX_SHIELDS; i++)
			{
				if(rw_shield_generator.shield[i].life > 0)
				{
					rw_shield_generator.shield[i].life--;
					if(rw_shield_generator.shield[i].life <= 0)
					{
						rw_shield_generator.shield[i].active = false;
					}
				}
			}
			switch(rw_damage)
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
			if(rw_damage_time > dt)
			{
				rw_damage_time--;
			}
			if(rw_damage >= 4)
			{
				rw_alert_ticks++;
				if(rw_alert_ticks > 20)
				{
					rw_alert_ticks = 0;
				}
				if(rw_alert_audio_ticks == 0)
				{
					al_play_sample(rw_sample[RW_SAMPLE_ALERT], 0.5, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				}
				rw_alert_audio_ticks++;
				if(rw_alert_audio_ticks >= 60)
				{
					rw_alert_audio_ticks = 0;
				}
			}
			for(i = 0; i < RW_MAX_THREATS; i++)
			{
				if(rw_threat[i].active)
				{
					rw_threat[i].x += rw_threat[i].vx;
					rw_threat[i].y += rw_threat[i].vy;
					rw_threat[i].angle += rw_threat[i].vangle;
					if(rw_distance(320.0, 240.0, rw_threat[i].x, rw_threat[i].y) < 36.0)
					{
						al_play_sample(rw_sample[RW_SAMPLE_DAMAGE], 0.5, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
						if(rw_threat[i].type == 0)
						{
							min_angle = rw_eight_ways[7];
							max_angle = rw_eight_ways[1];
						}
						else if(rw_threat[i].type == 7)
						{
							min_angle = rw_eight_ways[6];
							max_angle = rw_eight_ways[0];
						}
						else
						{
							min_angle = rw_eight_ways[rw_threat[i].type - 1];
							max_angle = rw_eight_ways[rw_threat[i].type + 1];
						}
						for(j = 0; j < 8; j++)
						{
							rw_generate_particle(rw_threat[i].x, rw_threat[i].y, min_angle, max_angle);
						}
						rw_threat[i].active = 0;
						rw_damage++;
						rw_damage_time = 20;
						if(rw_damage > 4)
						{
							rw_destroy_everything();
							rw_state = RW_STATE_GAME_OVER;
							rw_intro_planet_z = rw_camera_z;
							rw_alert_ticks = 0;
						}
					}
					else if(rw_distance(320.0, 240.0, rw_threat[i].x, rw_threat[i].y) < 48.0)
					{
						if(rw_shield_generator.shield[rw_threat[i].type].active)
						{
							al_play_sample(rw_sample[RW_SAMPLE_HIT], 0.5, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
							if(rw_threat[i].type == 0)
							{
								min_angle = rw_eight_ways[7];
								max_angle = rw_eight_ways[1];
							}
							else if(rw_threat[i].type == 7)
							{
								min_angle = rw_eight_ways[6];
								max_angle = rw_eight_ways[0];
							}
							else
							{
								min_angle = rw_eight_ways[rw_threat[i].type - 1];
								max_angle = rw_eight_ways[rw_threat[i].type + 1];
							}
							for(j = 0; j < 5; j++)
							{
								rw_generate_particle(rw_threat[i].x, rw_threat[i].y, min_angle, max_angle);
							}
							rw_threat[i].active = 0;
							rw_shield_generator.shield[rw_threat[i].type].active = 0;
							rw_shield_generator.shield[rw_threat[i].type].life = RW_SHIELD_MAX_LIFE;
						}
					}
				}
			}
			
			for(i = 0; i < RW_MAX_SHOTS; i++)
			{
				if(rw_shot[i].active)
				{
					rw_shot[i].x += rw_shot[i].vx;
					rw_shot[i].y += rw_shot[i].vy;
					if(rw_distance(320.0, 240.0, rw_shot[i].x, rw_shot[i].y) < 36.0)
					{
						al_play_sample(rw_sample[RW_SAMPLE_HIT], 0.5, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
						if(rw_shot[i].type == 0)
						{
							min_angle = rw_eight_ways[7];
							max_angle = rw_eight_ways[1];
						}
						else if(rw_shot[i].type == 7)
						{
							min_angle = rw_eight_ways[6];
							max_angle = rw_eight_ways[0];
						}
						else
						{
							min_angle = rw_eight_ways[rw_shot[i].type - 1];
							max_angle = rw_eight_ways[rw_shot[i].type + 1];
						}
						for(j = 0; j < 5; j++)
						{
							rw_generate_particle(rw_shot[i].x, rw_shot[i].y, min_angle, max_angle);
						}
						rw_shot[i].active = 0;
						rw_damage++;
						rw_damage_time = 20;
						if(rw_damage > 4)
						{
							rw_state = RW_STATE_GAME_OVER;
						}
					}
					else if(rw_distance(320.0, 240.0, rw_shot[i].x, rw_shot[i].y) < 48.0)
					{
						if(rw_shield_generator.shield[rw_shot[i].type].active)
						{
							al_play_sample(rw_sample[RW_SAMPLE_HIT], 0.5, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
							if(rw_shot[i].type == 0)
							{
								min_angle = rw_eight_ways[7];
								max_angle = rw_eight_ways[1];
							}
							else if(rw_shot[i].type == 7)
							{
								min_angle = rw_eight_ways[6];
								max_angle = rw_eight_ways[0];
							}
							else
							{
								min_angle = rw_eight_ways[rw_shot[i].type - 1];
								max_angle = rw_eight_ways[rw_shot[i].type + 1];
							}
							for(j = 0; j < 3; j++)
							{
								rw_generate_particle(rw_shot[i].x, rw_shot[i].y, min_angle, max_angle);
							}
							rw_shot[i].active = 0;
							rw_shield_generator.shield[rw_shot[i].type].active = 0;
							rw_shield_generator.shield[rw_shot[i].type].life = RW_SHIELD_MAX_LIFE;
		//					rw_score += 25;
						}
					}
				}
			}
			
			for(i = 0; i < RW_MAX_SHIPS; i++)
			{
				if(rw_ship[i].active)
				{
					rw_ship[i].angle += rw_ship[i].vangle;
					if(rw_ship[i].vangle > 0.0)
					{
						if(rw_ship[i].way == 0)
						{
							if(rw_ship[i].angle >= rw_eight_ways[rw_ship[i].way] + ALLEGRO_PI * 2.0)
							{
								rw_ship[i].angle = rw_eight_ways[rw_ship[i].way];
								rw_ship[i].vangle = 0.0;
							}
						}
						else
						{
							if(rw_ship[i].angle >= rw_eight_ways[rw_ship[i].way])
							{
								rw_ship[i].angle = rw_eight_ways[rw_ship[i].way];
								rw_ship[i].vangle = 0.0;
							}
						}
					}
					else if(rw_ship[i].vangle < 0.0)
					{
						if(rw_ship[i].way == 7)
						{
							if(rw_ship[i].angle <= rw_eight_ways[rw_ship[i].way] - ALLEGRO_PI * 2.0)
							{
								rw_ship[i].angle = rw_eight_ways[rw_ship[i].way];
								rw_ship[i].vangle = 0.0;
							}
						}
						else
						{
							if(rw_ship[i].angle <= rw_eight_ways[rw_ship[i].way])
							{
								rw_ship[i].angle = rw_eight_ways[rw_ship[i].way];
								rw_ship[i].vangle = 0.0;
							}
						}
					}
					rw_ship[i].dist += rw_ship[i].vdist;
					if(rw_ship[i].dist <= rw_ship[i].dest)
					{
						rw_ship[i].vdist = 0.0;
					}
					rw_ship[i].x = 320.0 + cos(rw_ship[i].angle) * rw_ship[i].dist;
					rw_ship[i].y = 240.0 + sin(rw_ship[i].angle) * rw_ship[i].dist;
					if(rw_ship[i].vangle == 0.0 && rw_ship[i].vdist == 0.0)
					{
						rw_ship[i].ticks++;
						if(rw_ship[i].ticks == 15)
						{
							r = t3f_rand(&rw_rng_state) % 100;
							if(r < 75)
							{
								for(j = 0; j < RW_MAX_SHOTS; j++)
								{
									if(!rw_shot[j].active)
									{
										rw_shot[j].x = 320.0 + cos(rw_ship[i].angle) * rw_ship[i].dist;
										rw_shot[j].y = 240.0 + sin(rw_ship[i].angle) * rw_ship[i].dist;
										rw_shot[j].vx = cos(rw_ship[i].angle) * -3.0;
										rw_shot[j].vy = sin(rw_ship[i].angle) * -3.0;
										rw_shot[j].type = rw_ship[i].way;
										rw_shot[j].active = true;
										break;
									}
								}
							}
						}
						if(rw_ship[i].ticks > 30)
						{
							r = t3f_rand(&rw_rng_state) % 100;
							if(r < 75)
							{
								rw_ship[i].way++;
								if(rw_ship[i].way > 7)
								{
									rw_ship[i].way = 0;
								}
								rw_ship[i].vangle = 0.005;
							}
							else
							{
								rw_ship[i].way--;
								if(rw_ship[i].way < 0)
								{
									rw_ship[i].way = 7;
								}
								rw_ship[i].vangle = -0.005;
							}
							rw_ship[i].ticks = 0;
						}
					}
				}
			}
			
			rw_ticks++;
			rw_score++;
			if(rw_score > rw_high_score)
			{
				rw_high_score = rw_score;
				if(!rw_new_high_score)
				{
					al_play_sample(rw_sample[RW_SAMPLE_HIGH_SCORE], 0.5, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
					rw_new_high_score = true;
				}
			}
			
			/* generate enemies */
			if(rw_ticks > rw_level_threat_ticks[rw_level] && t3f_rand(&rw_rng_state) % rw_level_prob_total[rw_level] < rw_level_prob[rw_level])
			{
				rw_generate_threat();
				if(rw_threat_count > 15 && rw_level < 19)
				{
					rw_threat_count = 0;
					al_play_sample(rw_sample[RW_SAMPLE_LEVEL_UP], 0.5, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
					rw_level++;
					if(rw_level == 9)
					{
						rw_ship[0].way = t3f_rand(&rw_rng_state) % 8;
						rw_ship[0].angle = rw_eight_ways[rw_ship[0].way];
						rw_ship[0].vangle = 0.0;
						rw_ship[0].dist = 320.0;
						rw_ship[0].dest = 200.0;
						rw_ship[0].vdist = -2.5;
						rw_ship[0].ticks = 0;
						rw_ship[0].active = true;
					}
					if(rw_level == 14)
					{
						rw_ship[1].way = t3f_rand(&rw_rng_state) % 8;
						rw_ship[1].angle = rw_eight_ways[rw_ship[1].way];
						rw_ship[1].vangle = 0.0;
						rw_ship[1].dist = 320.0;
						rw_ship[1].dest = 160.0;
						rw_ship[1].vdist = -2.5;
						rw_ship[1].ticks = 0;
						rw_ship[1].active = true;
					}
				}
				rw_ticks = 0;
			}
			if(t3f_key[ALLEGRO_KEY_ESCAPE] || t3f_key[ALLEGRO_KEY_BACK])
			{
				rw_state = RW_STATE_GAME_OVER;
			}
			break;
		}
	}
	if(rw_quit)
	{
		t3f_exit();
	}
}

static void rw_render_hover_text(ALLEGRO_FONT * fp, float x, float y, int flags, bool highlight, const char * text)
{
	al_draw_text(fp, highlight ? al_map_rgba_f(1.0, 1.0, 1.0, 1.0) : al_map_rgba_f(0.25, 0.25, 0.25, 0.25), x, y, flags, text);
}

void rw_render(void)
{
	int i;
	ALLEGRO_COLOR color;
	float alpha, beta;
	float mouse_x = 320.0, mouse_y = 240.0;
	
	if((t3f_flags & T3F_USE_MOUSE) && !(t3f_flags & T3F_USE_TOUCH))
	{
		mouse_x = t3f_mouse_x;
		mouse_y = t3f_mouse_y;
	}
	
	al_hold_bitmap_drawing(true);
	switch(rw_state)
	{
		case RW_STATE_INTRO:
		{
			al_clear_to_color(al_map_rgb(0, 0, 0));
			switch(rw_intro_state)
			{
				case 0:
				case 1:
				{
					t3f_draw_bitmap(rw_bitmap[RW_BITMAP_T3_LOGO], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 640 / 2 - al_get_bitmap_width(rw_bitmap[RW_BITMAP_T3_LOGO]) / 2, 480 / 2 - al_get_bitmap_height(rw_bitmap[RW_BITMAP_T3_LOGO]) / 2, rw_intro_z, 0);
					break;
				}
				case 2:
				{
					alpha = (float)rw_intro_ticker / 30.0;
					t3f_draw_bitmap(rw_bitmap[RW_BITMAP_T3_LOGO], al_map_rgba_f(alpha, alpha, alpha, alpha), 640 / 2 - al_get_bitmap_width(rw_bitmap[RW_BITMAP_T3_LOGO]) / 2, 480 / 2 - al_get_bitmap_height(rw_bitmap[RW_BITMAP_T3_LOGO]) / 2, rw_intro_z, 0);
					break;
				}
				case 3:
				{
					alpha = -(rw_intro_planet_z - 40.0) / (400.0 + 40.0);
					t3f_draw_rotated_bitmap(rw_bitmap[RW_BITMAP_WORLD], al_map_rgba_f(alpha, alpha, alpha, alpha), al_get_bitmap_width(rw_bitmap[RW_BITMAP_WORLD]) / 2, al_get_bitmap_height(rw_bitmap[RW_BITMAP_WORLD]) / 2, 640 / 2, 480 / 2, rw_intro_planet_z, rw_intro_planet_angle, 0);
					break;
				}
			}
			break;
		}
		case RW_STATE_TITLE:
		{
			al_clear_to_color(al_map_rgb(0, 0, 0));
			al_draw_textf(rw_font, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 320 - al_get_text_width(rw_font, "HIGH SCORE - 0:00:00") / 2, t3f_display_top, 0, "HIGH SCORE - %d:%02d:%02d", rw_high_score / 3600, (rw_high_score / 60) % 60, (int)(((float)(rw_high_score % 60) / 60.0) * 100.0) % 100);
			t3f_draw_rotated_bitmap(rw_bitmap[RW_BITMAP_WORLD], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), al_get_bitmap_width(rw_bitmap[RW_BITMAP_WORLD]) / 2, al_get_bitmap_height(rw_bitmap[RW_BITMAP_WORLD]) / 2, 640 / 2, 480 / 2, rw_intro_planet_z, rw_intro_planet_angle, 0);
			alpha = -rw_logo_z / 10.0;
			t3f_draw_bitmap(rw_bitmap[RW_BITMAP_LOGO], al_map_rgba_f(alpha, alpha, alpha, alpha), 320.0 - (float)al_get_bitmap_width(rw_bitmap[RW_BITMAP_LOGO]) / 2, 240 - al_get_bitmap_height(rw_bitmap[RW_BITMAP_LOGO]) - 128, rw_logo_z + 10.0, 0);
			alpha = 1.0 + rw_logo_z / 10.0;
			t3f_draw_bitmap(rw_bitmap[RW_BITMAP_LOGO], al_map_rgba_f(alpha, alpha, alpha, alpha), 320.0 - (float)al_get_bitmap_width(rw_bitmap[RW_BITMAP_LOGO]) / 2, 240 - al_get_bitmap_height(rw_bitmap[RW_BITMAP_LOGO]) - 128, rw_logo_z, 0);
			break;
		}
		case RW_STATE_PRIVACY:
		{
			al_clear_to_color(al_map_rgb(0, 0, 0));
			alpha = -rw_logo_z / 10.0;
			t3f_draw_bitmap(rw_bitmap[RW_BITMAP_LOGO], al_map_rgba_f(alpha, alpha, alpha, alpha), 320.0 - (float)al_get_bitmap_width(rw_bitmap[RW_BITMAP_LOGO]) / 2, 240 - al_get_bitmap_height(rw_bitmap[RW_BITMAP_LOGO]) - 128, rw_logo_z + 10.0, 0);
			alpha = 1.0 + rw_logo_z / 10.0;
			t3f_draw_bitmap(rw_bitmap[RW_BITMAP_LOGO], al_map_rgba_f(alpha, alpha, alpha, alpha), 320.0 - (float)al_get_bitmap_width(rw_bitmap[RW_BITMAP_LOGO]) / 2, 240 - al_get_bitmap_height(rw_bitmap[RW_BITMAP_LOGO]) - 128, rw_logo_z, 0);
			al_draw_text(rw_font, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 160, 240 - al_get_font_line_height(rw_font) * 2.0, 0, "THIS GAME ONLY STORES SETTINGS");
			al_draw_text(rw_font, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 160, 240 - al_get_font_line_height(rw_font) * 1.0, 0, "AND SCORES LOCALLY. IT WILL NOT");
			al_draw_text(rw_font, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 160, 240 + al_get_font_line_height(rw_font) * 0.0, 0, "SHARE ANY INFORMATION OVER");
			al_draw_text(rw_font, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 160, 240 + al_get_font_line_height(rw_font) * 1.0, 0, "THE INTERNET.");
			break;
		}
		case RW_STATE_EXIT:
		{
			al_clear_to_color(al_map_rgb(0, 0, 0));
			alpha = -rw_intro_planet_z / 400.0;
			t3f_draw_rotated_bitmap(rw_bitmap[RW_BITMAP_WORLD], al_map_rgba_f(alpha, alpha, alpha, alpha), al_get_bitmap_width(rw_bitmap[RW_BITMAP_WORLD]) / 2, al_get_bitmap_height(rw_bitmap[RW_BITMAP_WORLD]) / 2, 640 / 2, 480 / 2, rw_intro_planet_z, rw_intro_planet_angle, 0);
			break;
		}
		case RW_STATE_GAME_IN:
		case RW_STATE_GAME_OUT:
		{
			al_clear_to_color(al_map_rgb(0, 0, 0));
			al_draw_textf(rw_font, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 320 - al_get_text_width(rw_font, "HIGH SCORE - 0:00:00") / 2, t3f_display_top, 0, "HIGH SCORE - %d:%02d:%02d", rw_high_score / 3600, (rw_high_score / 60) % 60, (int)(((float)(rw_high_score % 60) / 60.0) * 100.0) % 100);
			t3f_draw_rotated_bitmap(rw_bitmap[RW_BITMAP_WORLD], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), al_get_bitmap_width(rw_bitmap[RW_BITMAP_WORLD]) / 2, al_get_bitmap_height(rw_bitmap[RW_BITMAP_WORLD]) / 2, 640 / 2, 480 / 2, rw_intro_planet_z, rw_intro_planet_angle, 0);
			break;
		}
		case RW_STATE_GAME:
		case RW_STATE_GAME_OVER:
		{
			al_clear_to_color(al_map_rgb(0, 0, 0));
			if(rw_damage >= 4)
			{
				alpha = 1.0 - (float)rw_alert_ticks / 80.0;
				color = al_map_rgba_f(alpha, (1.0 - (float)rw_damage_time / 20.0) * alpha, (1.0 - (float)rw_damage_time / 20.0) * alpha, alpha);
			}
			else
			{
				color = al_map_rgba_f(1.0, 1.0 - (float)rw_damage_time / 20.0, 1.0 - (float)rw_damage_time / 20.0, 1.0);
			}
			t3f_draw_bitmap(rw_bitmap[RW_BITMAP_WORLD], color, 640 / 2 - al_get_bitmap_width(rw_bitmap[RW_BITMAP_WORLD]) / 2, 480 / 2 - al_get_bitmap_height(rw_bitmap[RW_BITMAP_WORLD]) / 2, rw_camera_z, 0);
			for(i = 0; i < 8; i++)
			{
				if(rw_shield_generator.shield[i].life > 0)
				{
					alpha = (float)rw_shield_generator.shield[i].life / (float)(RW_SHIELD_MAX_LIFE);
					beta = rw_shield_generator.shield[i].active ? 1.0 : 0.0;
					t3f_draw_bitmap(rw_bitmap[RW_BITMAP_SHIELD_0 + i], al_map_rgba_f(alpha, beta * alpha, beta * alpha, alpha), 640 / 2 - al_get_bitmap_width(rw_bitmap[RW_BITMAP_SHIELD_0 + i]) / 2, 480 / 2 - al_get_bitmap_height(rw_bitmap[RW_BITMAP_SHIELD_0 + i]) / 2, rw_camera_z, 0);
				}
			}
			for(i = 0; i < RW_MAX_PARTICLES; i++)
			{
				if(rw_particle[i].active)
				{
					alpha = (float)rw_particle[i].life / (float)rw_particle[i].tlife;
					t3f_draw_bitmap(rw_bitmap[RW_BITMAP_PARTICLE], al_map_rgba_f(alpha, alpha, alpha, alpha), rw_particle[i].x, rw_particle[i].y, rw_camera_z, 0);
				}
			}
			for(i = 0; i < RW_MAX_THREATS; i++)
			{
				if(rw_threat[i].active)
				{
					t3f_draw_rotated_bitmap(rw_bitmap[RW_BITMAP_THREAT], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), al_get_bitmap_width(rw_bitmap[RW_BITMAP_THREAT]) / 2, al_get_bitmap_height(rw_bitmap[RW_BITMAP_THREAT]) / 2, rw_threat[i].x, rw_threat[i].y, rw_camera_z, rw_threat[i].angle, 0);
				}
			}
			for(i = 0; i < RW_MAX_SHOTS; i++)
			{
				if(rw_shot[i].active)
				{
					t3f_draw_bitmap(rw_bitmap[RW_BITMAP_SHOT], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), rw_shot[i].x - (float)al_get_bitmap_width(rw_bitmap[RW_BITMAP_SHOT]) / 2, rw_shot[i].y - (float)al_get_bitmap_height(rw_bitmap[RW_BITMAP_SHOT]) / 2, rw_camera_z, 0);
				}
			}
			for(i = 0; i < RW_MAX_SHIPS; i++)
			{
				if(rw_ship[i].active)
				{
					t3f_draw_rotated_bitmap(rw_bitmap[RW_BITMAP_SHIP], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), al_get_bitmap_width(rw_bitmap[RW_BITMAP_SHIP]) / 2, al_get_bitmap_height(rw_bitmap[RW_BITMAP_SHIP]) / 2, rw_ship[i].x, rw_ship[i].y, rw_camera_z, -rw_ship[i].angle, 0);
				}
			}
			al_draw_textf(rw_font, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 320 - al_get_text_width(rw_font, "HIGH SCORE - 0:00:00") / 2, t3f_display_top, 0, "HIGH SCORE - %d:%02d:%02d", rw_high_score / 3600, (rw_high_score / 60) % 60, (int)(((float)(rw_high_score % 60) / 60.0) * 100.0) % 100);
			al_draw_textf(rw_font, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 320 - al_get_text_width(rw_font, "SCORE - 0:00:00") / 2, t3f_display_top + 16, 0, "SCORE - %d:%02d:%02d", rw_score / 3600, (rw_score / 60) % 60, (int)(((float)(rw_score % 60) / 60.0) * 100.0) % 100);
			al_draw_textf(rw_font, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 320, t3f_display_top + 32, ALLEGRO_ALIGN_CENTRE, "LEVEL - %d", rw_level + 1);
		//	al_font_textprintf_centre(rw_font, 320, 32, "%d : %f : %f : %f", rw_ship[0].way, rw_eight_ways[rw_ship[0].way], rw_ship[0].angle, rw_ship[0].vangle);
			break;
		}
	}
	
	/* render guides */
	if(rw_state != RW_STATE_INTRO && rw_state != RW_STATE_EXIT && rw_state != RW_STATE_PRIVACY)
	{
		if(rw_state == RW_STATE_GAME)
		{
			al_draw_tinted_bitmap(rw_bitmap[RW_BITMAP_GUIDE], al_map_rgba_f(0.25, 0.25, 0.25, 0.25), 160.0 - 4.0, t3f_display_top + rw_third - 4.0, 0);
			al_draw_tinted_bitmap(rw_bitmap[RW_BITMAP_GUIDE], al_map_rgba_f(0.25, 0.25, 0.25, 0.25), 640.0 - 160.0 - 4.0, t3f_display_top + rw_third - 4.0, 0);
		}
		al_draw_tinted_bitmap(rw_bitmap[RW_BITMAP_GUIDE], al_map_rgba_f(0.25, 0.25, 0.25, 0.25), 160.0 - 4.0, t3f_display_top + rw_third * 2.0 - 4.0, 0);
		al_draw_tinted_bitmap(rw_bitmap[RW_BITMAP_GUIDE], al_map_rgba_f(0.25, 0.25, 0.25, 0.25), 640.0 - 160.0 - 4.0, t3f_display_top + rw_third * 2.0 - 4.0, 0);
		rw_render_hover_text(rw_font, 160.0 / 2.0, t3f_display_top + rw_third * 2.0 + rw_third / 2.0 - al_get_font_line_height(rw_font), ALLEGRO_ALIGN_CENTRE, mouse_x <= 160 && mouse_y >= t3f_display_top + rw_third * 2.0, "1");
		rw_render_hover_text(rw_font, 320, t3f_display_top + rw_third * 2.0 + rw_third / 2.0 - al_get_font_line_height(rw_font), ALLEGRO_ALIGN_CENTRE, mouse_x > 160 && mouse_x < 640 - 160 && mouse_y >= t3f_display_top + rw_third * 2.0, "2");
		rw_render_hover_text(rw_font, 640.0 - 160.0 / 2.0, t3f_display_top + rw_third * 2.0 + rw_third / 2.0 - al_get_font_line_height(rw_font), ALLEGRO_ALIGN_CENTRE, mouse_x >= 640 - 160 && mouse_y >= t3f_display_top + rw_third * 2.0, "3");
		if(rw_state == RW_STATE_TITLE)
		{
			rw_render_hover_text(rw_font, 160.0 / 2.0, t3f_display_top + rw_third * 2.0 + rw_third / 2.0, ALLEGRO_ALIGN_CENTRE, mouse_x <= 160 && mouse_y >= t3f_display_top + rw_third * 2.0, "PRIVACY INFO");
			rw_render_hover_text(rw_font, 320, t3f_display_top + rw_third * 2.0 + rw_third / 2.0, ALLEGRO_ALIGN_CENTRE, mouse_x > 160 && mouse_x < 640 - 160 && mouse_y >= t3f_display_top + rw_third * 2.0, "START GAME");
			rw_render_hover_text(rw_font, 640.0 - 160.0 / 2.0, t3f_display_top + rw_third * 2.0 + rw_third / 2.0, ALLEGRO_ALIGN_CENTRE, mouse_x >= 640 - 160 && mouse_y >= t3f_display_top + rw_third * 2.0, "TOGGLE MUSIC");
		}
		if(rw_state == RW_STATE_GAME)
		{
			rw_render_hover_text(rw_font, 160.0 / 2.0, t3f_display_top + rw_third / 2.0 - al_get_font_line_height(rw_font), ALLEGRO_ALIGN_CENTRE, mouse_x <= 160 && mouse_y < t3f_display_top + rw_third, "7");
			rw_render_hover_text(rw_font, 160.0 / 2.0, t3f_display_top + rw_third + rw_third / 2.0 - al_get_font_line_height(rw_font), ALLEGRO_ALIGN_CENTRE, mouse_x <= 160 && mouse_y > t3f_display_top + rw_third && mouse_y < t3f_display_top + rw_third * 2.0, "4");
			rw_render_hover_text(rw_font, 640.0 - 160.0 / 2.0, t3f_display_top + rw_third / 2.0 - al_get_font_line_height(rw_font), ALLEGRO_ALIGN_CENTRE, mouse_x >= 640 - 160 && mouse_y < t3f_display_top + rw_third, "9");
			rw_render_hover_text(rw_font, 640.0 - 160.0 / 2.0, t3f_display_top + rw_third + rw_third / 2.0 - al_get_font_line_height(rw_font), ALLEGRO_ALIGN_CENTRE, mouse_x >= 640 - 160 && mouse_y > t3f_display_top + rw_third && mouse_y < t3f_display_top + rw_third * 2.0, "6");
			rw_render_hover_text(rw_font, 320.0, t3f_display_top + rw_third / 2.0 - al_get_font_line_height(rw_font), ALLEGRO_ALIGN_CENTRE, mouse_x > 160 && mouse_x < 640 - 160 && mouse_y < t3f_display_top + rw_third, "8");
		}
	}
	al_hold_bitmap_drawing(false);
}

void rw_atlas_bitmap(ALLEGRO_BITMAP ** bitmap)
{
	ALLEGRO_BITMAP * bp;
	bp = t3f_add_bitmap_to_atlas(rw_atlas, bitmap, T3F_ATLAS_SPRITE);

	if(bp)
	{
		al_destroy_bitmap(*bitmap);
		*bitmap = bp;
	}
	else
	{
		printf("could not atlas bitmap\n");
	}
}

int rw_initialize(int argc, char * argv[])
{
	int i;
	
	if(!t3f_initialize("Eight Ways to Die", 640, 480, 60.0, rw_logic, rw_render, T3F_USE_KEYBOARD | T3F_USE_SOUND | T3F_USE_MOUSE | T3F_USE_TOUCH | T3F_FILL_SCREEN | T3F_FORCE_ASPECT | T3F_RESIZABLE))
	{
		return 0;
	}
	if(!t3f_locate_resource("data/world.png"))
	{
		printf("Cannot locate resources!\n");
		return 0;
	}
	
	t3f_set_event_handler(rw_event_handler);
	
	rw_bitmap[RW_BITMAP_WORLD] = t3f_load_resource((void **)(&rw_bitmap[RW_BITMAP_WORLD]), T3F_RESOURCE_TYPE_BITMAP, "data/world.png", 0, 0, 0);
	rw_bitmap[RW_BITMAP_SHIELD_0] = t3f_load_resource((void **)(&rw_bitmap[RW_BITMAP_SHIELD_0]), T3F_RESOURCE_TYPE_BITMAP, "data/shield0.png", 0, 0, 0);
	rw_bitmap[RW_BITMAP_SHIELD_1] = t3f_load_resource((void **)(&rw_bitmap[RW_BITMAP_SHIELD_1]), T3F_RESOURCE_TYPE_BITMAP, "data/shield1.png", 0, 0, 0);
	rw_bitmap[RW_BITMAP_SHIELD_2] = t3f_load_resource((void **)(&rw_bitmap[RW_BITMAP_SHIELD_2]), T3F_RESOURCE_TYPE_BITMAP, "data/shield2.png", 0, 0, 0);
	rw_bitmap[RW_BITMAP_SHIELD_3] = t3f_load_resource((void **)(&rw_bitmap[RW_BITMAP_SHIELD_3]), T3F_RESOURCE_TYPE_BITMAP, "data/shield3.png", 0, 0, 0);
	rw_bitmap[RW_BITMAP_SHIELD_4] = t3f_load_resource((void **)(&rw_bitmap[RW_BITMAP_SHIELD_4]), T3F_RESOURCE_TYPE_BITMAP, "data/shield4.png", 0, 0, 0);
	rw_bitmap[RW_BITMAP_SHIELD_5] = t3f_load_resource((void **)(&rw_bitmap[RW_BITMAP_SHIELD_5]), T3F_RESOURCE_TYPE_BITMAP, "data/shield5.png", 0, 0, 0);
	rw_bitmap[RW_BITMAP_SHIELD_6] = t3f_load_resource((void **)(&rw_bitmap[RW_BITMAP_SHIELD_6]), T3F_RESOURCE_TYPE_BITMAP, "data/shield6.png", 0, 0, 0);
	rw_bitmap[RW_BITMAP_SHIELD_7] = t3f_load_resource((void **)(&rw_bitmap[RW_BITMAP_SHIELD_7]), T3F_RESOURCE_TYPE_BITMAP, "data/shield7.png", 0, 0, 0);
	rw_bitmap[RW_BITMAP_THREAT] = t3f_load_resource((void **)(&rw_bitmap[RW_BITMAP_THREAT]), T3F_RESOURCE_TYPE_BITMAP, "data/threat.png", 0, 0, 0);
	rw_bitmap[RW_BITMAP_SHIP] = t3f_load_resource((void **)(&rw_bitmap[RW_BITMAP_SHIP]), T3F_RESOURCE_TYPE_BITMAP, "data/ship.png", 0, 0, 0);
	rw_bitmap[RW_BITMAP_SHOT] = t3f_load_resource((void **)(&rw_bitmap[RW_BITMAP_SHOT]), T3F_RESOURCE_TYPE_BITMAP, "data/shot.png", 0, 0, 0);
	rw_bitmap[RW_BITMAP_PARTICLE] = t3f_load_resource((void **)(&rw_bitmap[RW_BITMAP_PARTICLE]), T3F_RESOURCE_TYPE_BITMAP, "data/particle.png", 0, 0, 0);
	rw_bitmap[RW_BITMAP_LOGO] = t3f_load_resource((void **)(&rw_bitmap[RW_BITMAP_LOGO]), T3F_RESOURCE_TYPE_BITMAP, "data/logo.png", 0, 0, 0);
	rw_bitmap[RW_BITMAP_T3_LOGO] = t3f_load_resource((void **)(&rw_bitmap[RW_BITMAP_T3_LOGO]), T3F_RESOURCE_TYPE_BITMAP, "data/t3.png", 0, 0, 0);
	rw_bitmap[RW_BITMAP_ICON] = t3f_load_resource((void **)(&rw_bitmap[RW_BITMAP_ICON]), T3F_RESOURCE_TYPE_BITMAP, "data/icon.png", 0, 0, 0);
	rw_bitmap[RW_BITMAP_GUIDE] = t3f_load_resource((void **)(&rw_bitmap[RW_BITMAP_GUIDE]), T3F_RESOURCE_TYPE_BITMAP, "data/guide.png", 0, 0, 0);
	for(i = RW_BITMAP_WORLD; i <= RW_BITMAP_GUIDE; i++)
	{
		if(!rw_bitmap[i])
		{
			printf("Error loading image %d!\n", i);
			return 0;
		}
	}
	rw_atlas = t3f_create_atlas(1024, 1024);
	if(rw_atlas)
	{
		rw_atlas_bitmap(&rw_bitmap[RW_BITMAP_WORLD]);
		rw_atlas_bitmap(&rw_bitmap[RW_BITMAP_SHIELD_0]);
		rw_atlas_bitmap(&rw_bitmap[RW_BITMAP_SHIELD_1]);
		rw_atlas_bitmap(&rw_bitmap[RW_BITMAP_SHIELD_2]);
		rw_atlas_bitmap(&rw_bitmap[RW_BITMAP_SHIELD_3]);
		rw_atlas_bitmap(&rw_bitmap[RW_BITMAP_SHIELD_4]);
		rw_atlas_bitmap(&rw_bitmap[RW_BITMAP_SHIELD_5]);
		rw_atlas_bitmap(&rw_bitmap[RW_BITMAP_SHIELD_6]);
		rw_atlas_bitmap(&rw_bitmap[RW_BITMAP_SHIELD_7]);
		rw_atlas_bitmap(&rw_bitmap[RW_BITMAP_THREAT]);
		rw_atlas_bitmap(&rw_bitmap[RW_BITMAP_SHIP]);
		rw_atlas_bitmap(&rw_bitmap[RW_BITMAP_SHOT]);
		rw_atlas_bitmap(&rw_bitmap[RW_BITMAP_PARTICLE]);
		rw_atlas_bitmap(&rw_bitmap[RW_BITMAP_GUIDE]);
	}
	
	#ifndef ALLEGRO_MACOSX
		#ifndef T3F_ANDROID
			al_set_display_icon(t3f_display, rw_bitmap[RW_BITMAP_ICON]);
		#endif
	#endif
	
	rw_font = ocd_load_font("data/gamefont.pcx");
	if(!rw_font)
	{
		printf("Error loading font!\n");
		return 0;
	}
	
	rw_sample[RW_SAMPLE_DAMAGE] = al_load_sample("data/damage.ogg");
	rw_sample[RW_SAMPLE_HIT] = al_load_sample("data/hit.ogg");
	rw_sample[RW_SAMPLE_SHIELD] = al_load_sample("data/shield.ogg");
	rw_sample[RW_SAMPLE_ALERT] = al_load_sample("data/alert.ogg");
	rw_sample[RW_SAMPLE_LEVEL_UP] = al_load_sample("data/levelup.ogg");
	rw_sample[RW_SAMPLE_HIGH_SCORE] = al_load_sample("data/highscore.ogg");
	rw_sample[RW_SAMPLE_LOGO] = al_load_sample("data/logo.ogg");
	rw_sample[RW_SAMPLE_GAME_START] = al_load_sample("data/gamestart.ogg");
	rw_sample[RW_SAMPLE_LOGO_OUT] = al_load_sample("data/logoout.ogg");
	for(i = RW_SAMPLE_SHIELD; i <= RW_SAMPLE_LOGO_OUT; i++)
	{
		if(!rw_sample[i])
		{
			printf("Error loading sound %d!\n", i);
			return 0;
		}
	}

	rw_load_high_score(t3f_get_filename(t3f_data_path, "rw.hs"));
	
	rw_vertical_scale = (t3f_display_bottom - t3f_display_top) / 480.0;
	rw_third = (t3f_display_bottom - t3f_display_top) / 3.0;
	t3f_srand(&rw_rng_state, time(0));
	return 1;
}

int rw_exit(void)
{
	int i;
	
	al_destroy_display(t3f_display);
	rw_save_high_score(t3f_get_filename(t3f_data_path, "rw.hs"));
	t3f_stop_music();
	for(i = 0; i < RW_MAX_BITMAPS; i++)
	{
		if(rw_bitmap[i])
		{
			t3f_destroy_resource(rw_bitmap[i]);
		}
	}
	for(i = 0; i < RW_MAX_SAMPLES; i++)
	{
		if(rw_sample[i])
		{
			al_destroy_sample(rw_sample[i]);
		}
	}
	al_destroy_font(rw_font);
	return 1;
}

int main(int argc, char * argv[])
{
	if(rw_initialize(argc, argv))
	{
		if(rw_music_on)
		{
			t3f_play_music("data/title.xm");
		}
		al_play_sample(rw_sample[RW_SAMPLE_LOGO], 0.5, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
		t3f_run();
	}
	else
	{
		printf("Error: could not initialize T3F!\n");
	}
	rw_exit();
	return 0;
}
