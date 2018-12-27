#include "instance.h"

#include "t3f/resource.h"

#include "high_score.h"

int rw_exit(RW_INSTANCE * ip)
{
	char buf[1024];
	int i;

	rw_save_high_score(ip, t3f_get_filename(t3f_data_path, "rw.hs", buf, 1024));
	for(i = 0; i < RW_MAX_BITMAPS; i++)
	{
		if(ip->bitmap[i])
		{
			t3f_destroy_resource(ip->bitmap[i]);
		}
	}
	for(i = 0; i < RW_MAX_SAMPLES; i++)
	{
		if(ip->sample[i])
		{
			al_destroy_sample(ip->sample[i]);
		}
	}
	al_destroy_font(ip->font);
	return 1;
}
