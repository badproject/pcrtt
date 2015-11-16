#ifndef BUILD_VELVET_GRAPH_EXTERNAL
#define BUILD_VELVEL_GRAPH_EXTERNAL 1
#include "ExternalSort.h"
/*BUFFER SETTINGS FOR CREATING RUNS*/
#define KEYS_IN_RUN (8192*OS_PAGE_SIZE)
#define RUNS_PER_MERGE 16 
#define C_SIZE_MIN 128 
/*SIZE OF THE TOTAL MERGE BUFFER IS AS FOLLOWS
 *
 * (RUNS_PER_MERGE*C_SIZE_MIN*OS_PAGE_SIZE)*(key_size)
 *
 * IN EACH I/O THE ALGORITHM READS 
 * 
 * (C_SIZE_MIN*OS_PAGE_SIZE)*(key_size) bytes
 **/

/*BUFFER SETTING FOR MERGING*/
typedef struct eparams {
	unsigned char col_start; /*These are actual indicies*/
	unsigned char col_end; 
	unsigned char endian;
}KmerEdgeParams;
extern KmerEdgeParams ESortParams;
unsigned char CompareKmerEdges(void *a, void *b);
unsigned char CompareKmerEdgesGen(void *a, void *b);
unsigned char CompareKmerEdgesEquality(void *a, void *b);
void ExSortKmerEdges(const char *bin_file, const char *run_file, 
	unsigned char, ExKeyCompare);
unsigned char CompareKmersGen(void *a, void *b);
void ExSort(const char *in_file, const char *out_file, unsigned char KEY_SIZE,
			unsigned char col_start, unsigned char col_end, unsigned char endian);
#endif
