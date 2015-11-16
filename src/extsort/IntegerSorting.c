/*12/25/2009 Updated the radix index so 
 *that we can sort from Right to Left
 *
 *12/25/2009 Added an endian option
 *
 *12/25/2009 Using static structures to make use
 *of as memory efficiently.
 */
#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include "MeasureTime.h"
#include "IntegerSorting.h"
/*If the size of stacks in the buckets grow > 128* , 
 *then we will adjust the buckets
 */
#define DEFRAGMENT_CONSTANT 256 
static unsigned long KEY_STACK_INCREMENT=4096;
/*
 *Pointers to the bucket datastructures which can be used
 *to free after the end of sorting
 */
static void *G_IPERM = NULL; 
static void *G_BUCKETS = NULL;
static void *G_BUCKETS1 = NULL;
static unsigned int G_BSIZE;
#ifdef DEBUG_VERBOSE
#define debug_assert(a) assert(a)
#else
#define debug_assert(a) 
#endif

void iswap(void *a, void *b, unsigned int isize){
	unsigned char tx; unsigned char *ca = (unsigned char *)a; 
	unsigned char *cb = (unsigned char *)b; 
	unsigned int j=0;
	for(j=0; j<isize; j++){
		tx = cb[j]; cb[j] = ca[j];
		ca[j] = tx;
	}
}

void PrintKeys(IBucket *b, unsigned bsize, char *ckeys, 
	unsigned int key_width, unsigned char col_width);
void PushInBucket(IBucket *b, unsigned long kid){
	if(b->ssize <= b->sidx){
		ExpandBucket(b);
	}
	b->key_stack[b->sidx++] = kid;
}
void ExpandBucket(IBucket *b){
	b->key_stack = realloc(b->key_stack,
	(b->ssize+KEY_STACK_INCREMENT)*sizeof(unsigned long));
	b->ssize += KEY_STACK_INCREMENT;
	debug_assert(b->key_stack);
}
/*Contract the bucket*/
void ContractBucket(IBucket *b){
	unsigned long nsize = (b->sidx+KEY_STACK_INCREMENT)&
	(~((unsigned long)KEY_STACK_INCREMENT));
	if(nsize == b->ssize) return;
	b->key_stack = (nsize < b->ssize)?realloc(b->key_stack,nsize*sizeof(unsigned long)):b->key_stack;
	b->ssize = nsize;
}
unsigned int CharMap(void *a){
	return (unsigned int)(*(unsigned char *)a);
}
/*Finds the appropriate mapper for this col_width and returns
 *the function pointer*/
unsigned int (*BucketIndexMapper(unsigned char col_width))(void *){
	if(col_width == 1){
		return CharMap;
	}else{
		fprintf(stderr,"TODO: MAPPING BITS > 8 is currently not implemented\n");
		return NULL;
	}
}
/*Radix + Bucket sort with 2^Col-bit buckets keys are 
 *assumed to be placed in a contiguous order
 *
 *NOTE: col_width (in bytes) should be some standard type or atmost 255.
 *		key_width (in bytes)
 *      col_start (starting column for radix sort)
 * 		col_end   (final column for radix sort)
 */
