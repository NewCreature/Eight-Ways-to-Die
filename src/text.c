#include "t3f/t3f.h"

void rw_draw_monospace_text(T3F_FONT * fp, ALLEGRO_COLOR color, float x, float y, int flags, const char * space_text, const char * text)
{
  int i, j;
  float pos = x;
  float text_width = 0.0;
  float dx = t3f_get_text_width(fp, space_text);
  float mid = dx / 2.0;
  char buf[64] = {0};
  char char_buf[2] = {0};
  char * current_text[64] = {NULL};
  int current_text_count = 0;

  if(strlen(text) < 64)
  {
    /* split up text */
    strcpy(buf, text);
    current_text[current_text_count] = strtok(buf, "\t");
    while(current_text[current_text_count])
    {
      current_text_count++;
      current_text[current_text_count] = strtok(NULL, "\t");
    }

    /* figure out start position */
    for(i = 0; i < current_text_count; i++)
    {
      if(i % 2 == 0)
      {
        text_width += t3f_get_text_width(fp, current_text[i]);
      }
      else
      {
        text_width += (float)strlen(current_text[i]) * dx;
      }
    }
    if(flags & T3F_FONT_ALIGN_CENTER)
    {
      pos -= text_width / 2.0;
    }
    else if(flags & T3F_FONT_ALIGN_RIGHT)
    {
      pos -= text_width;
    }

    /* draw text */
    for(i = 0; i < current_text_count; i++)
    {
      if(i % 2 == 0)
      {
        t3f_draw_text(fp, color, pos, y, 0, 0, current_text[i]);
        pos += t3f_get_text_width(fp, current_text[i]);
      }
      else
      {
        for(j = 0; j < strlen(current_text[i]); j++)
        {
          char_buf[0] = current_text[i][j];
          t3f_draw_text(fp, color, pos + mid, y, 0, T3F_FONT_ALIGN_CENTER, char_buf);
          pos += dx;
        }
      }
    }
  }
}

void rw_draw_time_text(T3F_FONT * fp, ALLEGRO_COLOR color, float x, float y, int flags, const char * pre_text, int ticks)
{
  int minutes;
  int seconds;
  int hundredths;
  char buf[64];

  minutes = ticks / 3600;
  seconds = (ticks / 60) % 60;
  hundredths = (int)(((float)(ticks % 60) / 60.0) * 100.0) % 100;
  sprintf(buf, "%s\t%d\t:\t%02d\t:\t%02d", pre_text, minutes, seconds, hundredths);
  rw_draw_monospace_text(fp, color, x, y, flags, "0", buf);
}

void rw_draw_level_text(T3F_FONT * fp, ALLEGRO_COLOR color, float x, float y, int flags, const char * pre_text, int level)
{
  char buf[64];

  sprintf(buf, "%s\t%d", pre_text, level);
  rw_draw_monospace_text(fp, color, x, y, flags, "0", buf);
}
