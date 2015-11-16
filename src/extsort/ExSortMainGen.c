/*A GENERAL EXTERNAL SORTING ROUTINE*/
#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<string.h>
#include "IntegerSorting.h"
#include "BitArray.h"
#include "ExternalSortAPI.h"
#include "ExternalSort.h"
#include "CrashInfo.h"


int main(int argc, char **argv){
	unsigned char K;

	if(argc < 6){
		fprintf(stderr, 
		"USAGE: ./ex-sort {KEY_SIZE} {SORT-KEY-START} {SORT-KEY-END} {in-file} {out-file} {endian}\n");
		exit(1);
	}
	sscanf(argv[1], "%hhu", &K); 
	sscanf(argv[2], "%hhu", &ESortParams.col_start);
	sscanf(argv[3], "%hhu", &ESortParams.col_end);
	SetUpCrashRecovery();
	if(argc == 7){
		sscanf(argv[6], "%hhu", &ESortParams.endian);
	}else{
		ESortParams.endian = 1;
	}


	if(ESortParams.col_start >= K || 
			ESortParams.col_end >= K){
		fprintf(stderr, "INVALID COLUMN RANGES FOR SORTING\n");
		exit(1);
	}

	printf("SIZE OF EACH KEY IS %hhu\n", K);
	printf("EACH KEY WILL BE SORTED BETWEEN %hhu AND %hhu ENDIAN %hhu\n", 
		ESortParams.col_start, ESortParams.col_end, ESortParams.endian);
	ExSort(argv[4], argv[5], K, ESortParams.col_start, ESortParams.col_end,
		ESortParams.endian);
#if 0
	char *tmp_file = NULL;
	ESortParams.endian = 1;
	tmp_file = tmpnam(NULL);
	ExSortKmerEdges(argv[4], tmp_file/*argv[5]*/, K, CompareKmerEdges);
	ConvertRunToSequence(tmp_file, argv[5]);
#endif
	return 0;
}
