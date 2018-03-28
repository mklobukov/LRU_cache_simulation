#ifndef _CACHE_
#define _CACHE_

#include <inttypes.h>

#define MAX_NUM_SETS 1024
#define MAX_LINES_IN_SET 4

typedef struct cache_way_t {
  uint64_t tag;
  unsigned int valid_bit;
}CACHE_WAY;

typedef struct cache_t{
 CACHE_WAY cache[MAX_NUM_SETS][MAX_LINES_IN_SET];
 unsigned int size;
 unsigned int associativity;
 unsigned int line_size;
 unsigned int LRU[MAX_NUM_SETS][4][4];
 unsigned int num_sets;

}CACHE;


#endif
