#include "t3f/t3f.h"

void rw_draw_monospace_text(T3F_FONT * fp, ALLEGRO_COLOR color, float x, float y, int flags, const char * space_text, const char * text)
{
  int i;
  float pos = x;
  float dx = t3f_get_text_width(fp, space_text);
  char buf[2] = {0};

  if(flags & T3F_FONT_ALIGN_CENTER)
  {
    pos -= (dx * strlen(text)) / 2.0;
  }
  else if(flags & T3F_FONT_ALIGN_RIGHT)
  {
    pos -= dx * strlen(text);
  }
  for(i = 0; i < strlen(text); i++)
  {
    buf[0] = text[i];
    t3f_draw_text(fp, color, pos, y, 0, flags, buf);
    pos += dx;
  }
}

void rw_draw_time_text(T3F_FONT * fp, ALLEGRO_COLOR color, float x, float y, int flags, int ticks)
{
  int minutes;
  int seconds;
  int hundredths;
  float minutes_pos;
  float seconds_pos;
  float hundredths_pos;
  char buf[64];

  minutes = ticks / 3600;
  seconds = (ticks / 60) % 60;
  hundredths = (int)(((float)(ticks % 60) / 60.0) * 100.0) % 100;
  minutes_pos = x;
  sprintf(buf, "%02d:%02d:%02d", minutes, seconds, hundredths);
  rw_draw_monospace_text(fp, color, x, y, flags, "0", buf);
}
