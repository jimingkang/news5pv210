#include <stdio.h>

//#define DEBUG

#ifdef DEBUG
#define DBG(...) fprintf(stderr, " DBG(%s, %s(), %d): ", __FILE__, __FUNCTION__, __LINE__); fprintf(stderr, __VA_ARGS__)
#else
#define DBG(...)
#endif


int main(void)
{
	DBG("tiaoshi.\n");
	
	return 0;
}