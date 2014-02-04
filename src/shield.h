#ifndef RW_SHIELD_H
#define RW_SHEILD_H

#define RW_MAX_SHIELDS 8
#define RW_SHIELD_MAX_LIFE 12

typedef struct
{
	
	int charge;
	int life;
	bool active;

} RW_SHIELD;

typedef struct
{
	
	RW_SHIELD shield[8];
	int power;
	
} RW_SHIELD_GENERATOR;

#endif
