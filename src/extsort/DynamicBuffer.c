#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<string.h>
#include "DynamicBuffer.h"

DBuffer * NewDBuffer(unsigned char usize){
	DBuffer *db = malloc(sizeof(DBuffer));
	db->buffer = (unsigned char *) malloc(usize*DBUFFER_INCREMENT);
	db->bsize = DBUFFER_INCREMENT;
	db->bidx = 0; 
	db->usize = usize; 
	db->last_push = NULL;
	return db;
}

void InitDBuffer(DBuffer *dbuf, unsigned char usize){
	assert(dbuf); assert(usize);
	dbuf->buffer = (unsigned char *) malloc(usize*DBUFFER_INCREMENT);
	dbuf->bsize = DBUFFER_INCREMENT;
	dbuf->bidx = 0; 
	dbuf->usize = usize; 
}
/*NOTE: If blen < bidx you will loose data*/
void DBufferTrim(DBuffer *b, unsigned long blen){
	b->buffer = realloc(b->buffer, blen*(b->usize));
	assert(b->buffer);
	b->bsize = blen; 
	b->usize = blen;
}
void FreeDBuffer(DBuffer *db){
	free(db->buffer);
	free(db);
}
void * DBufferPush(DBuffer *dbuf, void *key){
	void *daddr;
	if(dbuf->bidx >= dbuf->bsize){
		dbuf->buffer = realloc(dbuf->buffer, 
			((dbuf->bsize+DBUFFER_INCREMENT)*dbuf->usize));
		dbuf->bsize += DBUFFER_INCREMENT;
	}
	daddr = memcpy(dbuf->buffer + ((dbuf->usize)*(dbuf->bidx++)),
		key, dbuf->usize);
	assert(daddr == (void *)(dbuf->buffer + (dbuf->usize*(dbuf->bidx-1))));
	dbuf->last_push = daddr;
	return daddr;
}

#ifdef UNIT_TEST_DBUFFER
#include<stdlib.h>

int main(){
	int iarray[6099];
	unsigned int i;
	DBuffer X;
	InitDBuffer(&X, sizeof(int));
	int *Xptr = NULL;

	for(i=0; i<6099; i++){
		iarray[i] = rand();
		DBufferPush(&X, iarray+i);
	}

	/*Now verify*/
	Xptr = (int *) X.buffer;
	for(i=0; i<6099; i++){
		if(Xptr[i] != iarray[i]){
			printf("TEST FAILED\n");
			exit(1);
		}
	}
	printf("PASSED\n");
	printf("DBuffer->bsize = %lu\n", X.bsize);
	printf("DBuffer->bidx = %lu\n", X.bidx);
	printf("Dbuffer->usize = %u\n", X.usize);
	free(X.buffer);
}
#endif
