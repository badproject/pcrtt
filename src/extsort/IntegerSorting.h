#ifndef __ISORT_H
#define __ISORT_H
typedef struct __isort_bucket__{
	unsigned long *key_stack; /*Indices to the keys*/
	unsigned long ssize; /*size of bucket-list*/
	unsigned long sidx;
}IBucket;
void ExpandBucket(IBucket *);
typedef unsigned int (* RadixMapper)(void *);
void IntegerSort(void *, void *, unsigned int, unsigned char, unsigned char, 
	unsigned char, char, RadixMapper, unsigned char);
void IntegerSort_SB(void *, void *, unsigned int, unsigned char, unsigned char, 
	unsigned char, char, RadixMapper, unsigned char);
unsigned int CharMap(void *);
void FreeISortBuckets(void);
#define SIZE_OF_BYTE 8
#endif
