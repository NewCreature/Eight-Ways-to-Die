#ifndef RW_CONTROL_HOTSPOT_H
#define RW_CONTROL_HOTSPOT_H

#include "instance.h"

void rw_render_hover_text(ALLEGRO_FONT * fp, float x, float y, int flags, bool highlight, const char * text);
int rw_hotspot_logic(RW_INSTANCE * ip);
void rw_render_hot_spots(RW_INSTANCE * ip);

#endif
