#include "instance.h"

int rw_save_high_score(RW_INSTANCE * ip, const char * fn)
{
	ALLEGRO_FILE * fp;
	const ALLEGRO_FILE_INTERFACE * old_interface;
	
	old_interface = al_get_new_file_interface();
	al_set_standard_file_interface();
	
	fp = al_fopen(fn, "wb");
	if(!fp)
	{
		return 0;
	}
	al_fwrite32le(fp, ip->high_score);
	al_fputc(fp, ip->music_on);
	al_fclose(fp);
	al_set_new_file_interface(old_interface);
	return 1;
}

int rw_load_high_score(RW_INSTANCE * ip, const char * fn)
{
	ALLEGRO_FILE * fp;
	const ALLEGRO_FILE_INTERFACE * old_interface;
	
	old_interface = al_get_new_file_interface();
	al_set_standard_file_interface();
	
	fp = al_fopen(fn, "rb");
	if(!fp)
	{
		return 0;
	}
	ip->high_score = al_fread32le(fp);
	ip->music_on = al_fgetc(fp);
	al_fclose(fp);
	al_set_new_file_interface(old_interface);
	return 1;
}
