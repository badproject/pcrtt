/*01/30/2010*/ /*11/22/2010 Creating a lean API for external sorting*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "ExternalSortAPI.h"
#include "DynamicBuffer.h"
#include "IntegerSorting.h"
#include "ExternalSort.h"
#include "MeasureTime.h"
/*Change the parameters to sort on differt keys*/
KmerEdgeParams ESortParams; 
/*
 *DESC: External Sort API function.
 * 
 *INPUT:
 * 1. in_file: name of the file containing keys to sort.
 * 2. out_file: name of the file to save the sorted keys.
 * 3. KEY_SIZE: size of each key in bytes, 
 * 4. col_start: the bytes between [col_start, col_end] will be used
 *               used to compare the keys during sorting. 
 * 5. col_end:   see above. 
 * 6. endian: either 0 or 1 (if 0 sorts the bytes between [col_start, col_end]
 *			  from left right, otherwise sorts from right to left).
 */
void ExSort(const char *in_file, const char *out_file, unsigned char KEY_SIZE,
			unsigned char col_start, unsigned char col_end, unsigned char endian){
	char *temp_file = NULL;
	ESortParams.col_start = col_start;
	ESortParams.col_end = col_end;
	ESortParams.endian = endian;
	temp_file = tempnam(".", "tmp-ex-sort");
	assert(temp_file);
	ExSortKmerEdges(in_file, temp_file, KEY_SIZE, CompareKmerEdges);
	ConvertRunToSequence(temp_file, out_file);

	if(remove(temp_file)){
		fprintf(stderr, "REMOVING FILE %s FAILED\n", temp_file);
		fprintf(stderr, "Try removing it manually\n");
	}

	if(temp_file){
		free(temp_file);
	}
}
/*
 *takes a binary file, set the columns which you want to 
 *sort in ESortParams.col_start and ESortParams.col_end
 */
void ExSortKmerEdges(const char *bin_kedge, const char *run_file, 
	unsigned char KEY_SIZE, ExKeyCompare key_compare){
	unsigned char *run_buffer = NULL;
	size_t key_buf_len = (KEY_SIZE*KEYS_IN_RUN);
	size_t buf_len = key_buf_len + sizeof(unsigned long);
	size_t ret_len;
	int bin_kedge_fd = open(bin_kedge, O_RDONLY);
	int run_file_fd = fileno(fopen(run_file, "w")); 
	unsigned long *runlen;
	unsigned long rcount = 0;
	unsigned char *key_buf;
	Clock *clk = CreateClock();

	if(!(bin_kedge_fd > 0 && run_file_fd > 0)){
		perror("FAILED TO OPEN FILES:");
		assert(0);
	}
	run_buffer = malloc(sizeof(unsigned char)*buf_len);
	assert(run_buffer);
	runlen = (unsigned long *)run_buffer;
	key_buf = run_buffer + sizeof(unsigned long);

	StartClock(clk);
	while((ret_len = SafeRead(bin_kedge_fd, key_buf, key_buf_len)) >= KEY_SIZE){
		*runlen = (unsigned long) ret_len;
		IntegerSort_SB(key_buf, key_buf + (*runlen) - KEY_SIZE, KEY_SIZE, 1, 
			ESortParams.col_start, ESortParams.col_end, 0, CharMap, 
			ESortParams.endian);

		ret_len = SafeWrite(run_file_fd, run_buffer, 
			((*runlen + sizeof(unsigned long))< buf_len)?(*runlen + 
				sizeof(unsigned long)):buf_len);
		rcount++;
		assert(ret_len == ((*runlen + sizeof(unsigned long) 
			< buf_len)?(*runlen + sizeof(unsigned long)):buf_len));
	}
	StopClock(clk);
	FreeISortBuckets();
	/*create the final run*/
	close(run_file_fd); close(bin_kedge_fd);
	free(run_buffer);
	
	printf("\n[EX-SORT CREATED %lu RUNS] took %ld ticks\n", rcount, GetClockTicks(clk));
	StartClock(clk);
	/*call the external rway merge*/
	ExternalRWayMerge(run_file, RUNS_PER_MERGE, KEY_SIZE, key_compare, rcount);
	StopClock(clk);
	printf("[R-WAY MERGE] took %ld ticks\n", GetClockTicks(clk));
}
/*Checks if the two edges are same*/
unsigned char CompareKmerEdges(void *a, void *b){
	unsigned int i;
	if(ESortParams.endian){
		for(i=ESortParams.col_start; 
			i<=ESortParams.col_end; i++){
			if(*(((unsigned char *)a)+i) != 
				*(((unsigned char *)b)+i)){
				return (*(((unsigned char *)a)+i) < 
					*(((unsigned char *)b)+i));
			}
		}
	}else{
		for(i=ESortParams.col_end; 
			(int)i >= (int )ESortParams.col_start; i--){
			if(*(((unsigned char *)a)+i) != 
				*(((unsigned char *)b)+i)){
				return (*(((unsigned char *)a)+i) < 
					*(((unsigned char *)b)+i));
			}
		}

	}
	return 1;
}
/*Compare for equality*/
unsigned char CompareKmerEdgesEquality(void *a, void *b){
	unsigned int i;
	for(i=ESortParams.col_start; i<=ESortParams.col_end; i++){
		if(*(((unsigned char *)a)+i) != 
			*(((unsigned char *)b)+i)){
			return 0;
		}
	}
	return 1;
}
