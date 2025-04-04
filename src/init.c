#include "t3f/resource.h"
#include "instance.h"
#include "main.h"
#include "high_score.h"
#include "event.h"

static void rw_atlas_bitmap(T3F_ATLAS * ap, T3F_BITMAP * bitmap)
{
	t3f_add_bitmap_to_atlas(ap, &bitmap->bitmap, T3F_ATLAS_SPRITE);
}

static void _rw_set_optimal_display_size(void)
{
	ALLEGRO_MONITOR_INFO info;
	int width, height;
	int current_width = 640;
	int current_height = 480;
	int width_increment = 640 / 4;
	int height_increment = 480 / 4;
	int c = 0;

	al_get_monitor_info(0, &info);
	width = info.x2 - info.x1 - 64;
	height = info.y2 - info.y1 - 64;
	while(current_width < width && current_height < height)
	{
		current_width += width_increment;
		current_height += height_increment;
		c++;
	}
	current_width -= width_increment;
	current_height -= height_increment;
	if(c > 1)
	{
		current_width -= width_increment;
		current_height -= height_increment;
	}
	if(current_width != al_get_display_width(t3f_display) || current_height != al_get_display_height(t3f_display))
	{
		t3f_set_gfx_mode(current_width, current_height, t3f_flags);
		al_set_window_position(t3f_display, (info.x2 - info.x1) / 2 - current_width / 2, (info.y2 - info.y1) / 2 - current_height / 2);
	}
}

