#include "t3f/resource.h"
#include "instance.h"
#include "main.h"
#include "ocdfont.h"
#include "high_score.h"
#include "event.h"

static void rw_atlas_bitmap(T3F_ATLAS * ap, ALLEGRO_BITMAP ** bitmap)
{
	ALLEGRO_BITMAP * bp;
	bp = t3f_add_bitmap_to_atlas(ap, bitmap, T3F_ATLAS_SPRITE);

	if(bp)
	{
		al_destroy_bitmap(*bitmap);
		*bitmap = bp;
	}
	else
	{
		printf("could not atlas bitmap\n");
	}
}

int rw_initialize(RW_INSTANCE * ip, int argc, char * argv[])
{
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
	
	t3f_set_event_handler(rw_event_handler);
	
	ip->bitmap[RW_BITMAP_WORLD] = t3f_load_resource((void **)(&ip->bitmap[RW_BITMAP_WORLD]), T3F_RESOURCE_TYPE_BITMAP, "data/world.png", 0, 0, 0);
	ip->bitmap[RW_BITMAP_SHIELD_0] = t3f_load_resource((void **)(&ip->bitmap[RW_BITMAP_SHIELD_0]), T3F_RESOURCE_TYPE_BITMAP, "data/shield0.png", 0, 0, 0);
	ip->bitmap[RW_BITMAP_SHIELD_1] = t3f_load_resource((void **)(&ip->bitmap[RW_BITMAP_SHIELD_1]), T3F_RESOURCE_TYPE_BITMAP, "data/shield1.png", 0, 0, 0);
	ip->bitmap[RW_BITMAP_SHIELD_2] = t3f_load_resource((void **)(&ip->bitmap[RW_BITMAP_SHIELD_2]), T3F_RESOURCE_TYPE_BITMAP, "data/shield2.png", 0, 0, 0);
	ip->bitmap[RW_BITMAP_SHIELD_3] = t3f_load_resource((void **)(&ip->bitmap[RW_BITMAP_SHIELD_3]), T3F_RESOURCE_TYPE_BITMAP, "data/shield3.png", 0, 0, 0);
	ip->bitmap[RW_BITMAP_SHIELD_4] = t3f_load_resource((void **)(&ip->bitmap[RW_BITMAP_SHIELD_4]), T3F_RESOURCE_TYPE_BITMAP, "data/shield4.png", 0, 0, 0);
	ip->bitmap[RW_BITMAP_SHIELD_5] = t3f_load_resource((void **)(&ip->bitmap[RW_BITMAP_SHIELD_5]), T3F_RESOURCE_TYPE_BITMAP, "data/shield5.png", 0, 0, 0);
	ip->bitmap[RW_BITMAP_SHIELD_6] = t3f_load_resource((void **)(&ip->bitmap[RW_BITMAP_SHIELD_6]), T3F_RESOURCE_TYPE_BITMAP, "data/shield6.png", 0, 0, 0);
	ip->bitmap[RW_BITMAP_SHIELD_7] = t3f_load_resource((void **)(&ip->bitmap[RW_BITMAP_SHIELD_7]), T3F_RESOURCE_TYPE_BITMAP, "data/shield7.png", 0, 0, 0);
	ip->bitmap[RW_BITMAP_THREAT] = t3f_load_resource((void **)(&ip->bitmap[RW_BITMAP_THREAT]), T3F_RESOURCE_TYPE_BITMAP, "data/threat.png", 0, 0, 0);
	ip->bitmap[RW_BITMAP_BIG_THREAT] = t3f_load_resource((void **)(&ip->bitmap[RW_BITMAP_BIG_THREAT]), T3F_RESOURCE_TYPE_BITMAP, "data/big_threat.png", 0, 0, 0);
	ip->bitmap[RW_BITMAP_SHIP] = t3f_load_resource((void **)(&ip->bitmap[RW_BITMAP_SHIP]), T3F_RESOURCE_TYPE_BITMAP, "data/ship.png", 0, 0, 0);
	ip->bitmap[RW_BITMAP_SHOT] = t3f_load_resource((void **)(&ip->bitmap[RW_BITMAP_SHOT]), T3F_RESOURCE_TYPE_BITMAP, "data/shot.png", 0, 0, 0);
	ip->bitmap[RW_BITMAP_PARTICLE] = t3f_load_resource((void **)(&ip->bitmap[RW_BITMAP_PARTICLE]), T3F_RESOURCE_TYPE_BITMAP, "data/particle.png", 0, 0, 0);
	ip->bitmap[RW_BITMAP_LOGO] = t3f_load_resource((void **)(&ip->bitmap[RW_BITMAP_LOGO]), T3F_RESOURCE_TYPE_BITMAP, "data/logo.png", 0, 0, 0);
	ip->bitmap[RW_BITMAP_T3_LOGO] = t3f_load_resource((void **)(&ip->bitmap[RW_BITMAP_T3_LOGO]), T3F_RESOURCE_TYPE_BITMAP, "data/t3.png", 0, 0, 0);
	ip->bitmap[RW_BITMAP_ICON] = t3f_load_resource((void **)(&ip->bitmap[RW_BITMAP_ICON]), T3F_RESOURCE_TYPE_BITMAP, "data/icon.png", 0, 0, 0);
	ip->bitmap[RW_BITMAP_GUIDE] = t3f_load_resource((void **)(&ip->bitmap[RW_BITMAP_GUIDE]), T3F_RESOURCE_TYPE_BITMAP, "data/guide.png", 0, 0, 0);
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
		rw_atlas_bitmap(ip->atlas, &ip->bitmap[RW_BITMAP_WORLD]);
		rw_atlas_bitmap(ip->atlas, &ip->bitmap[RW_BITMAP_SHIELD_0]);
		rw_atlas_bitmap(ip->atlas, &ip->bitmap[RW_BITMAP_SHIELD_1]);
		rw_atlas_bitmap(ip->atlas, &ip->bitmap[RW_BITMAP_SHIELD_2]);
		rw_atlas_bitmap(ip->atlas, &ip->bitmap[RW_BITMAP_SHIELD_3]);
		rw_atlas_bitmap(ip->atlas, &ip->bitmap[RW_BITMAP_SHIELD_4]);
		rw_atlas_bitmap(ip->atlas, &ip->bitmap[RW_BITMAP_SHIELD_5]);
		rw_atlas_bitmap(ip->atlas, &ip->bitmap[RW_BITMAP_SHIELD_6]);
		rw_atlas_bitmap(ip->atlas, &ip->bitmap[RW_BITMAP_SHIELD_7]);
		rw_atlas_bitmap(ip->atlas, &ip->bitmap[RW_BITMAP_THREAT]);
		rw_atlas_bitmap(ip->atlas, &ip->bitmap[RW_BITMAP_BIG_THREAT]);
		rw_atlas_bitmap(ip->atlas, &ip->bitmap[RW_BITMAP_SHIP]);
		rw_atlas_bitmap(ip->atlas, &ip->bitmap[RW_BITMAP_SHOT]);
		rw_atlas_bitmap(ip->atlas, &ip->bitmap[RW_BITMAP_PARTICLE]);
		rw_atlas_bitmap(ip->atlas, &ip->bitmap[RW_BITMAP_GUIDE]);
	}
	
	#ifndef ALLEGRO_MACOSX
		#ifndef T3F_ANDROID
			al_set_display_icon(t3f_display, ip->bitmap[RW_BITMAP_ICON]);
		#endif
	#endif
	
	ip->font = ocd_load_font("data/gamefont.pcx");
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
	for(i = RW_SAMPLE_SHIELD; i <= RW_SAMPLE_LOGO_OUT; i++)
	{
		if(!ip->sample[i])
		{
			printf("Error loading sound %d!\n", i);
			return 0;
		}
	}

	rw_load_high_score(ip, t3f_get_filename(t3f_data_path, "rw.hs"));
	
	ip->vertical_scale = (t3f_display_bottom - t3f_display_top) / 480.0;
	ip->third = (t3f_display_bottom - t3f_display_top) / 3.0;
	t3f_srand(&ip->rng_state, time(0));
	return 1;
}
