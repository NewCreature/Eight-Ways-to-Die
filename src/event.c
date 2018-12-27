#include "instance.h"

void rw_event_handler(ALLEGRO_EVENT * event, void * data)
{
	RW_INSTANCE * ip = (RW_INSTANCE *)data;
	switch(event->type)
	{
		case ALLEGRO_EVENT_DISPLAY_CLOSE:
		{
			if(ip->state == RW_STATE_TITLE)
			{
				ip->state = RW_STATE_EXIT;
			}
			else
			{
				t3f_event_handler(event);
			}
			break;
		}
		case ALLEGRO_EVENT_DISPLAY_RESIZE:
		{
			t3f_event_handler(event);
			ip->vertical_scale = (t3f_default_view->bottom - t3f_default_view->top) / 480.0;
			ip->third = (t3f_default_view->bottom - t3f_default_view->top) / 3.0;
		}
		/* pass the event through to T3F for handling by default */
		default:
		{
			t3f_event_handler(event);
			break;
		}
	}
}
