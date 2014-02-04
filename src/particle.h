#ifndef RW_PARTICLE_H
#define RW_PARTICLE_H

#define RW_MAX_PARTICLES 256

typedef struct
{
	
	float x;
	float y;
	float vx;
	float vy;
	float angle;
	
	bool active;
	int life;
	int tlife;
	int type;
	
} RW_PARTICLE;

#endif
