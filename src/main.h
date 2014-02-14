#ifndef RW_MAIN_H
#define RW_MAIN_H

void rw_toggle_music(RW_INSTANCE * ip);

void rw_event_handler(ALLEGRO_EVENT * event, void * data);
void rw_logic(void * data);
void rw_render(void * data);

#endif
