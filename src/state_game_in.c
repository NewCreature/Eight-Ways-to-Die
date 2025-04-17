#include "t3f/t3f.h"
#include "instance.h"

#include "text.h"

void rw_state_game_in_logic(RW_INSTANCE * ip)
{
	ip->intro_planet_z += 10.0;
	if(ip->intro_planet_z >= 0.0)
	{
		ip->state = RW_STATE_GAME;
	}
	ip->intro_planet_angle += ip->intro_planet_vangle;
}

void rw_state_game_in_render(RW_INSTANCE * ip)
{
	al_clear_to_color(al_map_rgb(0, 0, 0));
	rw_draw_time_text(ip->font, t3f_color_white, t3f_default_view->virtual_width / 2.0, t3f_default_view->top, T3F_FONT_ALIGN_CENTER, "HIGH SCORE - ", ip->high_score);
	t3f_draw_rotated_bitmap(ip->bitmap[RW_BITMAP_WORLD], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), ip->bitmap[RW_BITMAP_WORLD]->target_width / 2.0, ip->bitmap[RW_BITMAP_WORLD]->target_height / 2.0, 640 / 2, 480 / 2, ip->intro_planet_z, ip->intro_planet_angle, 0);
}
