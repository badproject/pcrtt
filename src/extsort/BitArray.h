#ifndef _BIT_ARRAY_H
#define _BIT_ARRAY_H 1
#include<stdio.h>
#include<stdlib.h>
#define BYTE_SIZE_IN_BITS 8
#define HALF_BYTE_SIZE 4
#define BYTE_SIZE_IN_BITS_2 6 
#define MASK_FIRST_TWO_MSB_BITS 192U
#define MASK_LAST_TWO_LSB_BITS 3U
/*finding the reminder with any power is very easy
 * a%(2^k) = a&(2^k-1). 
 *
 * NOTE: if 'a' is odd then REM_HALF_BYTE(a) is non-zero
 */
#define REM_HALF_BYTE(a) (a&3U)
typedef struct _barray_{
	unsigned char *barray;
	unsigned long width; 
	unsigned char msb_mask;
}BitArray;
/*Creates and returns a new BArray with the given
 *'width' in bits*/
BitArray *BArrayNew(unsigned long width);
/*BASIC OPERATIONS*/
/*Shift by 1 bit*/
void BArrayLShift();
void BArrayRShift();
/*
 *Shift by 2 bits: useful in 
 *manipulating DNA strings
 */
void BArrayLShift2(BitArray *, unsigned char);
void BArrayRShift2(BitArray *, unsigned char);
/*Returns a bit string up-to 8 bits, from 
 *position 'i'. Note if idx+len > width, the
 *bit register is truncated appropriately.
 */
unsigned char BArrayGetBit(BitArray *, unsigned long, unsigned char);
void PrintBits(BitArray *);
void PrintBitsInByte(unsigned char);
void PrintBitString(unsigned char *, unsigned long); 
/*Reset the bit-array*/
void ResetBitArray(BitArray *);
void FreeBitArray(BitArray *);
#endif
