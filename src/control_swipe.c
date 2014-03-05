#include "instance.h"

static int rw_single_swipe_logic(RW_INSTANCE * ip, int i)
{
	double angle;
	double slice;
	double offset;

	switch(ip->swipe[i].state)
	{
		case RW_SWIPE_INACTIVE:
		{
			if(t3f_touch[i].active)
			{
				ip->swipe[i].x = t3f_touch[i].x;
				ip->swipe[i].y = t3f_touch[i].y;
				ip->swipe[i].state = RW_SWIPE_ACTIVE;
				return 0;
			}
			break;
		}
		case RW_SWIPE_ACTIVE:
		{
			if(!t3f_touch[i].active)
			{
				ip->swipe[i].state = RW_SWIPE_INACTIVE;
				return 0;
			}
			else
			{
				if(t3f_distance(t3f_touch[i].x, t3f_touch[i].y, ip->swipe[i].x, ip->swipe[i].y) >= 32.0)
				{
					slice = (ALLEGRO_PI * 2.0) / 8.0;
					offset = slice / 2.0;
					angle = atan2(t3f_touch[i].y - ip->swipe[i].y, t3f_touch[i].x - ip->swipe[i].x) + ALLEGRO_PI;
					if(angle + offset > ALLEGRO_PI * 2.0)
					{
						angle -= ALLEGRO_PI * 2.0;
					}
					ip->swipe[i].state = RW_SWIPE_USED;
					if(angle + offset >= slice * 7.0)
					{
						return '1';
					}
					else if(angle + offset >= slice * 6.0)
					{
						return '2';
					}
					else if(angle + offset >= slice * 5.0)
					{
						return '3';
					}
					else if(angle + offset >= slice * 4.0)
					{
						return '6';
					}
					else if(angle + offset >= slice * 3.0)
					{
						return '9';
					}
					else if(angle + offset >= slice * 2.0)
					{
						return '8';
					}
					else if(angle + offset >= slice * 1.0)
					{
						return '7';
					}
					else
					{
						return '4';
					}
				}
			}
			break;
		}
		case RW_SWIPE_USED:
		{
			if(!t3f_touch[i].active)
			{
				ip->swipe[i].state = RW_SWIPE_INACTIVE;
			}
			break;
		}
	}
	return 0;
}

/* read swipe controls and simulate keys, returns character */
int rw_swipe_logic(RW_INSTANCE * ip)
{
	int i, key;
	
	for(i = 0; i < T3F_MAX_TOUCHES; i++)
	{
		key = rw_single_swipe_logic(ip, i);
		if(key != 0)
		{
			break;
		}
	}
	return key;
}

void rw_render_swipe(RW_INSTANCE * ip)
{
	int i;
	
	for(i = 0; i < T3F_MAX_TOUCHES; i++)
	{
		if(ip->swipe[i].state == RW_SWIPE_ACTIVE)
		{
			al_draw_tinted_bitmap(ip->bitmap[RW_BITMAP_GUIDE], al_map_rgba_f(0.25, 0.25, 0.25, 0.25), ip->swipe[i].x - 4.0, ip->swipe[i].y - 4.0, 0);
			al_draw_tinted_bitmap(ip->bitmap[RW_BITMAP_GUIDE], al_map_rgba_f(0.25, 0.25, 0.25, 0.25), t3f_touch[i].x - 4.0, t3f_touch[i].y - 4.0, 0);
		}
	}
}
