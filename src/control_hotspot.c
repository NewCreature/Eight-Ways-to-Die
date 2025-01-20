#include "instance.h"

void rw_render_hover_text(ALLEGRO_FONT * fp, float x, float y, int flags, bool highlight, const char * text)
{
	al_draw_text(fp, highlight ? al_map_rgba_f(1.0, 1.0, 1.0, 1.0) : al_map_rgba_f(0.25, 0.25, 0.25, 0.25), x, y, flags, text);
}

int rw_hotspot_logic(RW_INSTANCE * ip)
{
	int key = 0;
	int i;
	
	/* read one touch at a time and simulate key press for first touch detected */
	for(i = 0; i < T3F_MAX_TOUCHES; i++)
	{
		if(t3f_touch_pressed(i))
		{
			if(t3f_get_touch_x(i) <= 160)
			{
				if(t3f_get_touch_y(i) < t3f_default_view->top + ip->third)
				{
					key = '7';
				}
				else if(t3f_get_touch_y(i) < t3f_default_view->top + ip->third * 2.0)
				{
					key = '4';
				}
				else
				{
					key = '1';
				}
			}
			else if(t3f_get_touch_x(i) >= 640 - 160)
			{
				if(t3f_get_touch_y(i) < t3f_default_view->top + ip->third)
				{
					key = '9';
				}
				else if(t3f_get_touch_y(i) < t3f_default_view->top + ip->third * 2.0)
				{
					key = '6';
				}
				else
				{
					key = '3';
				}
			}
			else
			{
				if(t3f_get_touch_y(i) < 240)
				{
					key = '8';
				}
				else
				{
					key = '2';
				}
			}
			t3f_use_touch_press(i);
			break;
		}
	}
	return key;
}

void rw_render_hot_spots(RW_INSTANCE * ip)
{
	float mouse_x = 320.0, mouse_y = 240.0;
	
	if((t3f_flags & T3F_USE_MOUSE) && !(t3f_flags & T3F_USE_TOUCH))
	{
		mouse_x = t3f_get_mouse_x();
		mouse_y = t3f_get_mouse_y();
	}
	t3f_draw_bitmap(ip->bitmap[RW_BITMAP_GUIDE], al_map_rgba_f(0.25, 0.25, 0.25, 0.25), 160.0 - 4.0, t3f_default_view->top + ip->third - 4.0, 0, 0);
	t3f_draw_bitmap(ip->bitmap[RW_BITMAP_GUIDE], al_map_rgba_f(0.25, 0.25, 0.25, 0.25), 640.0 - 160.0 - 4.0, t3f_default_view->top + ip->third - 4.0, 0, 0);
	t3f_draw_bitmap(ip->bitmap[RW_BITMAP_GUIDE], al_map_rgba_f(0.25, 0.25, 0.25, 0.25), 160.0 - 4.0, t3f_default_view->top + ip->third * 2.0 - 4.0, 0, 0);
	t3f_draw_bitmap(ip->bitmap[RW_BITMAP_GUIDE], al_map_rgba_f(0.25, 0.25, 0.25, 0.25), 640.0 - 160.0 - 4.0, t3f_default_view->top + ip->third * 2.0 - 4.0, 0, 0);
	rw_render_hover_text(ip->font, 160.0 / 2.0, t3f_default_view->top + ip->third * 2.0 + ip->third / 2.0 - al_get_font_line_height(ip->font), ALLEGRO_ALIGN_CENTRE, mouse_x <= 160 && mouse_y >= t3f_default_view->top + ip->third * 2.0, "1");
	rw_render_hover_text(ip->font, 320, t3f_default_view->top + ip->third * 2.0 + ip->third / 2.0 - al_get_font_line_height(ip->font), ALLEGRO_ALIGN_CENTRE, mouse_x > 160 && mouse_x < 640 - 160 && mouse_y >= t3f_default_view->top + ip->third * 2.0, "2");
	rw_render_hover_text(ip->font, 640.0 - 160.0 / 2.0, t3f_default_view->top + ip->third * 2.0 + ip->third / 2.0 - al_get_font_line_height(ip->font), ALLEGRO_ALIGN_CENTRE, mouse_x >= 640 - 160 && mouse_y >= t3f_default_view->top + ip->third * 2.0, "3");
	rw_render_hover_text(ip->font, 160.0 / 2.0, t3f_default_view->top + ip->third / 2.0 - al_get_font_line_height(ip->font), ALLEGRO_ALIGN_CENTRE, mouse_x <= 160 && mouse_y < t3f_default_view->top + ip->third, "7");
	rw_render_hover_text(ip->font, 160.0 / 2.0, t3f_default_view->top + ip->third + ip->third / 2.0 - al_get_font_line_height(ip->font), ALLEGRO_ALIGN_CENTRE, mouse_x <= 160 && mouse_y > t3f_default_view->top + ip->third && mouse_y < t3f_default_view->top + ip->third * 2.0, "4");
	rw_render_hover_text(ip->font, 640.0 - 160.0 / 2.0, t3f_default_view->top + ip->third / 2.0 - al_get_font_line_height(ip->font), ALLEGRO_ALIGN_CENTRE, mouse_x >= 640 - 160 && mouse_y < t3f_default_view->top + ip->third, "9");
	rw_render_hover_text(ip->font, 640.0 - 160.0 / 2.0, t3f_default_view->top + ip->third + ip->third / 2.0 - al_get_font_line_height(ip->font), ALLEGRO_ALIGN_CENTRE, mouse_x >= 640 - 160 && mouse_y > t3f_default_view->top + ip->third && mouse_y < t3f_default_view->top + ip->third * 2.0, "6");
	rw_render_hover_text(ip->font, 320.0, t3f_default_view->top + ip->third / 2.0 - al_get_font_line_height(ip->font), ALLEGRO_ALIGN_CENTRE, mouse_x > 160 && mouse_x < 640 - 160 && mouse_y < t3f_default_view->top + ip->third, "8");
}

