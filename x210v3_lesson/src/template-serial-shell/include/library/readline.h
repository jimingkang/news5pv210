#ifndef __READLINE_H__
#define __READLINE_H__

#ifdef __cplusplus
extern "C" {
#endif

char * readline(int ch, const char * prompt);
int console_print(int ch, const char * fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* __READLINE_H__ */
