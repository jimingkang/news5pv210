#include <types.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <charset.h>
#include <s5pv210-serial.h>
#include <readline.h>

s32_t cx = 0, cy = 0;

enum tty_state_t {
	TTY_STATE_NORMAL,
	TTY_STATE_ESC,
	TTY_STATE_CSI,
};

enum {
	UNICODE_BS				= 0x0008,
	UNICODE_TAB				= 0x0009,
	UNICODE_LF				= 0x000a,
	UNICODE_CR				= 0x000d,
	UNICODE_SPACE			= 0x0020,

	UNICODE_HLINE			= 0x2500,
	UNICODE_VLINE			= 0x2502,

	UNICODE_LEFTTOP			= 0x250c,
	UNICODE_RIGHTTOP		= 0x2510,
	UNICODE_LEFTBOTTOM		= 0x2514,
	UNICODE_RIGHTBOTTOM		= 0x2518,

	UNICODE_UP				= 0x25b2,
	UNICODE_RIGHT			= 0x25b6,
	UNICODE_DOWN			= 0x25bc,
	UNICODE_LEFT			= 0x25c0,
};

struct rl_buf {
	int ch;
	u32_t * buf;
	u32_t * cut;
	size_t size;
	size_t len;
	size_t pos;
	s32_t x, y;
	s32_t w, h;

	/*
	 * below for private data
	 */
	enum tty_state_t state;
	s32_t params[8];
	s32_t num_params;

	s8_t utf8[32];
	s32_t usize;
};

static s32_t ucs4_strlen(const u32_t * s)
{
	const u32_t * sc;

	if(!s)
		return 0;

	for(sc = s; *sc != '\0'; ++sc);
	return sc - s;
}

static bool_t console_getcode(struct rl_buf * rl, u32_t * code)
{
	s8_t c;
	s32_t i;
	u32_t cp;
	s8_t * rest;

	if (s5pv210_serial_read(rl->ch, (u8_t *)&c, 1) != 1)
		return FALSE;

	switch(rl->state)
	{
	case TTY_STATE_NORMAL:
		switch(c)
		{
		case 27:
			rl->state = TTY_STATE_ESC;
			break;

		case 127:				/* backspace */
			*code = 0x8;		/* ctrl-h */
			return TRUE;

		default:
			rl->utf8[rl->usize++] = c;
			if(utf8_to_ucs4(&cp, 1, (const char *)rl->utf8, rl->usize, (const char **)&rest) > 0)
			{
				rl->usize -= rest - rl->utf8;
				memmove(rl->utf8, rest, rl->usize);

				*code = cp;
				return TRUE;
			}
			break;
		}
		break;

	case TTY_STATE_ESC:
		if(c == '[')
		{
			for(i = 0; i < ARRAY_SIZE(rl->params); i++)
				rl->params[i] = 0;
			rl->num_params = 0;
			rl->state = TTY_STATE_CSI;
		}
		else
		{
			rl->state = TTY_STATE_NORMAL;
		}
		break;

	case TTY_STATE_CSI:
		if(c >= '0' && c <= '9')
		{
			if(rl->num_params < ARRAY_SIZE(rl->params))
			{
				rl->params[rl->num_params] = rl->params[rl->num_params] * 10 + c - '0';
			}
		}
		else
		{
			rl->num_params++;
			if(c == ';')
				break;

			rl->state = TTY_STATE_NORMAL;
			switch(c)
			{
			case 'A':				/* arrow up */
				*code = 0x10;		/* ctrl-p */
				return TRUE;

			case 'B':				/* arrow down */
				*code = 0xe			/* ctrl-n */;
				return TRUE;

			case 'C':				/* arrow right */
				*code = 0x6;		/* ctrl-f */
				return TRUE;

			case 'D':				/* arrow left */
				*code = 0x2;		/* ctrl-b */
				return TRUE;

			case '~':
				if(rl->num_params != 1)
					break;

				switch(rl->params[0])
				{
				case 1:				/* home */
					*code = 0x1;	/* ctrl-a */
					return TRUE;

				case 2:				/* insert */
					break;

				case 3:				/* delete */
					*code = 0x8;	/* ctrl-h */
					return TRUE;

				case 4:				/* end */
					*code = 0x5;	/* ctrl-e */
					return TRUE;

				case 5:				/* page up*/
					*code = 0x10;	/* ctrl-p */
					return TRUE;

				case 6:				/* page down*/
					*code = 0xe;	/* ctrl-n */
					return TRUE;

				default:
					break;
				}
				break;

			default:
				break;
			}
		}
		break;

	default:
		rl->state = TTY_STATE_NORMAL;
		break;
	}

	return FALSE;
}

