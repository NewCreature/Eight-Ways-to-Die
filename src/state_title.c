#include "instance.h"

#include "t3f/draw.h"

#include "control_keyboard.h"
#include "control_hotspot.h"

#include "music.h"

#include "state_game.h"

static void rw_render_menu(RW_INSTANCE * ip)
{
	float mouse_x = 320.0, mouse_y = 240.0;
	
	if((t3f_flags & T3F_USE_MOUSE) && !(t3f_flags & T3F_USE_TOUCH))
	{
		mouse_x = t3f_mouse_x;
		mouse_y = t3f_mouse_y;
	}
	al_draw_tinted_bitmap(ip->bitmap[RW_BITMAP_GUIDE], al_map_rgba_f(0.25, 0.25, 0.25, 0.25), 160.0 - 4.0, t3f_display_top + ip->third * 2.0 - 4.0, 0);
	al_draw_tinted_bitmap(ip->bitmap[RW_BITMAP_GUIDE], al_map_rgba_f(0.25, 0.25, 0.25, 0.25), 640.0 - 160.0 - 4.0, t3f_display_top + ip->third * 2.0 - 4.0, 0);
	rw_render_hover_text(ip->font, 160.0 / 2.0, t3f_display_top + ip->third * 2.0 + ip->third / 2.0 - al_get_font_line_height(ip->font), ALLEGRO_ALIGN_CENTRE, mouse_x <= 160 && mouse_y >= t3f_display_top + ip->third * 2.0, "1");
	rw_render_hover_text(ip->font, 320, t3f_display_top + ip->third * 2.0 + ip->third / 2.0 - al_get_font_line_height(ip->font), ALLEGRO_ALIGN_CENTRE, mouse_x > 160 && mouse_x < 640 - 160 && mouse_y >= t3f_display_top + ip->third * 2.0, "2");
	rw_render_hover_text(ip->font, 640.0 - 160.0 / 2.0, t3f_display_top + ip->third * 2.0 + ip->third / 2.0 - al_get_font_line_height(ip->font), ALLEGRO_ALIGN_CENTRE, mouse_x >= 640 - 160 && mouse_y >= t3f_display_top + ip->third * 2.0, "3");
	rw_render_hover_text(ip->font, 160.0 / 2.0, t3f_display_top + ip->third * 2.0 + ip->third / 2.0, ALLEGRO_ALIGN_CENTRE, mouse_x <= 160 && mouse_y >= t3f_display_top + ip->third * 2.0, "PRIVACY INFO");
	rw_render_hover_text(ip->font, 320, t3f_display_top + ip->third * 2.0 + ip->third / 2.0, ALLEGRO_ALIGN_CENTRE, mouse_x > 160 && mouse_x < 640 - 160 && mouse_y >= t3f_display_top + ip->third * 2.0, "START GAME");
	rw_render_hover_text(ip->font, 640.0 - 160.0 / 2.0, t3f_display_top + ip->third * 2.0 + ip->third / 2.0, ALLEGRO_ALIGN_CENTRE, mouse_x >= 640 - 160 && mouse_y >= t3f_display_top + ip->third * 2.0, "TOGGLE MUSIC");
}

void rw_title_logo_logic(RW_INSTANCE * ip)
{
	ip->logo_z -= 0.1;
	if(ip->logo_z <= -10.0)
	{
		ip->logo_z = 0.0;
	}
}

void rw_state_title_logic(RW_INSTANCE * ip)
{
	int key = 0;
	int touch_key = 0;
	
	key = rw_keyboard_logic(ip);
	
	touch_key = rw_hotspot_logic(ip);
	if(touch_key)
	{
		key = touch_key;
	}
	
	ip->intro_planet_angle += 0.01;
	if(ip->intro_planet_angle >= ALLEGRO_PI * 2.0)
	{
		ip->intro_planet_angle -= ALLEGRO_PI * 2.0;
	}
	ip->start_alpha += 0.025;
	if(ip->start_alpha > 1.0)
	{
		ip->start_alpha = 0.0;
	}
	rw_title_logo_logic(ip);
	if(key == '1')
	{
		ip->state = RW_STATE_PRIVACY;
	}
	else if(key == '2')
	{
		al_play_sample(ip->sample[RW_SAMPLE_GAME_START], 0.5, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
		ip->intro_planet_vangle = ip->intro_planet_angle / (ip->intro_planet_z / 10.0);
		ip->game_mode = RW_GAME_MODE_NORMAL;
		rw_initialize_game(ip);
		ip->state = RW_STATE_GAME_IN;
	}
	else if(key == '3')
	{
		rw_toggle_music(ip);
	}
	if(t3f_key[ALLEGRO_KEY_ESCAPE] || t3f_key[ALLEGRO_KEY_BACK])
	{
		ip->state = RW_STATE_EXIT;
		t3f_key[ALLEGRO_KEY_ESCAPE] = 0;
		t3f_key[ALLEGRO_KEY_BACK] = 0;
	}
}

void rw_state_title_render(RW_INSTANCE * ip)
{
	float alpha;
	
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_textf(ip->font, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 320 - al_get_text_width(ip->font, "HIGH SCORE - 0:00:00") / 2, t3f_display_top, 0, "HIGH SCORE - %d:%02d:%02d", ip->high_score / 3600, (ip->high_score / 60) % 60, (int)(((float)(ip->high_score % 60) / 60.0) * 100.0) % 100);
	t3f_draw_rotated_bitmap(ip->bitmap[RW_BITMAP_WORLD], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), al_get_bitmap_width(ip->bitmap[RW_BITMAP_WORLD]) / 2, al_get_bitmap_height(ip->bitmap[RW_BITMAP_WORLD]) / 2, 640 / 2, 480 / 2, ip->intro_planet_z, ip->intro_planet_angle, 0);
	alpha = -ip->logo_z / 10.0;
	t3f_draw_bitmap(ip->bitmap[RW_BITMAP_LOGO], al_map_rgba_f(alpha, alpha, alpha, alpha), 320.0 - (float)al_get_bitmap_width(ip->bitmap[RW_BITMAP_LOGO]) / 2, 240 - al_get_bitmap_height(ip->bitmap[RW_BITMAP_LOGO]) - 128, ip->logo_z + 10.0, 0);
	alpha = 1.0 + ip->logo_z / 10.0;
	t3f_draw_bitmap(ip->bitmap[RW_BITMAP_LOGO], al_map_rgba_f(alpha, alpha, alpha, alpha), 320.0 - (float)al_get_bitmap_width(ip->bitmap[RW_BITMAP_LOGO]) / 2, 240 - al_get_bitmap_height(ip->bitmap[RW_BITMAP_LOGO]) - 128, ip->logo_z, 0);
	rw_render_menu(ip);
}
