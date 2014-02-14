#include "instance.h"

#include "t3f/draw.h"

void rw_state_exit_logic(RW_INSTANCE * ip)
{
	ip->intro_planet_z += 10.0;
	ip->intro_planet_angle += 0.01;
	if(ip->intro_planet_z >= 0.0)
	{
		ip->quit = 1;
	}
}

void rw_state_exit_render(RW_INSTANCE * ip)
{
	float alpha;
	
	al_clear_to_color(al_map_rgb(0, 0, 0));
	alpha = -ip->intro_planet_z / 400.0;
	t3f_draw_rotated_bitmap(ip->bitmap[RW_BITMAP_WORLD], al_map_rgba_f(alpha, alpha, alpha, alpha), al_get_bitmap_width(ip->bitmap[RW_BITMAP_WORLD]) / 2, al_get_bitmap_height(ip->bitmap[RW_BITMAP_WORLD]) / 2, 640 / 2, 480 / 2, ip->intro_planet_z, ip->intro_planet_angle, 0);
}
