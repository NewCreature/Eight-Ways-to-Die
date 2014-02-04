#ifndef RW_SHIP_H
#define RW_SHIP_H

#define RW_MAX_SHIPS  2
#define RW_MAX_SHOTS 16

typedef struct
{
	
	float x;
	float y;
	float angle;
	float vangle;
	float dist;
	float vdist;
	float dest;
	int way;
	
	bool active;
	int ticks;
	
} RW_SHIP;

typedef struct
{
	
	float x;
	float y;
	float vx;
	float vy;
	
	bool active;
	int type;
	
} RW_SHOT;

#endif
