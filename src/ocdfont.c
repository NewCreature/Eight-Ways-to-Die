#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>

static void ocd_convert_grey_to_alpha(ALLEGRO_BITMAP *bitmap)
{
	int x, y;
	unsigned char ir, ig, ib, ia;
	ALLEGRO_COLOR pixel;
	ALLEGRO_STATE old_state;

	if(!al_lock_bitmap(bitmap, al_get_bitmap_format(bitmap), 0))
	{
		return;
	}

	al_store_state(&old_state, ALLEGRO_STATE_TARGET_BITMAP);
	al_set_target_bitmap(bitmap);

	for(y = 0; y < al_get_bitmap_height(bitmap); y++)
	{
		for(x = 0; x < al_get_bitmap_width(bitmap); x++)
		{
			pixel = al_get_pixel(bitmap, x, y);
			al_unmap_rgba(pixel, &ir, &ig, &ib, &ia);
			if(ir == 255 && ig == 0 && ib == 255)
			{
				pixel = al_map_rgba(0, 0, 0, 0);
				al_put_pixel(x, y, pixel);
			}
			else if(ia > 0 && !(ir == 255 && ig == 255 && ib == 0))
			{
				pixel = al_map_rgba(ir, ir, ir, ir);
				al_put_pixel(x, y, pixel);
			}
		}
	}

	al_restore_state(&old_state);
	al_unlock_bitmap(bitmap);
}

ALLEGRO_FONT * ocd_load_font(char * fn)
{
	int ranges[] = {32, 126};
	ALLEGRO_FONT * fp;
	ALLEGRO_STATE old_state;
	ALLEGRO_BITMAP * fimage;	
//	int old_format = al_get_new_bitmap_format();
	
	al_store_state(&old_state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
//	al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ANY_WITH_ALPHA);
	al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP | ALLEGRO_NO_PREMULTIPLIED_ALPHA);
	fimage = al_load_bitmap(fn);
	if(!fimage)
	{
		return NULL;
	}
	ocd_convert_grey_to_alpha(fimage);
	al_restore_state(&old_state);
	fp = al_grab_font_from_bitmap(fimage, 1, ranges);
/*	if(!fp)
	{
		al_destroy_bitmap(fimage);
		return NULL;
	} */
	al_destroy_bitmap(fimage);
//	al_set_new_bitmap_format(old_format);
	return fp;
}
