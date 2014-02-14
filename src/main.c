#include "t3f/t3f.h"
#include "t3f/rng.h"
#include "t3f/draw.h"
#include "t3f/resource.h"

#include "ocdfont.h"
#include "shield.h"
#include "threat.h"
#include "ship.h"
#include "particle.h"

#include "control_hotspot.h"

#include "state_intro.h"
#include "state_title.h"
#include "state_privacy.h"
#include "state_game.h"

#include "init.h"
#include "exit.h"

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

void rw_event_handler(ALLEGRO_EVENT * event, void * data)
{
	RW_INSTANCE * ip = (RW_INSTANCE *)data;
	switch(event->type)
	{
		case ALLEGRO_EVENT_DISPLAY_CLOSE:
		{
			if(ip->state == RW_STATE_TITLE)
			{
				ip->state = RW_STATE_EXIT;
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
			ip->vertical_scale = (t3f_display_bottom - t3f_display_top) / 480.0;
			ip->third = (t3f_display_bottom - t3f_display_top) / 3.0;
		}
		/* pass the event through to T3F for handling by default */
		default:
		{
			t3f_event_handler(event);
			break;
		}
	}
}

void rw_logic(void * data)
{
	int i;
	int dt = 0;
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
			ip->intro_planet_z += 10.0;
			ip->intro_planet_angle += 0.01;
			if(ip->intro_planet_z >= 0.0)
			{
				ip->quit = 1;
			}
			break;
		}
		case RW_STATE_GAME_IN:
		{
			ip->intro_planet_z += 10.0;
			if(ip->intro_planet_z >= 0.0)
			{
				ip->state = RW_STATE_GAME;
			}
			ip->intro_planet_angle += ip->intro_planet_vangle;
			break;
		}
		case RW_STATE_GAME_OUT:
		{
			ip->intro_planet_z -= 10.0;
			ip->intro_planet_angle += 0.01;
			if(ip->intro_planet_z <= -400.0)
			{
				ip->intro_planet_z = -400.0;
				ip->state = RW_STATE_TITLE;
			}
			break;
		}
		case RW_STATE_GAME_OVER:
		{
			int pcount = 0;
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

void rw_render(void * data)
{
	float alpha;
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
			al_clear_to_color(al_map_rgb(0, 0, 0));
			alpha = -ip->intro_planet_z / 400.0;
			t3f_draw_rotated_bitmap(ip->bitmap[RW_BITMAP_WORLD], al_map_rgba_f(alpha, alpha, alpha, alpha), al_get_bitmap_width(ip->bitmap[RW_BITMAP_WORLD]) / 2, al_get_bitmap_height(ip->bitmap[RW_BITMAP_WORLD]) / 2, 640 / 2, 480 / 2, ip->intro_planet_z, ip->intro_planet_angle, 0);
			break;
		}
		case RW_STATE_GAME_IN:
		case RW_STATE_GAME_OUT:
		{
			al_clear_to_color(al_map_rgb(0, 0, 0));
			al_draw_textf(ip->font, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 320 - al_get_text_width(ip->font, "HIGH SCORE - 0:00:00") / 2, t3f_display_top, 0, "HIGH SCORE - %d:%02d:%02d", ip->high_score / 3600, (ip->high_score / 60) % 60, (int)(((float)(ip->high_score % 60) / 60.0) * 100.0) % 100);
			t3f_draw_rotated_bitmap(ip->bitmap[RW_BITMAP_WORLD], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), al_get_bitmap_width(ip->bitmap[RW_BITMAP_WORLD]) / 2, al_get_bitmap_height(ip->bitmap[RW_BITMAP_WORLD]) / 2, 640 / 2, 480 / 2, ip->intro_planet_z, ip->intro_planet_angle, 0);
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

int main(int argc, char * argv[])
{
	RW_INSTANCE * instance;
	
	instance = rw_create_instance();
	if(!instance)
	{
		printf("Error creating instance!\n");
		return -1;
	}
	if(rw_initialize(instance, argc, argv))
	{
		if(instance->music_on)
		{
			t3f_play_music("data/title.xm");
		}
		al_play_sample(instance->sample[RW_SAMPLE_LOGO], 0.5, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
		t3f_run();
	}
	else
	{
		printf("Error: could not initialize T3F!\n");
	}
	rw_exit(instance);
	rw_destroy_instance(instance);
	return 0;
}
