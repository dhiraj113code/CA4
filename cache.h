#define TRUE 1
#define FALSE 0

/* default cache parameters--can be changed */
#define WORD_SIZE 4
#define WORD_SIZE_OFFSET 2
#define DEFAULT_CACHE_SIZE (8 * 1024)
#define DEFAULT_CACHE_BLOCK_SIZE 16
#define DEFAULT_CACHE_ASSOC 1
#define DEFAULT_CACHE_WRITEBACK TRUE
#define DEFAULT_CACHE_WRITEALLOC TRUE
#define DEFAULT_NUM_CORE 1

/* constants for settting cache parameters */
#define NUM_CORE 0
#define CACHE_PARAM_BLOCK_SIZE 1
#define CACHE_PARAM_USIZE 2
#define CACHE_PARAM_ASSOC 3
#define PARAM_DEBUG 4 

#define DATA_LOAD_REFERENCE 0
#define DATA_STORE_REFERENCE 1
#define INSTRUCTION_LOAD_REFERENCE 2

#define READ_REQUEST 0
#define WRITE_REQUEST 1

/* MESI protocol state */
#define INVALID_STATE 0
#define EXCLUSIVE_STATE 1
#define SHARED_STATE 2
#define MODIFIED_STATE 3

/* Type of state transition instigators */
#define READ_HIT 0
#define READ_MISS_FROM_BUS 1
#define READ_MISS_FROM_MEMORY 2
#define WRITE_HIT 3
#define WRITE_MISS_FROM_BUS 4
#define WRITE_MISS_FROM_MEMORY 5

/* Types of broadcasts */
#define REMOTE_READ_MISS 6 
#define REMOTE_WRITE_HIT 7
#define REMOTE_WRITE_MISS 8

#define TAG_MISS 0
#define TAG_HIT_VALID 1
#define TAG_HIT_INVALID 2

#define DEFAULT_DEBUG FALSE
#define MORE_STATS FALSE 

/* structure definitions */
typedef struct cache_line_ {
  unsigned tag;
  int state;

  struct cache_line_ *LRU_next;
  struct cache_line_ *LRU_prev;
} cache_line, *Pcache_line;

typedef struct cache_ {
  int id;                       /* core ID */
  int size;			/* cache size */
  int associativity;		/* cache associativity */
  int n_sets;			/* number of cache sets */
  unsigned index_mask;		/* mask to find cache index */
  int index_mask_offset;	/* number of zero bits in mask */
  Pcache_line *LRU_head;	/* head of LRU list for each set */
  Pcache_line *LRU_tail;	/* tail of LRU list for each set */
  int *set_contents;		/* number of valid entries in set */
} cache, *Pcache;

typedef struct cache_stat_ {
  int accesses;			/* number of memory references */
  int misses;			/* number of cache misses */
  int replacements;		/* number of misses that cause replacments */
  int demand_fetches;		/* number of fetches */
  int copies_back;		/* number of write backs */
  int fetches_into_cache;       /* number of fetches into cache */
  int broadcasts;               /* number of broadcasts */
  int read_requests;            /* number of read requests */
  int write_requests;           /* number of write requests */
} cache_stat, *Pcache_stat;


/* function prototypes */
void set_cache_param();
void init_cache();
void perform_access(unsigned addr, unsigned access_type, unsigned pid);
void flush();
void delete(Pcache_line *, Pcache_line *, Pcache_line);
void insert(Pcache_line *, Pcache_line *, Pcache_line);
void dump_settings();
void print_stats();


/* macros */
#define LOG2(x) ((int)( log((double)(x)) / log(2) ))

Pcache_line allocateCL(unsigned tag);
unsigned isReadorWrite(unsigned access_type, unsigned pid);
int BroadcastnSearch(unsigned tag, unsigned index, unsigned broadcast_type, unsigned pid);
void mesiST_Remote(Pcache_line c_line, unsigned whatHappened, unsigned pid);
void mesiST_Local(Pcache_line c_line, unsigned whatHappened);
int search(Pcache_line c, unsigned tag, Pcache_line *hitAt);
void BroadcastnSetState(unsigned request_type, unsigned tag, unsigned index, unsigned pid, Pcache_line c_line, int isHit);
void printCL(Pcache_line c_line);
void PrintCache(unsigned n_sets);
char stateSymbol(unsigned state);
void PrintLiveStats();
