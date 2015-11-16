/*BitArray data structure*/
#include<assert.h>
#include<stdlib.h>
#include "BitArray.h"
/*Note the width is in bits*/
BitArray* BArrayNew(unsigned long width){
	BitArray *ptr = malloc(sizeof(BitArray)); assert(ptr);
	ptr->width = (width>>3); assert(width >= 1);
	ptr->msb_mask = (unsigned char)-1;
	if(width&7){
		ptr->width++; 
		ptr->msb_mask >>= (8-(width&7));
	}
	/**/
	ptr->barray = (unsigned char *) calloc(ptr->width, sizeof(unsigned char));
	assert(ptr->barray);
	return ptr;
}
/*Shift the entire */
void BArrayLShift2(BitArray *b, unsigned char mask){
	unsigned long i;
	for(i=0; i<(b->width)-1; i++){
		b->barray[i] <<=2;
		b->barray[i] |= ((b->barray[i+1] & MASK_FIRST_TWO_MSB_BITS)>>(BYTE_SIZE_IN_BITS_2));
	}
	b->barray[i] <<=2; b->barray[0] &= b->msb_mask; 
	b->barray[i] |= (mask&MASK_LAST_TWO_LSB_BITS);
}
/*Prints the bits inside a byte*/
void PrintBitsInByte(unsigned char b){
	unsigned char mask = (1U<<(BYTE_SIZE_IN_BITS-1));
	while(mask){
		if(mask&b){
			printf("1");
		}else{
			printf("0");
		}
		mask>>=1;
	}
}
void PrintBitString(unsigned char *b, 
	unsigned long bwidth){
	unsigned long i;
	for(i=0; i<bwidth; i++){
		PrintBitsInByte(b[i]);
	}
}
/*Prints the underlying bit-string*/
void PrintBits(BitArray *b){
	unsigned long i;
	for(i=0; i<b->width; i++){
		PrintBitsInByte(b->barray[i]);
	}
}

void ResetBitArray(BitArray *b){
	unsigned long i = b->width;
	for(i=0; i<b->width; i++){
		b->barray[i] = (unsigned char) 0;
	}
}
void FreeBitArray(BitArray *b){
	free(b->barray);
	free(b);
}

#ifdef UNIT_TEST_BARRAY
int main(int argc, char **argv){
	BitArray *ba = BArrayNew(31);
	unsigned char mask = 2;
	unsigned char i;
	for(i=0; i<16; i++){
		if(i&1){
			BArrayLShift2(ba, mask);
		}else{
			BArrayLShift2(ba, 0U);
		}
		PrintBits(ba);
		printf("\n");
	}
}
#endif
