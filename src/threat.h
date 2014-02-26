#ifndef RW_THREAT_H
#define RW_THREAT_H

#define RW_MAX_THREATS  128

#define RW_THREAT_BASIC   0
#define RW_THREAT_LARGE   1

typedef struct
{
	
	float gen_angle;
	float gen_speed;

	float x;
	float y;
	float vx;
	float vy;
	float angle;
	float vangle;
	
	bool active;
	int type;
	int pos;
	int size;
	
} RW_THREAT;

#endif
