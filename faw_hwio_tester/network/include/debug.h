#ifndef __DEBUG_H
#define __DEBUG_H

#define DEBUG_MODE 1

#if (defined DEBUG_MODE) && (DEBUG_MODE == 1)
#define dprintf(...) \
        {printf(__VA_ARGS__);}
#else
#define dprintf(...) do {} while(0)
#endif

#endif