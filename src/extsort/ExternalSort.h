/*
 * ExternalSorting framework to implement 
 * PDM based algorithms.
 *
 * Vamsi Kundeti (vamsik@engr.uconn.edu).
 *
 */
#ifndef __EXTERNAL_SORT_H 
#define __EXTERNAL_SORT_H
#include<stdio.h>
#include<sys/types.h>
 typedef long OOffset;
/*This is also the main memory size M*/
/*The run has to be of length exactly DB */
extern unsigned int D_SIZE; 
/*Definition of the Run*/
typedef struct _ORun_{
	 FILE *fptr;
	 OOffset seek;
	 void *rptr;
} ORun;
/*Run definition*/
typedef struct _ORdef_ {
	 unsigned long rlen;
	 void *rinfo;
}ORdef;

/*If a<=b (returns non-zero) ELSE returns (zero)
 *
 * a==b returns a value of '2'
 **/
typedef unsigned char (*ExKeyBinCompare)(unsigned char *a, 
	unsigned char *b);
typedef unsigned char (*ExKeyCompare)(void *,void *);
/*The core routine to which performs the external merge*/
void TopLevelRWayMerge(const char * , unsigned int, unsigned int, 
	ExKeyCompare, unsigned long rcount);
/*8*4096*/
#define OS_PAGE_SIZE 4096 
void ExternalRWayMerge(const char *run_file, unsigned int R, unsigned int KEY_SIZE,
	ExKeyCompare keycompare, unsigned long rcount);
unsigned char VerifyRunFile(FILE *runfile, 
	ExKeyCompare key_compare, unsigned int KEY_SIZE);
/*Use these routines especially when reading huge chunks
 *of data*/
ssize_t SafeRead(int fd,void *buf,size_t rlen);
ssize_t SafeWrite(int fd,void *buf,size_t wlen);
unsigned char ExternalBinarySearch(FILE *sorted_key_file, unsigned long keylen,
	unsigned char *key, ExKeyBinCompare key_compare, unsigned char KEY_SIZE, 
	unsigned char *answer, off_t );
void ConvertRunToSequence(const char *run_file, const char *out_file);
#endif
