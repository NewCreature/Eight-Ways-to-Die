#ifndef _RW_TEXT_H
#define _RW_TEXT_H

#include "t3f/t3f.h"

void rw_draw_monospace_text(T3F_FONT * fp, ALLEGRO_COLOR color, float x, float y, int flags, const char * text, const char * space_text);
void rw_draw_time_text(T3F_FONT * fp, ALLEGRO_COLOR color, float x, float y, int flags, int ticks);

#endif