static bool_t console_putcode(int ch, u32_t code)
{
	char buf[32];
	s32_t w, i;

	w = ucs4_width(code);
	if(w < 0)
		w = 0;

	switch(code)
	{
	case UNICODE_BS:
		return TRUE;

	case UNICODE_TAB:
		i = 8 - (cx % 8);
		if(i + cx >= 80)
			i = 80 - cx - 1;
		cx = cx + i;
		break;

	case UNICODE_LF:
		if(cy + 1 < 24)
			cy = cy + 1;
		break;

	case UNICODE_CR:
		cx = 0;
		break;

	default:
		if(cx + w < 80)
			cx = cx + w;
		else
		{
			if(cy + 1 < 24)
				cy = cy + 1;
			cx = 0;
		}
		break;
	}

	ucs4_to_utf8(&code, 1, buf, sizeof(buf));
	s5pv210_serial_write_string(ch, buf);

	return TRUE;
}

int console_print(int ch, const char * fmt, ...)
{
	va_list ap;
	u32_t code;
	char *p, *buf;
	int len;

	va_start(ap, fmt);
	len = vsnprintf(NULL, 0, fmt, ap);
	if(len < 0)
		return 0;
	buf = malloc(len + 1);
	if(!buf)
		return 0;
	len = vsnprintf(buf, len + 1, fmt, ap);
	va_end(ap);

	for(p = buf; utf8_to_ucs4(&code, 1, p, -1, (const char **)&p) > 0; )
	{
		console_putcode(ch, code);
	}

	free(buf);
	return len;
}

static void console_gotoxy(struct rl_buf * rl, int x, int y)
{
	char buf[32];

	sprintf(buf, (const char *)"\033[%d;%dH", y + 1, x + 1);
	s5pv210_serial_write_string(rl->ch, buf);

	cx = x;
	cy = y;
}

static void rl_gotoxy(struct rl_buf * rl)
{
	s32_t x, y;
	s32_t pos, i, w;

	for(i = 0, pos = 0; i < rl->pos; i++)
	{
		w = ucs4_width(rl->buf[i]);
		if(w < 0)
			w = 0;
		pos += w;
	}

	x = ((rl->y * rl->w) + rl->x + pos) % (rl->w);
	y = ((rl->y * rl->w) + rl->x + pos) / (rl->w);

	console_gotoxy(rl, x, y);
}

static void rl_space(struct rl_buf * rl, u32_t len)
{
	s32_t i;

	for(i = 0; i < len; i++)
		console_print(rl->ch, " ");
}

static void rl_print(struct rl_buf * rl, s32_t pos)
{
	char * utf8;

	if(pos < 0)
		return;

	if(pos > rl->len)
		return;

	utf8 = ucs4_to_utf8_alloc(rl->buf + pos, rl->len - pos);
	console_print(rl->ch, utf8);

	free(utf8);
}

static struct rl_buf * rl_buf_alloc(int ch, const char * prompt)
{
	struct rl_buf * rl;

	rl = malloc(sizeof(struct rl_buf));
	if(!rl)
		return NULL;

	rl->ch = ch;

	rl->size = 256;
	rl->len = 0;
	rl->pos = 0;
	rl->cut = NULL;

	rl->x = cx;
	rl->y = cy;
	rl->w = 80;
	rl->h = 24;

	rl->state = TTY_STATE_NORMAL;
	rl->num_params = 0;
	rl->usize = 0;

	rl->buf = malloc(sizeof(u32_t) * rl->size);
	if(!rl->buf)
	{
		free(rl);
		return NULL;
	}

	if(prompt)
	{
		console_print(rl->ch, prompt);
		rl->x = cx;
		rl->y = cy;
	}

	return rl;
}

static void rl_buf_free(struct rl_buf * rl)
{
	if(rl->cut)
		free(rl->cut);

	free(rl->buf);
	free(rl);
}

static void rl_cursor_home(struct rl_buf * rl)
{
	if(rl->pos != 0)
	{
		rl->pos = 0;
		rl_gotoxy(rl);
	}
}

static void rl_cursor_end(struct rl_buf * rl)
{
	if(rl->pos != rl->len)
	{
		rl->pos = rl->len;
		rl_gotoxy(rl);
	}
}

static void rl_cursor_left(struct rl_buf * rl)
{
	if(rl->pos > 0)
	{
		rl->pos = rl->pos - 1;
		rl_gotoxy(rl);
	}
}

static void rl_cursor_right(struct rl_buf * rl)
{
	if(rl->pos < rl->len)
	{
		rl->pos = rl->pos + 1;
		rl_gotoxy(rl);
	}
}

static void rl_insert(struct rl_buf * rl, u32_t * str)
{
	u32_t * p;
	s32_t len = ucs4_strlen(str);

	if(len <= 0)
		return;

	if(len + rl->len >= rl->size)
	{
		p = realloc(rl->buf, sizeof(u32_t) * rl->size * 2);
		if(!p)
			return;
		rl->size = rl->size * 2;
		rl->buf = p;
	}

	if(len + rl->len < rl->size)
	{
		memmove(rl->buf + rl->pos + len, rl->buf + rl->pos, (rl->len - rl->pos + 1) * sizeof(u32_t));
		memmove (rl->buf + rl->pos, str, len * sizeof(u32_t));

		rl->len = rl->len + len;
		rl_print(rl, rl->pos);
		rl->pos = rl->pos + len;
		rl_gotoxy(rl);
	}
}

