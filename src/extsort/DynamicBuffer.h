/*DynamicBuffer a simple data-structure to model expandable graphs*/
#ifndef __DYNAMIC_BUFFER_MY_UTIL
#define __DYNAMIC_BUFFER_MY_UTIL 1
#ifdef __cplusplus
extern "C" { 
#endif
#define DBUFFER_INCREMENT 1024
/*The total buffer space occupied is bsize*usize */
typedef struct _dbuffer_ {
	unsigned char *buffer;
	/*next available index for a push*/
	unsigned long bidx; 
	/*Current Size of the buffer*/
	unsigned long bsize;
	/*size of one unit in bytes*/
	unsigned char usize;
	/*Pointer to last pushed record*/
	unsigned char *last_push;
}DBuffer;
void InitDBuffer(DBuffer *, unsigned char usize);
/*Returns the pointer to the last pushed item*/
void * DBufferPush(DBuffer *, void *);
/*Trim the buffer to unsigned long*/
void DBufferTrim(DBuffer *, unsigned long);
DBuffer * NewDBuffer(unsigned char usize);
void FreeDBuffer(DBuffer *);
#ifdef __cplusplus
}
#endif
#endif
