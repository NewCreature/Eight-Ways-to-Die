#ifndef RW_THREAT_H
#define RW_THREAT_H

#define RW_MAX_THREATS  128

#define RW_THREAT_BASIC   0

typedef struct
{
	
	float x;
	float y;
	float vx;
	float vy;
	float angle;
	float vangle;
	
	bool active;
	int type;
	
} RW_THREAT;

#endif
