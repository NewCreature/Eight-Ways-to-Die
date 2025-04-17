#include "instance.h"

#include "t3f/draw.h"
#include "t3f/sound.h"

#include "control_keyboard.h"
#include "control_hotspot.h"

#include "music.h"

#include "state_game.h"
#include "text.h"

static void rw_render_menu(RW_INSTANCE * ip)
{
	float mouse_x = 320.0, mouse_y = 240.0;

	if(t3f_flags & T3F_USE_MOUSE)
	{
		mouse_x = t3f_get_mouse_x();
		mouse_y = t3f_get_mouse_y();
	}
	t3f_draw_bitmap(ip->bitmap[RW_BITMAP_GUIDE], al_map_rgba_f(0.25, 0.25, 0.25, 0.25), 160.0 - 4.0, t3f_default_view->top + ip->third * 2.0 - 4.0, 0, 0);
	t3f_draw_bitmap(ip->bitmap[RW_BITMAP_GUIDE], al_map_rgba_f(0.25, 0.25, 0.25, 0.25), 640.0 - 160.0 - 4.0, t3f_default_view->top + ip->third * 2.0 - 4.0, 0, 0);
	rw_render_hover_text(ip->font, 160.0 / 2.0, t3f_default_view->top + ip->third * 2.0 + ip->third / 2.0 - t3f_get_font_line_height(ip->font), T3F_FONT_ALIGN_CENTER, mouse_x <= 160 && mouse_y >= t3f_default_view->top + ip->third * 2.0, "1");
	rw_render_hover_text(ip->font, 320, t3f_default_view->top + ip->third * 2.0 + ip->third / 2.0 - t3f_get_font_line_height(ip->font), T3F_FONT_ALIGN_CENTER, mouse_x > 160 && mouse_x < 640 - 160 && mouse_y >= t3f_default_view->top + ip->third * 2.0, "2");
	rw_render_hover_text(ip->font, 640.0 - 160.0 / 2.0, t3f_default_view->top + ip->third * 2.0 + ip->third / 2.0 - t3f_get_font_line_height(ip->font), T3F_FONT_ALIGN_CENTER, mouse_x >= 640 - 160 && mouse_y >= t3f_default_view->top + ip->third * 2.0, "3");
	rw_render_hover_text(ip->font, 160.0 / 2.0, t3f_default_view->top + ip->third * 2.0 + ip->third / 2.0, T3F_FONT_ALIGN_CENTER, mouse_x <= 160 && mouse_y >= t3f_default_view->top + ip->third * 2.0, "PRIVACY INFO");
	rw_render_hover_text(ip->font, 320, t3f_default_view->top + ip->third * 2.0 + ip->third / 2.0, T3F_FONT_ALIGN_CENTER, mouse_x > 160 && mouse_x < 640 - 160 && mouse_y >= t3f_default_view->top + ip->third * 2.0, "START GAME");
	rw_render_hover_text(ip->font, 640.0 - 160.0 / 2.0, t3f_default_view->top + ip->third * 2.0 + ip->third / 2.0, T3F_FONT_ALIGN_CENTER, mouse_x >= 640 - 160 && mouse_y >= t3f_default_view->top + ip->third * 2.0, "TOGGLE MUSIC");
	t3f_draw_text(ip->font, t3f_color_white, t3f_default_view->virtual_width / 2.0, t3f_default_view->virtual_height - t3f_get_font_line_height(ip->font), 0, T3F_FONT_ALIGN_CENTER, T3F_APP_COPYRIGHT);
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
		t3f_play_sample(ip->sample[RW_SAMPLE_GAME_START], 0.5, 0.0, 1.0);
		ip->intro_planet_vangle = ip->intro_planet_angle / (ip->intro_planet_z / 10.0);
		rw_initialize_game(ip);
		ip->state = RW_STATE_GAME_IN;
	}
	else if(key == '3')
	{
		rw_toggle_music(ip);
	}
	if(t3f_key_pressed(ALLEGRO_KEY_ESCAPE) || t3f_key_pressed(ALLEGRO_KEY_BACK))
	{
		ip->state = RW_STATE_EXIT;
		t3f_use_key_press(ALLEGRO_KEY_ESCAPE);
		t3f_use_key_press(ALLEGRO_KEY_BACK);
	}
}

void rw_state_title_render(RW_INSTANCE * ip)
{
	float alpha;

	al_clear_to_color(al_map_rgb(0, 0, 0));
	rw_draw_time_text(ip->font, t3f_color_white, t3f_default_view->virtual_width / 2.0, t3f_default_view->top, T3F_FONT_ALIGN_CENTER, "HIGH SCORE - ", ip->high_score);
	t3f_draw_rotated_bitmap(ip->bitmap[RW_BITMAP_WORLD], al_map_rgba_f(1.0, 1.0, 1.0, 1.0), ip->bitmap[RW_BITMAP_WORLD]->target_width / 2.0, ip->bitmap[RW_BITMAP_WORLD]->target_height / 2.0, 640 / 2, 480 / 2, ip->intro_planet_z, ip->intro_planet_angle, 0);
	alpha = -ip->logo_z / 10.0;
	t3f_draw_bitmap(ip->bitmap[RW_BITMAP_LOGO], al_map_rgba_f(alpha, alpha, alpha, alpha), 320.0 - ip->bitmap[RW_BITMAP_LOGO]->target_width / 2.0, 240 - ip->bitmap[RW_BITMAP_LOGO]->target_height - 128, ip->logo_z + 10.0, 0);
	alpha = 1.0 + ip->logo_z / 10.0;
	t3f_draw_bitmap(ip->bitmap[RW_BITMAP_LOGO], al_map_rgba_f(alpha, alpha, alpha, alpha), 320.0 - ip->bitmap[RW_BITMAP_LOGO]->target_width / 2.0, 240 - ip->bitmap[RW_BITMAP_LOGO]->target_height - 128, ip->logo_z, 0);
	rw_render_menu(ip);
}