static void rl_delete(struct rl_buf * rl, u32_t len)
{
	s32_t n, i, w;

	if(len > rl->len - rl->pos)
		len = rl->len - rl->pos;

	if(rl->pos + len <= rl->len)
	{
		for(i = 0, n = 0; i < len; i++)
		{
			w = ucs4_width(rl->buf[rl->pos + i]);
			if(w < 0)
				w = 0;
			n += w;
		}

		if(rl->cut)
			free(rl->cut);

		rl->cut = malloc((rl->len - rl->pos + 1) * sizeof(u32_t));
		if(rl->cut)
		{
			memcpy(rl->cut, rl->buf + rl->pos, (rl->len - rl->pos + 1) * sizeof(u32_t));
			rl->cut[rl->len - rl->pos] = '\0';
		}

		memmove(rl->buf + rl->pos, rl->buf + rl->pos + len, sizeof(u32_t) * (rl->len - rl->pos + 1));
		rl->len = rl->len - len;
		rl_print(rl, rl->pos);
		rl_space(rl, n);
		rl_gotoxy(rl);
	}
}

static bool_t readline_handle(struct rl_buf * rl, u32_t code)
{
    u32_t tmp[2];
    u32_t n;

	switch(code)
	{
	case 0x0:	/* null */
		break;

	case 0x1:	/* ctrl-a: to the start of the line */
		rl_cursor_home(rl);
		break;

	case 0x2:	/* ctrl-b: to the left */
		rl_cursor_left(rl);
		break;

	case 0x3:	/* ctrl-c: break the readline */
		rl_cursor_home(rl);
		rl_delete(rl, rl->len);
		return TRUE;

	case 0x4:	/* ctrl-d: delete the character underneath the cursor */
		if(rl->pos < rl->len)
			rl_delete(rl, 1);
		break;

	case 0x5:	/* ctrl-e: to the end of the line */
		rl_cursor_end(rl);
		break;

	case 0x6:	/* ctrl-f: to the right */
		rl_cursor_right(rl);
		break;

	case 0x7:	/* ctrl-g */
		break;

	case 0x8:	/* ctrl-h: backspace */
		if(rl->pos > 0)
		{
			rl_cursor_left(rl);
			rl_delete(rl, 1);
		}
		break;

	case 0x9: 	/* ctrl-i: tab */
		break;

	case 0xb: 	/* ctrl-k: delete everything from the cursor to the end of the line */
		if(rl->pos < rl->len)
			rl_delete(rl, rl->len - rl->pos);
		break;

	case 0xc: 	/* ctrl-l */
		break;

	case 0xa:	/* ctrl-j: lf */
	case 0xd: 	/* ctrl-m: cr */
		return TRUE;

	case 0xe:	/* ctrl-n: the next history */
		rl_cursor_home(rl);
		rl_delete(rl, rl->len);
		break;

	case 0xf: 	/* ctrl-o */
		break;

	case 0x10:	/* ctrl-p: the previous history */
		break;

	case 0x11: 	/* ctrl-q */
		break;

	case 0x12:	/* ctrl-r */
		break;

	case 0x13: 	/* ctrl-s */
		break;

	case 0x14: 	/* ctrl-t */
		break;

	case 0x15: 	/* ctrl-u: delete everything from the cursor back to the line start */
		if(rl->pos > 0)
		{
			n = rl->pos;
			rl_cursor_home(rl);
			rl_delete(rl, n);
		}
		break;

	case 0x16:	/* ctrl-v */
		break;

	case 0x17: 	/* ctrl-w */
		break;

	case 0x18: 	/* ctrl-x */
		break;

	case 0x19:	/* ctrl-y: paste the killed text at the cursor position */
		if(rl->cut)
			rl_insert(rl, rl->cut);
		break;

	case 0x1a: 	/* ctrl-z */
		break;

	default:
	      tmp[0] = code;
	      tmp[1] = '\0';
	      rl_insert(rl, tmp);
		break;
	}

	return FALSE;
}

/*
 * read line with utf-8 stream
 */
char * readline(int ch, const char * prompt)
{
	struct rl_buf * rl;
	char * utf8 = NULL;
	u32_t code;

	rl = rl_buf_alloc(ch, prompt);
	if(!rl)
		return NULL;

	for(;;)
	{
		if(console_getcode(rl, &code))
		{
			if(readline_handle(rl, code))
			{
				console_print(rl->ch, "\r\n");
				break;
			}
		}
	}

	if(rl->len > 0)
		utf8 = ucs4_to_utf8_alloc(rl->buf, rl->len);

	rl_buf_free(rl);
	return utf8;
}