void IntegerSort(void *first_key, void *last_key, unsigned int key_width, 
	unsigned char col_width, unsigned char col_start, unsigned char col_end, 
	char ioptions, RadixMapper bmap, unsigned char endian){
	static IBucket *buckets = NULL; 
	static unsigned int bsize=0;
	static IBucket *buckets1 = NULL;
	static unsigned long *iperm = NULL; /*Permutation to Apply on data*/
	static unsigned long iperm_size = 0; 
	unsigned long j=0, i=0, k=0, cycle_start=0, pop=0;
	unsigned long ksize=(last_key - first_key)/key_width + 1; 
	unsigned long kid; IBucket *bptr, *bptr1; 
	unsigned long cbsize = (2<<(col_width*SIZE_OF_BYTE));
	char *ckeys = (char *)first_key; unsigned int bidx=0;
	IBucket *aux;

	debug_assert(col_start <= col_end && col_end < key_width/col_width);
	debug_assert(bmap); debug_assert(key_width >= col_width);
	debug_assert(ksize > 0);
	//printf("ISORT: Keys = %lu \n",ksize);
	
	for(i=0; i<bsize && bsize && buckets; i++){
		buckets[i].sidx=0; 
		buckets1[i].sidx=0;
	}

	/*Create buckets if necessary*/
	if(bsize != cbsize){
		buckets = realloc(buckets, sizeof(IBucket)*(cbsize+1));
		buckets1 = realloc(buckets1, sizeof(IBucket)*(cbsize+1));
		G_BUCKETS = buckets;
		G_BUCKETS1 = buckets1;
		debug_assert(buckets && buckets1);
		/*Initialize newly created buckets*/
		for(i=bsize; i<cbsize; i++){
			buckets[i].ssize=0; buckets[i].key_stack=NULL; buckets[i].sidx=0;
			buckets1[i].ssize=0; buckets1[i].key_stack=NULL; buckets1[i].sidx=0;
		}
		bsize = cbsize; 
		G_BSIZE = bsize;
	}

	/*First distribute the keys*/
	if(endian){
		j = col_end;
	}else{
		j = col_start;
	}
	for(i=0; i< ksize; i++){
		bidx = bmap(ckeys+i*key_width +j*col_width) ; debug_assert(bidx < bsize);
		PushInBucket(&buckets[bidx],i);
	}
	bptr1 = buckets; bptr = buckets1;
	/*RADIX SORT FOR key_width/col_width passes*/
	if(endian){
		while( --j <= col_end && j  >=  col_start){ 
			/*swap*/
			aux = bptr; bptr = bptr1; bptr1 = aux;  
			for(i=0; i<bsize; i++){
				for(k=0; k<bptr[i].sidx; k++){
					kid = bptr[i].key_stack[k];
					bidx = bmap(ckeys +kid*key_width + j*col_width); debug_assert(bidx < bsize);
					PushInBucket(bptr1+bidx,kid);
				}
				bptr[i].sidx = 0;
				/*Also free the bucket*/ 
				if(bptr[i].key_stack){
					free(bptr[i].key_stack); 
					bptr[i].ssize=0;
					bptr[i].key_stack=NULL;
				}
			}
		} 

		bptr = bptr1; 
	}else{
		while( ++j <= col_end){
			aux = bptr; bptr = bptr1; bptr1 = aux;  
			for(i=0; i<bsize; i++){
				for(k=0; k<bptr[i].sidx; k++){
					kid = bptr[i].key_stack[k];
					bidx = bmap(ckeys +kid*key_width + j*col_width); debug_assert(bidx < bsize);
					PushInBucket(bptr1+bidx,kid);
				}
				bptr[i].sidx = 0;
				/*Also free the bucket*/ 
				if(bptr[i].key_stack){
					free(bptr[i].key_stack); 
					bptr[i].ssize=0;
					bptr[i].key_stack=NULL;
				}
			}
		} 
		bptr = bptr1; 
	}

	if(iperm_size != ksize){
		iperm = realloc(iperm,sizeof(unsigned long)*ksize);
		G_IPERM = iperm;
	}

	j=0; //unsigned long mem_used_in_buckets=0;
	for(i=0; i<bsize; i++){
	//	mem_used_in_buckets += bptr[i].ssize;
		for(k=0; k<bptr[i].sidx; k++){
			kid = bptr[i].key_stack[k];
			iperm[j++] = kid; 
		}
	}
	//PrintKeys(bptr,bsize,ckeys,key_width,col_width);
	/*now apply the permutation, this destroy's the permutation
	 *here destroying it is OK*/
	cycle_start = 0; 
	while(cycle_start < ksize){
		pop = cycle_start;
		while(iperm[pop] != cycle_start){
			iswap(ckeys+iperm[pop]*key_width, ckeys+pop*key_width, key_width);
			iswap(&pop,&iperm[pop],sizeof(unsigned long));
		}
		iperm[pop] = pop;
		cycle_start++;
	}

	if(ioptions){ /*free-up the space*/
		free(iperm);
		for(i=0; i<bsize; i++){
			if(buckets[i].key_stack)
				free(buckets[i].key_stack);
			if(buckets1[i].key_stack)
				free(buckets1[i].key_stack);
		}
		free(buckets); free(buckets1);
	}
	//printf("Memory used in buckets is %lu bytes\n",mem_used_in_buckets);
}
void FreeISortBuckets(void){
	IBucket *buckets = (IBucket *) G_BUCKETS;
	IBucket *buckets1 = (IBucket *) G_BUCKETS1;
	unsigned long i; 
	unsigned int bsize = G_BSIZE;
	if(G_IPERM){
		free(G_IPERM);
		G_IPERM = NULL;
	}
	for(i=0; i<bsize && buckets; i++){
		if(buckets[i].key_stack){
			free(buckets[i].key_stack);
			buckets[i].key_stack = NULL;
		}
		if(buckets1[i].key_stack){
			free(buckets1[i].key_stack);
			buckets1[i].key_stack = NULL;
		}
	}
	if(buckets){
		free(buckets); free(buckets1);
	}
	G_BUCKETS = NULL;
	G_BUCKETS1 = NULL;
}

