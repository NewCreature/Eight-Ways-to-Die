#include "instance.h"

#include "t3f/draw.h"

#include "control_keyboard.h"
#include "control_hotspot.h"

#include "state_title.h"

void rw_state_privacy_logic(RW_INSTANCE * ip)
{
	int key = 0;
	
	rw_title_logo_logic(ip);
	key = rw_keyboard_logic(ip);
	if(!key)
	{
		key = rw_hotspot_logic(ip);
	}
	if(key)
	{
		ip->state = RW_STATE_TITLE;
	}
}

void rw_state_privacy_render(RW_INSTANCE * ip)
{
	float alpha;
	
	al_clear_to_color(al_map_rgb(0, 0, 0));
	alpha = -ip->logo_z / 10.0;
	t3f_draw_bitmap(ip->bitmap[RW_BITMAP_LOGO], al_map_rgba_f(alpha, alpha, alpha, alpha), 320.0 - ip->bitmap[RW_BITMAP_LOGO]->target_width / 2.0, 240 - ip->bitmap[RW_BITMAP_LOGO]->target_height - 128, ip->logo_z + 10.0, 0);
	alpha = 1.0 + ip->logo_z / 10.0;
	t3f_draw_bitmap(ip->bitmap[RW_BITMAP_LOGO], al_map_rgba_f(alpha, alpha, alpha, alpha), 320.0 - ip->bitmap[RW_BITMAP_LOGO]->target_width / 2.0, 240 - ip->bitmap[RW_BITMAP_LOGO]->target_height - 128, ip->logo_z, 0);
	t3f_draw_text(ip->font, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 160, 240 - t3f_get_font_line_height(ip->font) * 2.0, 0, 0, "THIS GAME ONLY STORES SETTINGS");
	t3f_draw_text(ip->font, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 160, 240 - t3f_get_font_line_height(ip->font) * 1.0, 0, 0, "AND SCORES LOCALLY. IT WILL NOT");
	t3f_draw_text(ip->font, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 160, 240 + t3f_get_font_line_height(ip->font) * 0.0, 0, 0, "SHARE ANY INFORMATION OVER");
	t3f_draw_text(ip->font, al_map_rgba_f(1.0, 1.0, 1.0, 1.0), 160, 240 + t3f_get_font_line_height(ip->font) * 1.0, 0, 0, "THE INTERNET.");
}
