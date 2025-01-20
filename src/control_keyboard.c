#include "instance.h"

int rw_keyboard_logic(RW_INSTANCE * ip)
{
	int key = 0;
	
	/* read keyboard input */
	key = t3f_get_char(0);
	if(key == 'q' || key == 'Q')
	{
		key = '7';
	}
	else if(key == 'w' || key == 'W')
	{
		key = '8';
	}
	else if(key == 'e' || key == 'E')
	{
		key = '9';
	}
	else if(key == 'a' || key == 'A')
	{
		key = '4';
	}
	else if(key == 'd' || key == 'D')
	{
		key = '6';
	}
	else if(key == 'z' || key == 'Z')
	{
		key = '1';
	}
	else if(key == '5' || key == 's' || key == 'S' || key == 'x' || key == 'X')
	{
		key = '2';
	}
	else if(key == 'c' || key == 'C')
	{
		key = '3';
	}
	return key;
}	