int rw_initialize(RW_INSTANCE * ip, int argc, char * argv[])
{
	char buf[1024];
	int i;
	int flags = 0;

	flags |= T3F_USE_KEYBOARD;
	flags |= T3F_USE_MOUSE;
	flags |= T3F_USE_TOUCH;
	flags |= T3F_USE_SOUND;
	flags |= T3F_FORCE_ASPECT;
	flags |= T3F_FILL_SCREEN;

	if(!t3f_initialize("Eight Ways to Die", 640, 480, 60.0, rw_logic, rw_render, flags, ip))
	{
		return 0;
	}
	if(!al_get_config_value(t3f_config, "T3F", "display_width"))
	{
		_rw_set_optimal_display_size();
	}

	t3f_set_event_handler(rw_event_handler);

	ip->bitmap[RW_BITMAP_WORLD] = t3f_load_bitmap("data/world.png", T3F_BITMAP_FLAG_PADDED, false);
	ip->bitmap[RW_BITMAP_SHIELD_0] = t3f_load_bitmap("data/shield0.png", T3F_BITMAP_FLAG_PADDED, false);
	ip->bitmap[RW_BITMAP_SHIELD_1] = t3f_load_bitmap("data/shield1.png", T3F_BITMAP_FLAG_PADDED, false);
	ip->bitmap[RW_BITMAP_SHIELD_2] = t3f_load_bitmap("data/shield2.png", T3F_BITMAP_FLAG_PADDED, false);
	ip->bitmap[RW_BITMAP_SHIELD_3] = t3f_load_bitmap("data/shield3.png", T3F_BITMAP_FLAG_PADDED, false);
	ip->bitmap[RW_BITMAP_SHIELD_4] = t3f_load_bitmap("data/shield4.png", T3F_BITMAP_FLAG_PADDED, false);
	ip->bitmap[RW_BITMAP_SHIELD_5] = t3f_load_bitmap("data/shield5.png", T3F_BITMAP_FLAG_PADDED, false);
	ip->bitmap[RW_BITMAP_SHIELD_6] = t3f_load_bitmap("data/shield6.png", T3F_BITMAP_FLAG_PADDED, false);
	ip->bitmap[RW_BITMAP_SHIELD_7] = t3f_load_bitmap("data/shield7.png", T3F_BITMAP_FLAG_PADDED, false);
	ip->bitmap[RW_BITMAP_THREAT] = t3f_load_bitmap("data/threat.png", T3F_BITMAP_FLAG_PADDED, false);
	ip->bitmap[RW_BITMAP_BIG_THREAT] = t3f_load_bitmap("data/big_threat.png", T3F_BITMAP_FLAG_PADDED, false);
	ip->bitmap[RW_BITMAP_SHIP] = t3f_load_bitmap("data/ship.png", T3F_BITMAP_FLAG_PADDED, false);
	ip->bitmap[RW_BITMAP_SHOT] = t3f_load_bitmap("data/shot.png", T3F_BITMAP_FLAG_PADDED, false);
	ip->bitmap[RW_BITMAP_PARTICLE] = t3f_load_bitmap("data/particle.png", T3F_BITMAP_FLAG_PADDED, false);
	ip->bitmap[RW_BITMAP_LOGO] = t3f_load_bitmap("data/logo.png", T3F_BITMAP_FLAG_PADDED, false);
	ip->bitmap[RW_BITMAP_T3_LOGO] = t3f_load_bitmap("data/t3.png", T3F_BITMAP_FLAG_PADDED, false);
	ip->bitmap[RW_BITMAP_GUIDE] = t3f_load_bitmap("data/guide.png", T3F_BITMAP_FLAG_PADDED, false);
	for(i = RW_BITMAP_WORLD; i <= RW_BITMAP_BIG_THREAT; i++)
	{
		if(!ip->bitmap[i])
		{
			printf("Error loading image %d!\n", i);
			return 0;
		}
	}
	ip->atlas = t3f_create_atlas(1024, 1024);
	if(ip->atlas)
	{
		rw_atlas_bitmap(ip->atlas, ip->bitmap[RW_BITMAP_WORLD]);
		rw_atlas_bitmap(ip->atlas, ip->bitmap[RW_BITMAP_SHIELD_0]);
		rw_atlas_bitmap(ip->atlas, ip->bitmap[RW_BITMAP_SHIELD_1]);
		rw_atlas_bitmap(ip->atlas, ip->bitmap[RW_BITMAP_SHIELD_2]);
		rw_atlas_bitmap(ip->atlas, ip->bitmap[RW_BITMAP_SHIELD_3]);
		rw_atlas_bitmap(ip->atlas, ip->bitmap[RW_BITMAP_SHIELD_4]);
		rw_atlas_bitmap(ip->atlas, ip->bitmap[RW_BITMAP_SHIELD_5]);
		rw_atlas_bitmap(ip->atlas, ip->bitmap[RW_BITMAP_SHIELD_6]);
		rw_atlas_bitmap(ip->atlas, ip->bitmap[RW_BITMAP_SHIELD_7]);
		rw_atlas_bitmap(ip->atlas, ip->bitmap[RW_BITMAP_THREAT]);
		rw_atlas_bitmap(ip->atlas, ip->bitmap[RW_BITMAP_BIG_THREAT]);
		rw_atlas_bitmap(ip->atlas, ip->bitmap[RW_BITMAP_SHIP]);
		rw_atlas_bitmap(ip->atlas, ip->bitmap[RW_BITMAP_SHOT]);
		rw_atlas_bitmap(ip->atlas, ip->bitmap[RW_BITMAP_PARTICLE]);
		rw_atlas_bitmap(ip->atlas, ip->bitmap[RW_BITMAP_GUIDE]);
	}

	ip->font = t3f_load_font("data/gamefont.pcx", T3F_FONT_TYPE_AUTO, 0, 0, false);
	if(!ip->font)
	{
		printf("Error loading font!\n");
		return 0;
	}

	ip->sample[RW_SAMPLE_DAMAGE] = al_load_sample("data/damage.ogg");
	ip->sample[RW_SAMPLE_HIT] = al_load_sample("data/hit.ogg");
	ip->sample[RW_SAMPLE_SHIELD] = al_load_sample("data/shield.ogg");
	ip->sample[RW_SAMPLE_ALERT] = al_load_sample("data/alert.ogg");
	ip->sample[RW_SAMPLE_LEVEL_UP] = al_load_sample("data/levelup.ogg");
	ip->sample[RW_SAMPLE_HIGH_SCORE] = al_load_sample("data/highscore.ogg");
	ip->sample[RW_SAMPLE_LOGO] = al_load_sample("data/logo.ogg");
	ip->sample[RW_SAMPLE_GAME_START] = al_load_sample("data/gamestart.ogg");
	ip->sample[RW_SAMPLE_LOGO_OUT] = al_load_sample("data/logoout.ogg");
	ip->sample[RW_SAMPLE_SHIP_FIRE] = al_load_sample("data/fire.ogg");
	for(i = RW_SAMPLE_SHIELD; i <= RW_SAMPLE_SHIP_FIRE; i++)
	{
		if(!ip->sample[i])
		{
			printf("Error loading sound %d!\n", i);
			return 0;
		}
	}

	rw_load_high_score(ip, t3f_get_filename(t3f_data_path, "rw.hs", buf, 1024));

	ip->vertical_scale = (t3f_default_view->bottom - t3f_default_view->top) / 480.0;
	ip->third = (t3f_default_view->bottom - t3f_default_view->top) / 3.0;
	t3f_srand(&ip->rng_state, time(0));
	return 1;
}
