/* cachesim_structs_methods.h */
/* David Tran */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

/* Define Structs and Functions */

typedef struct cache_row {
	char *tag_bits;
	int valid;
	int dirty;
	int LRU;
	char *cache_data;
	char *set_bits;
	char *offset_bits;
	char cache_address[24];
} cache_row;

typedef struct cache_set {
	cache_row **rows;
} cache_set;

typedef struct cache {
	int hits;
	int misses;
	int loads;
	int stores;

	int cache_size;
	int block_size;

	int set_size;
	char *set_assoc;
	int assoc_val;
	int num_sets;

	char *write_policy;
	cache_set **sets;
} cache;

typedef struct memory_block {
	int dec_address;			//address
	char *data_bits; 			//data: 2 characters (1 byte) * "block_size" per block
	struct memory_block *next_block; //linked-list of blocks
} memory_block;

typedef struct address {
	int no_tag_bits;
	int no_index_bits;
	int no_offest_bits;
	int DSetIndex;
	char *tag_bits;
	char *set_bits;
	char *offset_bits;
	char *loadOrStore;

	memory_block *blocks;
} address;


/* Calculation Methods */

char *hexToBinary(const char hex[]);
int binaryDecConversion(char *binary_str);
int calcTAG(address *main_memory);
int calcINDEX();
int calcOFFSET();
int calcTraceLines(FILE *file);
void concat(char* str, char c);
bool checkForMalloc(char* str);



/* Struct Initialization Methods */

void initCache();
cache_row* initLine();
void initSet(cache_set *set);
void initEmptyCache();
memory_block* initMemBlocks();
void initAddress(address *main_memory);
void initEmptyMemory();



/* Address Struct Modification Methods */

void writeBackMemory(address *main_memory, char* hexAddress, char* cache_data);
void insertMemBlock(address *main_memory, char* hexAddress, char* sw_data);
void fillMemBlock(address *main_memory, char* hexAddress, char* sw_data);
void printMem(address *main_memory);
void updateMem(address *main_memory, char* hexAddress, char* sw_data);
void destroyMemory();
void updateAddress (address *main_memory, char *address_bits);



/* Least-Recently-Used (LRU) Methods */

int findLRULocation(address *main_memory);
void updateLRU_MRU(address *main_memory);
void setMRU(address *main_memory, int MRUlocation);
void incrementLRU(address *main_memory, int MRULocation);



/* Write Back & Write-Through Methods */

void WT_evictLRU_insertMRU(address *main_memory, char* hexAddress, int access_size);
void writeThrough(address *main_memory, char* hexAddress, char* sw_data, int access_size);
void WB_removeLRU_insertMRU(address *main_memory, char* hexAddress, char* sw_data, int access_size);
void writeBack(address *main_memory, char* hexAddress, char* sw_data, int access_size);



/* Cache Modification Methods */
void destroyCache();
void updateCacheWithTrace(FILE *file, int num_lines);
char* loadIntoCacheBlockFromMemory(address *main_memory, char* hexAddress);
char* returnEmptyBlock();
char* getData(char* cache_data, char *hexAddress, int byteSize);
void writeCacheBlock(char* cache_data, char* hexAddress, char *sw_data);

