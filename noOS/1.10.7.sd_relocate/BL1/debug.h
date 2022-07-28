#ifndef BARE_DEBUG_H
#define BARE_DEBUG_H

#ifdef __DEBUG__

extern int printf(const char *,...);

#define INFO(format,...) printf("[INFO]:"format,##__VA_ARGS__)

#define ERROR(format,arg...) printf("[ERROR]:"format,##arg)

#else
#define INFO(arg,...) 

#define ERROR(format,arg...) 

#endif

#endif
