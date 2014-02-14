#ifndef RW_INSTANCE_H
#define RW_INSTANCE_H

#include "t3f/rng.h"
#include "shield.h"
#include "threat.h"
#include "ship.h"
#include "particle.h"
#include "defines.h"

typedef struct
{
	/* intro data */
	float intro_z;
	float intro_planet_z;
	float intro_planet_angle;
	float intro_planet_vangle;
	int   intro_state;
	int   intro_ticker;
	float start_alpha;
	float logo_z;

	/* game data */
	T3F_RNG_STATE rng_state;
	int ticks;
	int quit;
	ALLEGRO_BITMAP * bitmap[RW_MAX_BITMAPS];
	ALLEGRO_FONT * font;
	ALLEGRO_SAMPLE * sample[RW_MAX_SAMPLES];
	T3F_ATLAS * atlas;
	int game_mode;
	int control_mode;

	/* swipe data */
	int swipe_state;
	int swipe_touch;
	int swipe_x;
	int swipe_y;
	int score;
	int high_score;
	bool new_high_score;
	int threat_count;
	int level;
	int damage;
	int damage_time;
	int alert_ticks;
	int alert_audio_ticks;
	float camera_z;
	float camera_target_z;
	int state;
	bool music_on;
	float vertical_scale;
	float third;

	RW_SHIELD_GENERATOR shield_generator;
	RW_THREAT threat[RW_MAX_THREATS];
	RW_SHIP ship[RW_MAX_SHIPS];
	RW_SHOT shot[RW_MAX_SHOTS];
	RW_PARTICLE particle[RW_MAX_PARTICLES];
} RW_INSTANCE;

RW_INSTANCE * rw_create_instance(void);
void rw_destroy_instance(RW_INSTANCE * ip);

#endif
