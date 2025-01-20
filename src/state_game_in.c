#include "instance.h"

#include "t3f/draw.h"

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
	al_draw_textf(ip->font, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 320 - al_get_text_width(ip->font, "HIGH SCORE - 0:00:00") / 2, t3f_default_view->top, 0, "HIGH SCORE - %d:%02d:%02d", ip->high_score / 3600, (ip->high_score / 60) % 60, (int)(((float)(ip->high_score % 60) / 60.0) * 100.0) % 100);
	t3f_draw_rotated_bitmap(ip->bitmap[RW_BITMAP_WORLD], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), t3f_get_bitmap_width(ip->bitmap[RW_BITMAP_WORLD]) / 2, t3f_get_bitmap_height(ip->bitmap[RW_BITMAP_WORLD]) / 2, 640 / 2, 480 / 2, ip->intro_planet_z, ip->intro_planet_angle, 0);
}
