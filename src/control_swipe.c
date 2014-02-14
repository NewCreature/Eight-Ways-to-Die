#include "instance.h"

/* read swipe controls and simulate keys, returns character */
int rw_swipe_logic(RW_INSTANCE * ip)
{
	int i;
	double angle;
	double slice;
	double offset;
	
	switch(ip->swipe_state)
	{
		case RW_SWIPE_INACTIVE:
		{
			for(i = 0; i < T3F_MAX_TOUCHES; i++)
			{
				if(t3f_touch[i].active)
				{
					ip->swipe_touch = i;
					ip->swipe_x = t3f_touch[i].x;
					ip->swipe_y = t3f_touch[i].y;
					ip->swipe_state = RW_SWIPE_ACTIVE;
					return 0;
				}
			}
			break;
		}
		case RW_SWIPE_ACTIVE:
		{
			if(!t3f_touch[ip->swipe_touch].active)
			{
				ip->swipe_state = RW_SWIPE_INACTIVE;
				return 0;
			}
			else
			{
				if(t3f_distance(t3f_touch[ip->swipe_touch].x, t3f_touch[ip->swipe_touch].y, ip->swipe_x, ip->swipe_y) >= 32.0)
				{
					slice = (ALLEGRO_PI * 2.0) / 8.0;
					offset = slice / 2.0;
					angle = atan2(t3f_touch[ip->swipe_touch].y - ip->swipe_y, t3f_touch[ip->swipe_touch].x - ip->swipe_x) + ALLEGRO_PI;
					if(angle + offset > ALLEGRO_PI * 2.0)
					{
						angle -= ALLEGRO_PI * 2.0;
					}
					ip->swipe_state = RW_SWIPE_USED;
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
			if(!t3f_touch[ip->swipe_touch].active)
			{
				ip->swipe_state = RW_SWIPE_INACTIVE;
			}
			break;
		}
	}
	return 0;
}

void rw_render_swipe(RW_INSTANCE * ip)
{
	if(ip->swipe_state == RW_SWIPE_ACTIVE)
	{
		al_draw_tinted_bitmap(ip->bitmap[RW_BITMAP_GUIDE], al_map_rgba_f(0.25, 0.25, 0.25, 0.25), ip->swipe_x - 4.0, ip->swipe_y - 4.0, 0);
		al_draw_tinted_bitmap(ip->bitmap[RW_BITMAP_GUIDE], al_map_rgba_f(0.25, 0.25, 0.25, 0.25), t3f_touch[ip->swipe_touch].x - 4.0, t3f_touch[ip->swipe_touch].y - 4.0, 0);
	}
}