/**************************************************************************
 *IntegerSort_SB: This is the integer sorting which uses static bucket pairs
 *you should explicitly free the buckets by calling 'FreeISortBuckets'. This
 *also assumes the buckets it uses the size of upper CONFIDENCE_INTERVAL as
 *the size of each bucket. Currently the upper approximate CONFIDENCE_INTERVAL 
 *is as follows $CI^+ = E[input_size]+4096$
 *************************************************************************/
void IntegerSort_SB(void *first_key, void *last_key, unsigned int key_width, 
	unsigned char col_width, unsigned char col_start, unsigned char col_end, 
	char ioptions, RadixMapper bmap, unsigned char endian){
	static IBucket *buckets = NULL; 
	static unsigned int bsize=0;
	static IBucket *buckets1 = NULL;
	static unsigned long *iperm = NULL; /*Permutation to Apply on data*/
	static unsigned long iperm_size = 0; 
	unsigned long j=0, i=0, k=0, cycle_start=0, pop=0;
	unsigned long ksize=(last_key - first_key)/key_width + 1; 
	unsigned long kid; IBucket *bptr, *bptr1; 
	unsigned long cbsize = (2<<(col_width*SIZE_OF_BYTE));
	char *ckeys = (char *)first_key; unsigned int bidx=0;
	IBucket *aux;
#ifdef DEBUG_VERBOSE
	Clock *clk = CreateClock();
#endif

	debug_assert(col_start <= col_end && col_end < key_width/col_width);
	debug_assert(bmap); debug_assert(key_width >= col_width);
	debug_assert(ksize > 0);
#ifdef DEBUG_VERBOSE
	printf("ISORT-[STARTED]: Keys = %lu \n",ksize);
	StartClock(clk);
#endif

	/*reset state if 'FreeIsortBuckets' called in the last call*/
	if(!G_BUCKETS){ 
		bsize = 0; 
		buckets = NULL; buckets1 = NULL;
		iperm = NULL; iperm_size = 0;
	}
	
	for(i=0; i<bsize && bsize && buckets; i++){
		buckets[i].sidx=0; 
		buckets1[i].sidx=0;
	}

	/*Create buckets if necessary*/
	if(bsize != cbsize){
		buckets = realloc(buckets, sizeof(IBucket)*(cbsize+1));
		buckets1 = realloc(buckets1, sizeof(IBucket)*(cbsize+1));
		debug_assert(buckets && buckets1);
		/*MAKE KEY_STACK_INCREMENT as the CONFIDENCE_INTERVAL*/
		KEY_STACK_INCREMENT = ksize/cbsize + 4096;
		G_BUCKETS = buckets;
		G_BUCKETS1 = buckets1;
		/*Initialize newly created buckets to the CONFIDDENCE_INTERVAL*/
		for(i=bsize; i<cbsize; i++){
			buckets[i].ssize=0; buckets[i].key_stack=NULL; 
			buckets[i].key_stack = malloc(sizeof(unsigned long)*(KEY_STACK_INCREMENT));
			assert(buckets[i].key_stack);
			buckets[i].sidx=0;
			buckets[i].ssize = KEY_STACK_INCREMENT;
			
			buckets1[i].ssize=0; buckets1[i].key_stack=NULL; 
			buckets1[i].key_stack = malloc(sizeof(unsigned long)*(KEY_STACK_INCREMENT));
			assert(buckets[i].key_stack);
			buckets1[i].sidx=0;
			buckets1[i].ssize = KEY_STACK_INCREMENT;
		}
		bsize = cbsize; 
		G_BSIZE = bsize;
		KEY_STACK_INCREMENT= 4096;
	}

	/*First distribute the keys*/
	if(endian){
		j = col_end;
	}else{
		j = col_start;
	}
	for(i=0; i< ksize; i++){
		bidx = bmap(ckeys+i*key_width +j*col_width) ; debug_assert(bidx < bsize);
		PushInBucket(&buckets[bidx],i);
	}
	bptr1 = buckets; bptr = buckets1;
	/*RADIX SORT FOR key_width/col_width passes*/
	if(endian){
		while( --j <= col_end && j  >=  col_start){ 
			/*swap*/
			aux = bptr; bptr = bptr1; bptr1 = aux;  
			for(i=0; i<bsize; i++){
				for(k=0; k<bptr[i].sidx; k++){
					kid = bptr[i].key_stack[k];
					bidx = bmap(ckeys +kid*key_width + j*col_width); debug_assert(bidx < bsize);
					PushInBucket(bptr1+bidx,kid);
				}
				bptr[i].sidx = 0;
			}
		} 

		bptr = bptr1; 
	}else{
		while( ++j <= col_end){
			aux = bptr; bptr = bptr1; bptr1 = aux;  
			for(i=0; i<bsize; i++){
				for(k=0; k<bptr[i].sidx; k++){
					kid = bptr[i].key_stack[k];
					bidx = bmap(ckeys +kid*key_width + j*col_width); debug_assert(bidx < bsize);
					PushInBucket(bptr1+bidx,kid);
				}
				bptr[i].sidx = 0;
			}
		} 
		bptr = bptr1; 
	}

	if(iperm_size != ksize){
		iperm = realloc(iperm,sizeof(unsigned long)*ksize);
		G_IPERM = iperm;
	}

	j=0; //unsigned long mem_used_in_buckets=0;
	for(i=0; i<bsize; i++){
	//	mem_used_in_buckets += bptr[i].ssize;
		for(k=0; k<bptr[i].sidx; k++){
			kid = bptr[i].key_stack[k];
			iperm[j++] = kid; 
		}
	}
	//PrintKeys(bptr,bsize,ckeys,key_width,col_width);
	/*now apply the permutation, this destroy's the permutation
	 *here destroying it is OK*/
	cycle_start = 0; 
	while(cycle_start < ksize){
		pop = cycle_start;
		while(iperm[pop] != cycle_start){
			iswap(ckeys+iperm[pop]*key_width, ckeys+pop*key_width, key_width);
			iswap(&pop,&iperm[pop],sizeof(unsigned long));
		}
		iperm[pop] = pop;
		cycle_start++;
	}
#ifdef DEBUG_VERBOSE
	StopClock(clk);
	printf("ISORT [FINISHED] in  %ld ticks\n", GetClockTicks(clk));
#endif
}

void PrintKeys(IBucket *b, unsigned bsize, char *ckeys, unsigned int key_width, unsigned char col_width){
	unsigned int i,j,k;
	unsigned long kid;
	for(i=0; i<bsize; i++){
		for(k=0; k<b[i].sidx; k++){
			kid = b[i].key_stack[k];
			for(j=0; j*col_width< key_width; j++){
				printf("%03u ",(unsigned char)ckeys[kid*key_width + j*col_width]);
			}
			printf("\n");
		}
	}

}

#ifdef UNIT_TEST_ISORT
#include<stdlib.h>
unsigned long long uinttest[1<<27];
int main(){
	unsigned long long i;
	Clock *clk = CreateClock();
	for(i=0; i<(1<<27); i++){
		uinttest[i] = rand();
	}
	StartClock(clk);
	IntegerSort(uinttest, &uinttest[(1<<27)-1], 
		sizeof(unsigned long long), 1, 0, 5, 0, CharMap, 0);
	StopClock(clk);
	printf("The clock ticks is %ld \n",GetClockTicks(clk));
}
#endif
