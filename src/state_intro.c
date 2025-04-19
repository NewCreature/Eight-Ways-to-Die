#include "instance.h"

#include "t3f/draw.h"

static void finish_intro(RW_INSTANCE * ip)
{
	ip->intro_planet_z = -400.0;
	ip->state = RW_STATE_TITLE;
}

void rw_state_intro_logic(RW_INSTANCE * ip)
{
	switch(ip->intro_state)
	{
		case 0:
		{
			ip->intro_z += 10.0;
			if(ip->intro_z >= 0.0)
			{
				ip->intro_z = 0.0;
				ip->intro_state = 1;
				ip->intro_ticker = 0;
			}
			break;
		}
		case 1:
		{
			ip->intro_ticker++;
			if(ip->intro_ticker >= 120)
			{
				al_play_sample(ip->sample[RW_SAMPLE_LOGO_OUT], 0.5, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				ip->intro_state = 2;
				ip->intro_ticker = 30;
			}
			break;
		}
		case 2:
		{
			ip->intro_z += 10.0;
			ip->intro_ticker--;
			if(ip->intro_ticker <= 0)
			{
				ip->intro_state = 3;
			}
			break;
		}
		case 3:
		{
			ip->intro_planet_z -= 10.0;
			if(ip->intro_planet_z <= -400.0)
			{
				finish_intro(ip);
			}
			ip->intro_planet_angle += 0.01;
			break;
		}
	}
}

void rw_state_intro_render(RW_INSTANCE * ip)
{
	float alpha;

	al_clear_to_color(al_map_rgb(0, 0, 0));
	switch(ip->intro_state)
	{
		case 0:
		case 1:
		{
			t3f_draw_bitmap(ip->bitmap[RW_BITMAP_T3_LOGO], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 640 / 2 - ip->bitmap[RW_BITMAP_T3_LOGO]->target_width / 2.0, 480 / 2 - ip->bitmap[RW_BITMAP_T3_LOGO]->target_height / 2.0, ip->intro_z, 0);
			break;
		}
		case 2:
		{
			alpha = (float)ip->intro_ticker / 30.0;
			t3f_draw_bitmap(ip->bitmap[RW_BITMAP_T3_LOGO], al_map_rgba_f(alpha, alpha, alpha, alpha), 640 / 2 - ip->bitmap[RW_BITMAP_T3_LOGO]->target_width / 2.0, 480 / 2 - ip->bitmap[RW_BITMAP_T3_LOGO]->target_height / 2.0, ip->intro_z, 0);
			break;
		}
		case 3:
		{
			alpha = -(ip->intro_planet_z - 40.0) / (400.0 + 40.0);
			t3f_draw_rotated_bitmap(ip->bitmap[RW_BITMAP_WORLD], al_map_rgba_f(alpha, alpha, alpha, alpha), ip->bitmap[RW_BITMAP_WORLD]->target_width / 2.0, ip->bitmap[RW_BITMAP_WORLD]->target_height / 2.0, 640 / 2, 480 / 2, ip->intro_planet_z, ip->intro_planet_angle, 0);
			break;
		}
	}
}
