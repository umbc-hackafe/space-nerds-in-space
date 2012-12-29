#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>

#include "snis_font.h"
#include "snis_typeface.h"
#include "snis_graph.h"

#define DEFINE_SNIS_TEXT_INPUT_GLOBALS
#include "snis_text_input.h"

struct snis_text_input_box {
	int x, y;
	int color, font;
	char *buffer;
	int buflen;
	snis_text_input_box_callback callback;
	void *cookie;
	int cursor_pos;
	volatile int *timer;
	int height, width;
	int has_focus;
};

struct snis_text_input_box *snis_text_input_box_init(int x, int y,
					int height, int width,
					int color, int font,
					char *buffer, int buflen,
					volatile int *timer,
					snis_text_input_box_callback callback,
					void *cookie)
{
	struct snis_text_input_box *t;

	t = malloc(sizeof(*t));
	t->x = x;
	t->y = y;
	t->color = color;
	t->font = font;
	t->buffer = buffer;
	t->buflen = buflen - 1;
	t->cursor_pos = strlen(t->buffer);
	t->timer = timer;
	t->callback = callback;
	t->cookie = cookie;
	t->height = height;
	t->width = width;
	t->has_focus = 0;
	return t;
}

void snis_text_input_box_draw(GtkWidget *w, GdkGC *gc, struct snis_text_input_box *t)
{
	int cursor_on = t->has_focus && (*t->timer & 0x04);

	sng_set_foreground(t->color);	
	sng_current_draw_rectangle(w->window, gc, 0, t->x, t->y, t->width, t->height);
	if (t->has_focus)
		sng_current_draw_rectangle(w->window, gc, 0, t->x - 1, t->y - 1,
						t->width + 2, t->height + 2);

	sng_abs_xy_draw_string_with_cursor(w, gc, t->buffer, t->font,
				t->x + 4, t->y + font_lineheight[t->font],
				t->cursor_pos, cursor_on);
}

void snis_text_input_box_set_focus(struct snis_text_input_box *t, int has_focus)
{
	t->has_focus = has_focus;
}

int snis_text_input_box_button_press(struct snis_text_input_box *t, int x, int y)
{
	int hit;

	if (x < t->x || x > t->x + t->width ||
                y < t->y || y > t->y + t->height)
                hit = 0;
	else
		hit = 1;
	/* put stuff for cursor positioning here. */
	return hit;
}

int snis_text_input_box_keypress(struct snis_text_input_box *t, GdkEventKey *event)
{
	char c;
	int currentlen;
	if (event->type != GDK_KEY_PRESS)
		return 0;
	if ((event->keyval & ~0x7f) != 0)
		return 0;
	c = (event->keyval & 0x7f);
	currentlen = strlen(t->buffer);
	if (currentlen == t->cursor_pos)
		t->buffer[t->cursor_pos + 1] = '\0';	
	t->buffer[t->cursor_pos] = c;
	if (t->cursor_pos < t->buflen - 1)
		t->cursor_pos++;
	return 0;
}

int snis_text_input_box_keyrelease(struct snis_text_input_box *t, GdkEventKey *event)
{
	return 0;
}

