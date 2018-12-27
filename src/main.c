#include "init.h"
#include "exit.h"

int main(int argc, char * argv[])
{
	RW_INSTANCE * instance;

	instance = rw_create_instance();
	if(!instance)
	{
		printf("Error creating instance!\n");
		return -1;
	}
	if(rw_initialize(instance, argc, argv))
	{
		if(instance->music_on)
		{
			t3f_play_music("data/title.xm");
		}
		al_play_sample(instance->sample[RW_SAMPLE_LOGO], 0.5, 0.5, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
		t3f_run();
	}
	else
	{
		printf("Error: could not initialize T3F!\n");
	}
	rw_exit(instance);
	rw_destroy_instance(instance);
	t3f_finish();
	return 0;
}
