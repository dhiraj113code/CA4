#include <stdio.h>
#include <math.h>

#include "cache.h"
#include "main.h"

/* cache configuration parameters */
static int cache_usize = DEFAULT_CACHE_SIZE;
static int cache_block_size = DEFAULT_CACHE_BLOCK_SIZE;
static int words_per_block = DEFAULT_CACHE_BLOCK_SIZE / WORD_SIZE;
static int cache_assoc = DEFAULT_CACHE_ASSOC;
static int cache_writeback = DEFAULT_CACHE_WRITEBACK;
static int cache_writealloc = DEFAULT_CACHE_WRITEALLOC;
static int num_core = DEFAULT_NUM_CORE;

/* cache model data structures */
/* max of 8 cores */
static cache mesi_cache[8];
static cache_stat mesi_cache_stat[8];

/************************************************************/
void set_cache_param(param, value)
  int param;
  int value;
{
  switch (param) {
  case NUM_CORE:
    num_core = value;
    break;
  case CACHE_PARAM_BLOCK_SIZE:
    cache_block_size = value;
    words_per_block = value / WORD_SIZE;
    break;
  case CACHE_PARAM_USIZE:
    cache_usize = value;
    break;
  case CACHE_PARAM_ASSOC:
    cache_assoc = value;
    break;
  default:
    printf("error set_cache_param: bad parameter value\n");
    exit(-1);
  }
}
/************************************************************/

/************************************************************/
void init_cache()
{
  /* initialize the cache, and cache statistics data structures */

  //Initialize the caches - depending on the number of cores present
  //All core caches are identical
  int n_blocks, n_sets, mask_size, block_offset, mask, i, j;

  n_blocks = cache_usize/cache_block_size;
  n_sets = n_blocks/cache_assoc;
  block_offset = LOG2(cache_block_size);
  mask_size = n_sets + block_offset;
  mask = (1<<mask_size) - 1;
  for(i = 0; i < num_core; i++)
  {
     mesi_cache[i].id = i;
     mesi_cache[i].size = cache_usize;
     mesi_cache[i].associativity = cache_assoc;
     mesi_cache[i].n_sets = n_sets;
     mesi_cache[i].index_mask = mask;
     mesi_cache[i].index_mask_offset = block_offset;
  }

  //Dynamically allocating memory for LRU head, LRU tail and contents arrays
  for(i = 0; i < num_core; i++)
  {
     mesi_cache[i].LRU_head = (Pcache_line*)malloc(sizeof(Pcache_line)*mesi_cache[i].n_sets);
     mesi_cache[i].LRU_tail = (Pcache_line*)malloc(sizeof(Pcache_line)*mesi_cache[i].n_sets);
     mesi_cache[i].set_contents = (int*)malloc(sizeof(int)*mesi_cache[i].n_sets);
  }

  //Checking if memory is allocated properly or not
  for(i = 0; i < num_core; i++)
  {
     if(mesi_cache[i].LRU_head == NULL || mesi_cache[i].LRU_tail == NULL || mesi_cache[i].set_contents == NULL)
        {printf("error : Memory allocation failed for mesi_cache[%d] LRU_head, LRU_tail\n", i); exit(-1);}

  }

  //Initializing set_contents
  for(i = 0; i < num_core; i++)
  {
     for(j = 0; j < mesi_cache[i].n_sets; j++)
     {
        mesi_cache[i].set_contents[j] = 0;
        mesi_cache[i].LRU_head[j] = (Pcache_line)NULL;
        mesi_cache[i].LRU_tail[j] = (Pcache_line)NULL;
     }
  }
}
/************************************************************/

/************************************************************/
void perform_access(addr, access_type, pid)
     unsigned addr, access_type, pid;
{
  /* handle accesses to the mesi caches */
}
/************************************************************/

/************************************************************/
void flush()
{
  /* flush the mesi caches */
}
/************************************************************/

/************************************************************/
void delete(head, tail, item)
  Pcache_line *head, *tail;
  Pcache_line item;
{
  if (item->LRU_prev) {
    item->LRU_prev->LRU_next = item->LRU_next;
  } else {
    /* item at head */
    *head = item->LRU_next;
  }

  if (item->LRU_next) {
    item->LRU_next->LRU_prev = item->LRU_prev;
  } else {
    /* item at tail */
    *tail = item->LRU_prev;
  }
}
/************************************************************/

/************************************************************/
/* inserts at the head of the list */
void insert(head, tail, item)
  Pcache_line *head, *tail;
  Pcache_line item;
{
  item->LRU_next = *head;
  item->LRU_prev = (Pcache_line)NULL;

  if (item->LRU_next)
    item->LRU_next->LRU_prev = item;
  else
    *tail = item;

  *head = item;
}
/************************************************************/

/************************************************************/
void dump_settings()
{
  printf("Cache Settings:\n");
  printf("\tSize: \t%d\n", cache_usize);
  printf("\tAssociativity: \t%d\n", cache_assoc);
  printf("\tBlock size: \t%d\n", cache_block_size);
}
/************************************************************/

/************************************************************/
void print_stats()
{
  int i;
  int demand_fetches = 0;
  int copies_back = 0;
  int broadcasts = 0;

  printf("*** CACHE STATISTICS ***\n");

  for (i = 0; i < num_core; i++) {
    printf("  CORE %d\n", i);
    printf("  accesses:  %d\n", mesi_cache_stat[i].accesses);
    printf("  misses:    %d\n", mesi_cache_stat[i].misses);
    printf("  miss rate: %f (%f)\n", 
	   (float)mesi_cache_stat[i].misses / (float)mesi_cache_stat[i].accesses,
	   1.0 - (float)mesi_cache_stat[i].misses / (float)mesi_cache_stat[i].accesses);
    printf("  replace:   %d\n", mesi_cache_stat[i].replacements);
  }

  printf("\n");
  printf("  TRAFFIC\n");
  for (i = 0; i < num_core; i++) {
    demand_fetches += mesi_cache_stat[i].demand_fetches;
    copies_back += mesi_cache_stat[i].copies_back;
    broadcasts += mesi_cache_stat[i].broadcasts;
  }
  printf("  demand fetch (words): %d\n", demand_fetches);
  /* number of broadcasts */
  printf("  broadcasts:           %d\n", broadcasts);
  printf("  copies back (words):  %d\n", copies_back);
}
/************************************************************/
