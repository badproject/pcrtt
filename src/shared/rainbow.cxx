/**
 * Posix compliant version of CSA-Rainbow-Table-Tool
 * 
 * Copyright (C) 2015 NEngine Team
 * 
 * This file is part of CSA-Rainbow-Table-Tool.
 * 
 * CSA-Rainbow-Table-Tool is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * CSA-Rainbow-Table-Tool is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with CSA-Rainbow-Table-Tool.
 * If not, see <http://www.gnu.org/licenses/>.
 * 
 * This software is based on the windows version of CSA-Rainbow-Table-Tool
 * 
 * Copyright (C) 2012  Colibri <colibri.dvb@googlemail.com>  
 * http://colibri.net63.net/ alias http://colibri-dvb.info/
 * 
 */
#include <pcrtt.h>
#include <shared/shared.h>
#include <shared/rainbow.h>
#include <core/memalloc.h>
#include <vector>
#include <core/sort.h>
//#include <algorithm>

/*
#include <iostream>
#include <limits>

#include <stxxl/io>
#include <stxxl/vector>
#include <stxxl/stream>*/

namespace PCRTT {
namespace Shared {

/** ---------------------------------------------------------------------------
 *  -- Rainbow Table ----------------------------------------------------------
 *  ---------------------------------------------------------------------------
 */

#define CHECK_ALLOC(buffer,size) \
	buffer = new uchar[size]; \
	assert(buffer);

#define ADV_SIZE	(2*6)

/** locale */

static int compare(const void * a, const void * b)
{
	// cmp EndCw
	int i;
	const uchar *aa = (uchar*)a;
	const uchar *bb = (uchar*)b;

	for ( i=0; i< 6; ++i )
	{
		if(aa[6+i] != bb[6+i])
		{
			return aa[6+i] - bb[6+i];
		}
	}

	return 0; // equal
}

static inline void cw6ToValue(const uchar *Cw6, uint64_t *pullValue)
{
	*pullValue = 0;
	((uchar*)pullValue)[0] = Cw6[5];
	((uchar*)pullValue)[1] = Cw6[4];
	((uchar*)pullValue)[2] = Cw6[3];
	((uchar*)pullValue)[3] = Cw6[2];
	((uchar*)pullValue)[4] = Cw6[1];
	((uchar*)pullValue)[5] = Cw6[0];
}

static inline void valueToCw6(uchar *Cw6, const uint64_t ullValue)
{
	Cw6[0] = (ullValue >> 40) & 0xFF;
	Cw6[1] = (ullValue >> 32) & 0xFF;
	Cw6[2] = (ullValue >> 24) & 0xFF;
	Cw6[3] = (ullValue >> 16) & 0xFF;
	Cw6[4] = (ullValue >> 8) & 0xFF;
	Cw6[5] = ullValue & 0xFF;
}

static inline void incCw6(uchar *Cw6)
{
	uint64_t ullValue;
	cw6ToValue( Cw6, &ullValue );	++ullValue;
	valueToCw6( Cw6, ullValue );
}

static inline void getRnd(uint64_t *pRnd)
{
	int i;
	for(i=0; i < sizeof(uint64_t); ++i)
	{
		((uchar*)pRnd)[i] = rand() & 0xFF;
	}
}

static void EncRBElements(uchar *Cw6EndArray, const uchar *Cw6StartArray,
	ulong Count, ulong RoundNr, int SkipFirstEnc)
{
	int i;
	uchar Cw6[6], Crypt[8];
	ulong TempRoundNr;

	for(i=0; i<Count; ++i)
	{
		int FirstLoop = 1;

		memcpy( Cw6, &Cw6StartArray[i*6], sizeof(Cw6) );
		TempRoundNr = RoundNr;

		do
		{
			if( !SkipFirstEnc || !FirstLoop )
			{
				// Encrypt
				enc23( Crypt, Cw6 );

				//reduction function (cut last 2 byte)
				memcpy( Cw6, Crypt, sizeof(Cw6) );
			}

			FirstLoop = 0;

			//Reduction part 2 (XOR RoundNr)
			RainbowXorRoundNr(Cw6, TempRoundNr);
			++TempRoundNr;

		} while( TempRoundNr < RAINBOW_CHAIN_LEN );

		memcpy( &Cw6EndArray[i*6], Cw6, sizeof(Cw6) );
	}
}

static String GetRbtFileName(const String& RbtDirName,String filename)
{
	int Pos;
	String RbtDirName2(RbtDirName), pRbtFileName,
			ChainFileTitle;//without path and extention
	String RefExt, ChainFileTitleWithoutPrefix, ChainFileTitleWithoutPrefixRef;

	RbtDirName2 = RbtDirName;
	ensureDirNameHasBackslashEnding(RbtDirName2);

	//remove path
	char s = dirsep();
	Pos = filename.ReverseFind(s);

	if(Pos >= 0)
	{
		//path present -> remove path
		filename = filename.Right( filename.size() - (Pos + 1) );
	}

	//remove extention
	RefExt = format(".%s", FILE_EXT_RB_CHAINS);

	if(filename.size() < RefExt.size())
	{
		return pRbtFileName; // error
	}
	if(filename.Right(RefExt.size()) == RefExt)
	{
		ChainFileTitle = filename.Left(filename.size() - RefExt.size());

		ChainFileTitleWithoutPrefixRef = format("%s_%s_%Xh", 
			FILE_PART1_ALGO, FILE_PART2_PLAIN, RAINBOW_CHAIN_LEN);

		if(ChainFileTitle.size() >= ChainFileTitleWithoutPrefixRef.size())
		{
			ChainFileTitleWithoutPrefix = ChainFileTitle.Left(ChainFileTitleWithoutPrefixRef.size());

			if ( ChainFileTitleWithoutPrefixRef == ChainFileTitleWithoutPrefix )
			{
				// match
				pRbtFileName = format("%s%s.%s", 
					RbtDirName2.c_str(), ChainFileTitleWithoutPrefixRef.c_str(),
					FILE_EXT_RB_TABLE );
			}
		}
	}
	return pRbtFileName;
}

static void checkForIncStartValues(File* file,
	const uchar *FirstCw6StartValueRef, String& err)
{
	uchar Cw6StartValue[6], *Buffer;
	uint64_t FilePos, FileLen;
	String str1, str2;
	int i;

	file->SeekFromStart( 0 );

	FileLen = file->GetSize();

	if(FileLen != (MAX_ELEMENTS_PER_CHAIN_FILE * (6+6)))
	{
		err = format("ERROR: File \"%s\" has length %llu instead of %llu\n", 
			file->GetName().c_str(),
				FileLen, MAX_ELEMENTS_PER_CHAIN_FILE * (6+6));
		return;
	}

	assert(((MAX_ELEMENTS_PER_CHAIN_FILE * (6+6)) % ALLOC_SIZE_PER_FILE) == 0);

	CHECK_ALLOC(Buffer, ALLOC_SIZE_PER_FILE)

	memcpy( Cw6StartValue, FirstCw6StartValueRef, 6 );

	for(FilePos=0; FilePos < FileLen; FilePos += ALLOC_SIZE_PER_FILE )
	{
		if(file->read(Buffer, 1, ALLOC_SIZE_PER_FILE) != ALLOC_SIZE_PER_FILE)
		{
			err = format("ERROR: Can't read from file \"%s\"\n", 
				file->GetName().c_str() );
			KILLARRAY( Buffer );
			return;
		}

		for ( i=0; i < ALLOC_SIZE_PER_FILE; i += (6+6) )
		{
			if ( memcmp( &Buffer[i], Cw6StartValue, 6 ) != 0 )
			{
				str1 = String::bin2String( &Buffer[i], 6 );
				str2 = String::bin2String( Cw6StartValue, 6 );

				err = format("ERROR: File \"%s\" has a wrong start "
						"value (%s instead of %s) at pos %llu\n",
						file->GetName().c_str(), str1.c_str(), str2.c_str(),
						FilePos+i
					);

				KILLARRAY( Buffer );
				return;
			}

			incCw6(Cw6StartValue);
		}
	}
	KILLARRAY( Buffer );
}

static void CheckChainFile(Worker_t* worker, String filename, String& err,
	uchar *RndCw6EndValue)
{
	// RndCw6EndValue is optional (can be NULL if not required)
	// CFile ChainFile;
	File file;
	uchar StartCw6[6], EndCw6[6], RecalcedEndCw6[6];
	String Text, str1, str2;
	int i;
	
	err.clear();

	if( !file.Open( filename.c_str(), "rb" ) )
	{
		err = format( "ERROR: Can't open file \"%s\"\n", filename.c_str() );
		return;
	}

//	file.Read( Buffer, Count );

	uchar FirstCw6StartValueRef[6];

	if( !getStartCw6FromFileName(filename, FirstCw6StartValueRef) )
	{
		// error getting prefix from file name
		err = format("ERROR: Can't get prefix from file name \"%s\"\n",
				filename.c_str() );
		return;
	}

	output_gui("Checking chain file (start values) ...\n");
	checkForIncStartValues(&file,FirstCw6StartValueRef,err);

	if( err.size() )
	{
		return;
	}

	if ( RBT_STOP_ASKED || STOP_JOB_ASKED )
	{
		return;
	}

	output_gui("Checking chain file (recalculate %d random end values with "
			"CPU) ...\n",COUNT_OF_VERIFICATION_CHECKS_WITH_CPU_PER_CHAIN_FILE);

	float ErrorPerCent;
	ErrorPerCent = 100;

	for(i=0; i < COUNT_OF_VERIFICATION_CHECKS_WITH_CPU_PER_CHAIN_FILE; ++i)
	{
		err = Rainbow::GetRndChainFromFile(&file, StartCw6, EndCw6);

		if( err.size() )
		{
			output_gui(err);
			return;
		}

		if(worker->job == RBT_STOP || worker->job == STOP_JOB)
		{
			return;
		}

		str1 = String::bin2String( StartCw6, 6 );
		str2 = String::bin2String( EndCw6, 6 );

		output_gui("Verifying StartCw6 (%s) and EndCw6 (%s) pair\n",
				str1.c_str(), str2.c_str() );

		uint64_t Time;
		uint64_t Delta;
		Time = get_time();

		EncRBElements( RecalcedEndCw6, StartCw6, 1, 0, 0 );
//		Delta = get_time() - Time;
//		Text = format("Time = %llu ms\n", Delta);
//		output_gui(Text);

		if( RndCw6EndValue != NULL )
		{
			memcpy(RndCw6EndValue, EndCw6, 6);
		}

		if ( memcmp(RecalcedEndCw6, EndCw6, 6) != 0 )
		{
			str1 = String::bin2String(RecalcedEndCw6, 6);
			err = format("ERROR: The recalulated EndCw6 (%s) is "
							"different\n", str1.c_str() );
			return;
		}
		else
		{
			ErrorPerCent = ErrorPerCent / 2;

			output_gui("OK. So the chain file seems to be at least %.2f%% "
					"genuine\n", 100 - ErrorPerCent);
		}
	}
}

static void addElementToList(uchar_vp& list,
	uchar *TableCw6Start, uchar *TableCw6End, ulong Cw6EndArrayIdx)
{
	// [in/out] fcc_v *list (result will be added to the list)
	// [in] uchar TableCw6Start[6]
	// [in] uchar TableCw6End[6]
	// [in] ulong Cw6EndArrayIdx (from search - not from table)

	uchar *Cw6StartEndIdx = new uchar[6+6+4];

	memcpy( Cw6StartEndIdx, TableCw6Start, 6 );
	memcpy( &Cw6StartEndIdx[6], TableCw6End, 6 );

	Cw6StartEndIdx[12] = Cw6EndArrayIdx; // save Cw6EndArrayIdx
	Cw6StartEndIdx[13] = Cw6EndArrayIdx >> 8;
	Cw6StartEndIdx[14] = Cw6EndArrayIdx >> 16;
	Cw6StartEndIdx[15] = Cw6EndArrayIdx >> 24;

	list.push_back( Cw6StartEndIdx );
}

/** global action */

int Rainbow::IsStartPrefixInList(const uchar *StartPrefix,
	uchar_vp& list)
{
	// cmp 22bit start prefix
	int size, i;
	uchar *Cw6StartEndIdx;

	size = list.size();

	if( !size )
		return 0; // StartPrefix not in list

	for(i=0; i < size; ++i)
	{
		Cw6StartEndIdx = (uchar*)list[i];

		if((Cw6StartEndIdx[0] == StartPrefix[0])
			&& (Cw6StartEndIdx[1] == StartPrefix[1])
			&& ((Cw6StartEndIdx[2] & 0xFC) == (StartPrefix[2] & 0xFC)))
		{
			return 1; // StartPrefix in list
		}
	}

	return 0;
}

void Rainbow::FillCw6ArrayWithIncCw6Start(uchar *Cw6, ulong Elements,
	const uchar *Cw6Start)
{
	int i;
	uint64_t StartValue;

	cw6ToValue( Cw6Start, &StartValue );

	for(i=0; i < Elements; ++i)
	{
		Cw6[(i*6) + 0] = (StartValue >> 40) & 0xFF;
		Cw6[(i*6) + 1] = (StartValue >> 32) & 0xFF;
		Cw6[(i*6) + 2] = (StartValue >> 24) & 0xFF;
		Cw6[(i*6) + 3] = (StartValue >> 16) & 0xFF;
		Cw6[(i*6) + 4] = (StartValue >> 8) & 0xFF;
		Cw6[(i*6) + 5] = StartValue & 0xFF;

		++StartValue;
	}
}

String Rainbow::ValidateChainFile(File* file,const uchar *Cw6StartRef)
{
	String err;
	uint64_t FileLen;
	ulong Elements;
	uchar FirstCw6Start[6], LastCw6Start[6];
	String str1, str2;

	//default
	Elements = 0;

	FileLen = file->GetSize();

	if( (FileLen % (6+6)) != 0 )
	{
		err = format("ERROR: The file \"%s\" has a length (%llu) that"
			" isn't a multiple of (6+6)\n", file->GetName().c_str(), FileLen );

		return err;
	}

	Elements = FileLen / (6+6);

	if(Elements > 0)
	{
		file->SeekFromStart( 0  );

		file->Read( FirstCw6Start, 6 );

		if(memcmp(FirstCw6Start, Cw6StartRef, 6) != 0)
		{
			str1 = String::bin2String( FirstCw6Start, 6 );
			str2 = String::bin2String( Cw6StartRef, 6 );

			err = format("ERROR: The file \"%s\" has a wrong StartCw6 (%s) "
				"at pos (%Xh) (should be %s)\n",file->GetName().c_str(), 
				str1.c_str(), 0, str2.c_str() );

			return err;
		}

		file->SeekFromEnd( -(6+6) );
		uint64_t LastPos;
		LastPos = file->Tellg();
		file->Read(LastCw6Start, 6);

		uchar Cw6StartRef2[6];
		uint64_t ullValue;

		cw6ToValue(Cw6StartRef, &ullValue);
		ullValue += (LastPos/(6+6));

		valueToCw6(Cw6StartRef2, ullValue);

		if( memcmp( LastCw6Start, Cw6StartRef2, 6) != 0 )
		{
			str1 = String::bin2String( LastCw6Start, 6 );
			str2 = String::bin2String( Cw6StartRef2, 6 );

			err = format("ERROR: The file \"%s\" has a wrong StartCw6 (%s) "
					"at pos (%llXh) (should be %s)\n",file->GetName().c_str(),
					str1.c_str(), LastPos, str2.c_str() );
			return err;
		}
	}

	return err;
}

String Rainbow::GetInfoFromChainFile(uchar *NextCw6Start,ulong *pElements,
	File* file)
{
	String err;
	//NextCw6Start can be NULL if the info is not required
	uint64_t FileLen;

	// default
	if( NextCw6Start )
		memset( NextCw6Start, 0x00, 6 );

	*pElements = 0;

	FileLen = file->GetSize();

	if ( (FileLen % (6+6)) != 0 )
	{
		err = format("ERROR: The file \"%s\" has a length (%llu) that "
						"isn't a multiple of (6+6)\n",
						file->GetName().c_str(),FileLen);
	}

	*pElements = FileLen / (6+6);
	
	//TRACE_DEBUG("FileLen: %d - pElements: %d\n", FileLen, *pElements );

	if(NextCw6Start)
	{
		if(*pElements > 0)
		{
			file->SeekFromEnd( -(6+6) );
			file->Read( NextCw6Start, 6 );

			incCw6( NextCw6Start );
		}
	}
	return err;
}

void Rainbow::SaveCwStartAndEndArray(const uchar *Cw6Start,
	const uchar *Cw6End, ulong Elements, File *pDstFile)
{
	int i;
	String Text;

	//Save
	const ulong sz = Elements*(6+6);
	uchar *DataToSave;

	CHECK_ALLOC(DataToSave,sz)

	memset( DataToSave, 0x00, sz );

	for( i=0; i < Elements; ++i )
	{
		memcpy( &DataToSave[i*(6+6)], &Cw6Start[i*6], 6 );
		memcpy( &DataToSave[(i*(6+6))+6], &Cw6End[i*6], 6 );
	}

	pDstFile->Seekp( 0, std::ios_base::end );
	pDstFile->Write(DataToSave, sz);

	KILLARRAY( DataToSave );
}

String Rainbow::GetRndChainFromFile(File* file, uchar *StartCw6, uchar *EndCw6)
{
	String err;
	uint64_t FileLen, Elements, Rnd, ElementOffset;
	FileLen = file->GetSize();

	if((FileLen % (6+6)) != 0)
	{
		err = format("ERROR: The file \"%s\" has a length (%llu) "
			"that isn't a multiple of (6+6)\n", 
			file->GetName().c_str(), FileLen);
		return err;
	}

	Elements = FileLen / (6+6);
	getRnd( &Rnd );
	ElementOffset = (Rnd % Elements) * (6+6);

	file->SeekFromStart( ElementOffset );

	file->Read( StartCw6, 6 );
	file->Read( EndCw6, 6 );
	
	return err;
}

static int cb_merge(const char* message,const uint& total,const uint& writted,
	void* data)
{
	Worker_t* worker = (Worker_t*)data;

	if( !worker || STOP_JOB_ASKED || RBT_STOP_ASKED )
	{
		// merge canceled
		return 0;
	}

	if ( writted % 10000 == 0 )
	{
		worker->stats.ProcAddToRbtFile = (float)writted * 100.0 / 
											(float)(total);
		UpdateStatistic( &worker->stats, Stats_t::RBT );
	}

	if ( message )
	{
		output_gui( "%s\n", message );
	}

	return 1;
}

struct my_type
{
	my_type() : size(0) {}
	my_type(const my_type& o) : size(o.size)
	{ memcpy(data,o.data,sizeof(data)); }
	
	static my_type min_value()
    {
		if ( min.size == 0 )
		{
			memset(min.data,0x00,sizeof(min.data));
			min.size = 1;
		}
		return min;
    }
	static my_type max_value()
	{
		if ( max.size == 0 )
		{
			memset(max.data,0xFF,sizeof(max.data));
			max.size = 1;
		}
		return max;
	}
   
	/*my_type& operator=(const my_type& o) {
		data = o.data; size = o.size;
	}*/
	//inline bool operator()(my_type& a,my_type& b) { return (a < b); }
	uchar data[12];
	uint size;
	/** static */
	static my_type min;
	static my_type max;
};

my_type my_type::min;
my_type my_type::max;

inline bool operator == (const my_type& a, const my_type& b)
{ return (memcmp(a.data+6, b.data+6, 6) == 0); }

inline bool operator<(const my_type& a, const my_type& b)
{ return (memcmp(a.data+6, b.data+6, 6) < 0); }


struct Cmp
{
	typedef my_type first_argument_type;
	typedef my_type second_argument_type;
	typedef bool result_type;

	bool operator () (const my_type& a, const my_type& b) const
	{ return (a < b); }

	static my_type min_value() { return my_type::min_value(); }
	static my_type max_value() { return my_type::min_value(); }
};

std::ostream& operator << (std::ostream& o, const my_type& obj)
{
	if ( obj.size ) o.write((const char*)obj.data, obj.size);
	return o;
}

//int Rainbow::MergeSortedFiles(CFile *pDstFile, CFile *src1, CFile *src2)
int Rainbow::MergeSortedFiles(Worker_t* worker, File *pDstFile,
	File *src1,File *src2)
{
	//return false=error true=ok
	uint64_t SrcLen1, SrcLen2, DstPos;
	int UsedSizeBuffer1 = 0, UsedSizeBuffer2 = 0, UsedSizeDstBuffer = 0;
	uint64_t RemainingFromSrcFile1, RemainingFromSrcFile2;
	int Buffer1Pos = 0, Buffer2Pos = 0;
	uchar *Buffer1, *Buffer2, *BufferDst;

	SrcLen1 = src1->GetSize();
	SrcLen2 = src2->GetSize();

	const uint64_t TotalSize = (uint64_t)(SrcLen1 + SrcLen2);

	RemainingFromSrcFile1 = SrcLen1;
	RemainingFromSrcFile2 = SrcLen2;

	src1->SeekFromStart( 0 );
	src2->SeekFromStart( 0 );

	CHECK_ALLOC( Buffer1, ALLOC_SIZE_PER_FILE )
	CHECK_ALLOC( Buffer2, ALLOC_SIZE_PER_FILE )
	CHECK_ALLOC( BufferDst, ALLOC_SIZE_PER_FILE )

	const uint64_t aspf = ALLOC_SIZE_PER_FILE;

	for( DstPos = 0; DstPos < TotalSize; DstPos += ADV_SIZE )
	{
		if( STOP_JOB_ASKED || RBT_STOP_ASKED )
		{
			// merge canceled
			KILLARRAY( Buffer1 );
			KILLARRAY( Buffer2 );
			KILLARRAY( BufferDst );

			return 0;
		}

		if ( (DstPos & 0xFFFFFF) == 0 )
		{
			worker->stats.ProcAddToRbtFile = (float)DstPos * 100.0 / 
				(float)(SrcLen1+SrcLen2);
			UpdateStatistic( &worker->stats, Stats_t::RBT );
		}

		// refill Buffer
		if ( (UsedSizeBuffer1 == 0) && (RemainingFromSrcFile1 > 0) )
		{
			int Len = std::min(RemainingFromSrcFile1, aspf);
			int r = src1->ReadSome(Buffer1, Len);

			assert( r == Len );

			RemainingFromSrcFile1 -= Len;
			UsedSizeBuffer1 += Len;
			Buffer1Pos = 0;
		}

		if ( (UsedSizeBuffer2 == 0) && (RemainingFromSrcFile2 > 0) )
		{
			int Len = std::min( RemainingFromSrcFile2, aspf );
			int r = src2->ReadSome(Buffer2, Len);

			assert ( r == Len );

			RemainingFromSrcFile2 -= Len;
			UsedSizeBuffer2 += Len;
			Buffer2Pos = 0;
		}

		// write DstBuffer to file
		if ( UsedSizeDstBuffer == ALLOC_SIZE_PER_FILE )
		{
			pDstFile->Write( BufferDst, ALLOC_SIZE_PER_FILE );
			UsedSizeDstBuffer = 0;
		}

		// cmp
		if ( UsedSizeBuffer1 == 0 )
		{
			// no more Buffer1 data -> copy Buffer2
			memcpy( BufferDst + UsedSizeDstBuffer,
				Buffer2 + Buffer2Pos, ADV_SIZE );

			UsedSizeDstBuffer += ADV_SIZE;
			Buffer2Pos += ADV_SIZE;
			UsedSizeBuffer2 -= ADV_SIZE;

			continue;
		}

		if ( UsedSizeBuffer2 == 0 )
		{
			//no more Buffer2 data -> copy Buffer1
			memcpy( BufferDst + UsedSizeDstBuffer,
				Buffer1 + Buffer1Pos, ADV_SIZE );

			UsedSizeDstBuffer += ADV_SIZE;
			Buffer1Pos += ADV_SIZE;
			UsedSizeBuffer1 -= ADV_SIZE;

			continue;
		}

		// cmp buffer
		if ( memcmp( Buffer1 + (Buffer1Pos+6),
			Buffer2 +(Buffer2Pos+6), 6) < 0 )
		{
			// Buffer1 is lower -> copy Buffer1
			memcpy( BufferDst + UsedSizeDstBuffer,
				Buffer1 + Buffer1Pos, ADV_SIZE );

			UsedSizeDstBuffer += ADV_SIZE;
			Buffer1Pos += ADV_SIZE;
			UsedSizeBuffer1 -= ADV_SIZE;
		}
		else
		{
			memcpy( BufferDst + UsedSizeDstBuffer,
				Buffer2 + Buffer2Pos, ADV_SIZE );

			UsedSizeDstBuffer += ADV_SIZE;
			Buffer2Pos += ADV_SIZE;
			UsedSizeBuffer2 -= ADV_SIZE;
		}
	}

	// write remaining DstBuffer to file
	if ( UsedSizeDstBuffer > 0 )
	{
		pDstFile->Write( BufferDst, UsedSizeDstBuffer );
		UsedSizeDstBuffer = 0;
	}

	assert( UsedSizeBuffer1 == 0);
	assert( UsedSizeBuffer2 == 0);

	KILLARRAY( Buffer1 );
	KILLARRAY( Buffer2 );
	KILLARRAY( BufferDst );

	return 1;
}

static bool compare2(const Chunk_t& a, const Chunk_t& b)
{
	const uchar* aa = a.data;
	const uchar* bb = b.data;
	for ( int i=0; i< 6; ++i )
	{
		if(aa[6+i] != bb[6+i])
		{
			return aa[6+i] - bb[6+i];
		}
	}
	return 0;
}

static int sort_rbt(const char* src, const char* dst)
{
	FILE* fi, *fo;
	PCRTT::Chunk_v buffer;
	std::vector<uchar> vuchar;
	uchar* data;
	ulong i, offset=0;
	const ulong size = File::Size(src);
	double elapsed;
	const ulong total = size / ADV_SIZE;

	if( (size % ADV_SIZE) != 0 )
	{
		TRACE_ERROR("Invalid size of file source %s / %d", src, size );
		return 0;
	}

	fi = fopen(src,"rb");

	if(!fi)
	{
		TRACE_ERROR("Can't open file source: %s", src );
		return 0;
	}

	fo = fopen(dst,"wb");

	if(!fo)
	{
		TRACE_ERROR("Can't open file dest: %s", dst );
		return 0;
	}

	vuchar.resize(size);
	printf("vuchar.size: %d\n", vuchar.size() );
	data = vuchar.data();
	buffer.reserve( total );

	TRACE_DEBUG("Number of chunk to process: %d", total );

	elapsed = TestSpeed(1);
	int len = fread( data, 1, size, fi );
	printf("len: %d size: %d\n", len, size );
	assert(len == size);
	fclose(fi);

	for(i = 0; i < total; ++i)
	{
		Chunk_t t;

		t.size = ADV_SIZE;
		t.scmp = ADV_SIZE / 2;
		t.offset = ADV_SIZE / 2;
		t.data = &data[offset]; // new uchar[ADV_SIZE];
		offset += ADV_SIZE;

		//len = fread( t.data, 1, ADV_SIZE, fi );
		//assert(len == ADV_SIZE);

		buffer.push_back( t );
	}
	elapsed = TestSpeed(0);

	TRACE_DEBUG("Time for reading = %f msec.\n", elapsed);

	elapsed = TestSpeed(1);
	double st = sort( buffer, 8, total );
	elapsed = TestSpeed(0);

	TRACE_DEBUG( "Time for sorting = %f msec (st: %f).\n", elapsed,st );

	elapsed = TestSpeed(1);
	for(i = 0; i < total; ++i)
	{
		int len = fwrite( buffer[i].data, 1, ADV_SIZE, fo );
		assert(len == ADV_SIZE);
		//KILLARRAY(buffer[i].data);
	}
	elapsed = TestSpeed(0);
	fclose(fo);

	TRACE_DEBUG("Time for writing = %f msec.\n", elapsed);

	return 1;
}


struct u12
{
	uchar data[12];
};

struct cpuComp
{
	bool operator()(const u12& a, const u12& b)
	{
		return memcmp(a.data+6,b.data+6,6);
	}
};

static int sort_rbt2(const char* src, const char* dst)
{
	FILE* fi, *fo;
	//std::vector<uchar> vuchar;
	std::vector<u12> buffer;
	uchar* data;
	ulong i, offset=0;
	const ulong size = PCRTT::File::Size(src);
	double elapsed;
	const ulong total = size / ADV_SIZE;

	if( (size % ADV_SIZE) != 0 )
	{
		TRACE_ERROR("Invalid size of file source %s / %d", src, size );
		return 0;
	}

	fi = fopen(src,"rb");

	if(!fi)
	{
		TRACE_ERROR("Can't open file source: %s", src );
		return 0;
	}

	fo = fopen(dst,"wb");

	if(!fo)
	{
		TRACE_ERROR("Can't open file dest: %s", dst );
		return 0;
	}

	buffer.resize(total);

	printf("buffer.size: %d\n", buffer.size() );
	data = (uchar*)buffer.data();

	TRACE_DEBUG("Number of chunk to process: %d", total );

	elapsed = TestSpeed(1);
	int len = fread( data, 1, size, fi );
	printf("len: %d size: %d\n", len, size );
	assert(len == size);
	fclose(fi);

	elapsed = TestSpeed(0);

	TRACE_DEBUG("Time for reading = %f msec.\n", elapsed);

	elapsed = TestSpeed(1);
	std::sort( buffer.begin(), buffer.end(), cpuComp() );
	//tbb::parallel_sort(buffer.begin(), buffer.end(), cpuComp();
	elapsed = TestSpeed(0);

	TRACE_DEBUG( "Time for sorting = %f msec.\n", elapsed );

	elapsed = TestSpeed(1);
	fwrite( data, 1, size, fo );
	elapsed = TestSpeed(0);
	fclose(fo);

	TRACE_DEBUG("Time for writing = %f msec.\n", elapsed);

	return 1;
}

int Rainbow::SortRBTableFile(String UnsortedSrcFileName,
	String SortedDstFileName)
{
#if 1
	return sort_rbt2(UnsortedSrcFileName, SortedDstFileName);
#else
	// read unsorted file -> write to sorted file
	File SrcFile, DstFile;
	uchar *Data;
	uint64_t SrcLen;
	double elapsed;

	if ( !SrcFile.Open( UnsortedSrcFileName.c_str(), "rb") )
		return 0;

	SrcLen = SrcFile.GetSize();

	if((SrcLen % ADV_SIZE) != 0)
		return 0;

	if(!DstFile.Open(SortedDstFileName, "wb"))
		return 0;

	Data = new uchar[SrcLen];	assert( Data );

	elapsed = TestSpeed(1);
	SrcFile.Read(Data, SrcLen);
	elapsed = TestSpeed(0);

	TRACE_DEBUG("Time for reading = %f msec.\n", elapsed);

	elapsed = TestSpeed(1);
	qsort( (void*)Data, SrcLen/ADV_SIZE, ADV_SIZE, compare );
	elapsed = TestSpeed(0);

	TRACE_DEBUG( "Time for sorting = %f msec.\n", elapsed );

	elapsed = TestSpeed(1);
	DstFile.Write(Data, SrcLen);
	elapsed = TestSpeed(0);

	TRACE_DEBUG("Time for writing = %f msec.\n", elapsed );

	// free buffer
	KILLARRAY( Data );
#endif // 0
	return 1;
}

int Rainbow::FindCw6EndAndGetCs6StartFromSortedFile(
	uchar_vp& list, const uchar *Cw6EndArray,
	int Cw6EndArrayElements, File *src)
{
	// [out] list: 6Byte Cw6Start, 6Byte Cw6End, 4Byte Idx LSB
	// [in] const uchar *Cw6EndArray
	// [in] int Cw6EndArrayElements
	// [in] CFile SrcFile (Rainbow table)
	// find all matches and not only one
	// return true=ok false=error
	// binary search in sorted file
	uchar TableCw6Start[6], TableCw6End[6];
	String str1, str2;
	int64_t i, ElementIdx, Offset;
	ulong m;

	// List must be empty
	if( list.size() )
	{
		return 0; // error
	}

	src->SeekFromStart( 0 );

	uint64_t Len = src->GetSize();
	if((Len % ADV_SIZE) != 0)
	{
		return 0;//error
	}
	if(Len == 0)
	{
		return 1;//nothing to do
	}

	for(m=0; m < Cw6EndArrayElements; ++m)
	{
		const uchar *Cw6End = &Cw6EndArray[m * 6];

		int64_t LowerIdx = 0;
		int64_t UpperIdxOutside = Len / ADV_SIZE;
		int CmpResult;
		for(i=0;;++i)
		{
			ElementIdx = LowerIdx + (((UpperIdxOutside-1) - LowerIdx) / 2);
			Offset = ElementIdx * ADV_SIZE;
			if(Offset >= Len)
			{
				return 0;//error
			}
			src->SeekFromStart( Offset );
			src->Read(&TableCw6Start, 6);
			src->Read(&TableCw6End, 6);

			CmpResult = memcmp(TableCw6End, Cw6End, 6);
			if(CmpResult == 0)
			{
				//found
				addElementToList(list, TableCw6Start, TableCw6End, m);

				//may be the element left or right has the same CwEnd => check also

				int ContSearch;
				int64_t ElementIdxLower;
				int64_t ElementIdxUpper;
				ElementIdxLower = ElementIdx;
				ElementIdxUpper = ElementIdx;

				//check lower idx
				do
				{
					ContSearch = 0;
					ElementIdxLower--;
					if(ElementIdx >= 0)
					{
						Offset = ElementIdxLower * ADV_SIZE;
						assert(Offset < Len);

						src->SeekFromStart( Offset );
						src->Read(&TableCw6Start, 6);
						src->Read(&TableCw6End, 6);

						CmpResult = memcmp(TableCw6End, Cw6End, 6);

						if(CmpResult == 0)
						{
							//found
							addElementToList(list, TableCw6Start, TableCw6End, m);
							ContSearch = 1;
						}
					}
				}while(ContSearch);

				//check upper idx
				do
				{
					ContSearch = 0;
					ElementIdxUpper++;
					Offset = ElementIdxUpper * ADV_SIZE;
					if(Offset < Len)
					{
						src->SeekFromStart( Offset );
						src->Read(&TableCw6Start, 6);
						src->Read(&TableCw6End, 6);

						CmpResult = memcmp(TableCw6End, Cw6End, 6);

						if(CmpResult == 0)
						{
							//found
							addElementToList(list, TableCw6Start, TableCw6End, m);
							ContSearch = 1;
						}
					}
				}while(ContSearch);

				break;
			}

			if((UpperIdxOutside-1) <= LowerIdx)
			{
				break;//not found
			}

			if(CmpResult < 0)
			{
				//table element to low
				LowerIdx = ElementIdx+1;
			}
			else
			{
				//table element to high
				UpperIdxOutside = ElementIdx;
			}
		}
	}

	return 1;//ok
}

void Rainbow::FreeListElemets(uchar_vp& list)
{
	// free list elements
	for(int i=0; i<list.size(); ++i)
	{
		uchar *data = (uchar*)list[i];
		assert(data != NULL);
		KILLARRAY( data );
	}

	list.clear();
}

void Rainbow::LoadCwEndValuesFromFile(uchar **pBuffer, int *pLen, String SrcFileName)
{
	//this fctn will alloc and caller must free Buffer
	File SrcFile;

	*pBuffer = NULL;
	*pLen = 0;

	if(!SrcFile.Open(SrcFileName.c_str(), "rb"))
	{
		return;//file not present
	}

	uint64_t SrcLen = SrcFile.GetSize();
	if((SrcLen == 0) || ((SrcLen % 6) != 0))
	{
		return;//invalid content len
	}

	*pBuffer = new uchar[SrcLen];

	if(SrcFile.read(*pBuffer,1,SrcLen) != SrcLen)
	{
		delete [] *pBuffer;
		*pBuffer = NULL;
		return;//error reading file
	}
	*pLen = SrcLen;
}

// -- 
// addToRbtStart > start
void Rainbow::start(Worker_t* worker)
{
	String Text, RbtFileName, ChainFileName, RbtFileNameTemp;
	String ErrorText, RbtFileName2;
	uchar Cw6[6], RndCw6EndValue[6];
	File ChainFile, DstFile, RbtFile;
	uint64_t Time, Delta;
	String addtorbt_src( getAddToRbtSrc() );
	String addtorbt_rbtdir( getAddToRbtRbtDir() );

	if (!worker)
		return;

	clear_output_gui();

	Time = get_time();

	SetButtonStateaddToRbtStart();
	output_gui("Add to RBT Start\n");

	//pTI->faddToRbtStart = 0; // got it

	output_gui("Chain file: %s\n"
		"RBT dir: %s\n", addtorbt_src.c_str(), addtorbt_rbtdir.c_str() );

	if ( !addtorbt_src.size() )
	{
		output_gui("ERROR: Chain file name is empty\n");
		SetButtonStateAddToRbtStop();
		return;
	}

	if( !addtorbt_rbtdir.size() )
	{
		output_gui("ERROR: RBT dir name is empty\n");
		SetButtonStateAddToRbtStop();
		return;
	}

	ChainFileName = addtorbt_src;
	
	if(!getStartCw6FromFileName(ChainFileName, Cw6))
	{
		//error getting prefix from file name
		output_gui("ERROR: Can't get prefix from file name \"%s\"\n",
			ChainFileName.c_str() );
		SetButtonStateAddToRbtStop();
		return;
	}

	RbtFileName = GetRbtFileName( getAddToRbtRbtDir(), ChainFileName );

	if ( !RbtFileName.size() )
	{
		output_gui("ERROR: calculating RBT file name\n");
		SetButtonStateAddToRbtStop();
		return;
	}

	output_gui("RBT file: %s\n", RbtFileName.c_str() );

	CheckChainFile(worker, ChainFileName, ErrorText, RndCw6EndValue);

	if(worker->job == RBT_STOP || worker->job == STOP_JOB)
	{
		SetButtonStateAddToRbtStop();
		output_gui("Add to RBT Stop\n");
		//pTI->fAddToRbtStop = 0;//got it
		return;
	}

	if ( ErrorText.size() )
	{
		output_gui(ErrorText);
		SetButtonStateAddToRbtStop();
		return;
	}

	if( file_exists(RbtFileName) )
	{
		// file present -> open it
		if( !RbtFile.Open(RbtFileName.c_str(), "rb") )
		{
			output_gui("ERROR: Can't open file \"%s\"\n", RbtFileName.c_str() );
			SetButtonStateAddToRbtStop();
			return;
		}
	}
	else
	{
		// file not present -> create empty file
		if( !RbtFile.Open(RbtFileName, "a+") )
		{
			output_gui("ERROR: Can't create empty file \"%s\"\n",
				RbtFileName.c_str() );
			SetButtonStateAddToRbtStop();
			return;
		}
	}

	output_gui( "Checking if chain file is already present in RBT ...\n" );
	uchar_vp Cw6StartEndIdxList;

	if( !FindCw6EndAndGetCs6StartFromSortedFile(Cw6StartEndIdxList,
		RndCw6EndValue, 1, &RbtFile))
	{
		output_gui("ERROR: Unexpected error\n");
		SetButtonStateAddToRbtStop();
		return;
	}

	if( IsStartPrefixInList(Cw6,Cw6StartEndIdxList) )
	{
		//Cw6Start value already present in RBT
		output_gui("Chain file already present in RBT.\nMerge done\n");
		FreeListElemets( Cw6StartEndIdxList );
		SetButtonStateAddToRbtStop();
		return;
	}

	String SortedChainFileName;
	// removed since we sort the whole rbt again and again
	// with external sort algo
	//SortedChainFileName = ChainFileName;
	SortedChainFileName = ChainFileName + ".sorted";

	TRACE_DEBUG("Sorting file: %s", SortedChainFileName.c_str() );

	output_gui("Sorting ... (will take a few minutes)\n");

	if( !SortRBTableFile(ChainFileName, SortedChainFileName) )
	{
		output_gui("ERROR: Can't sort chain file \"%s\"\n",
			ChainFileName.c_str() );
		SetButtonStateAddToRbtStop();
		return;
	}

	if(!ChainFile.Open(SortedChainFileName.c_str(), "rb"))
	{
		output_gui("ERROR: Can't open file \"%s\"\n", 
			SortedChainFileName.c_str() );
		SetButtonStateAddToRbtStop();
		return;
	}

	RbtFileNameTemp = RbtFileName + ".temp";

	if(!DstFile.Open(RbtFileNameTemp.c_str(), "a+"))
	{
		output_gui("ERROR: Can't create file \"%s\"\n",RbtFileNameTemp.c_str());
		SetButtonStateAddToRbtStop();
		return;
	}

	//Merge
	output_gui("Merging ...\n");

	if( !MergeSortedFiles(worker, &DstFile, &RbtFile, &ChainFile) )
	{
		//merge canceled
		DstFile.Close();
		File::Delete( RbtFileNameTemp );

		SetButtonStateAddToRbtStop();
		if(worker->job == RBT_STOP)
		{
			output_gui("Add to RBT Stop\n");
			//pTI->fAddToRbtStop = 0;//got it
			worker->stats.ProcAddToRbtFile = 0;
			UpdateStatistic(&worker->stats, Stats_t::RBT);
		}
		return;
	}

	worker->stats.ProcAddToRbtFile = 0;
	UpdateStatistic(&worker->stats, Stats_t::RBT);

	ChainFile.Close();
	RbtFile.Close();
	DstFile.Close();

	RbtFileName2 = RbtFileName + ".delete";

	// rename old RBT before deleting it
	if ( 0 != File::Rename(RbtFileName, RbtFileName2) )
	{
		output_gui("ERROR: Rename from \"%s\" to \"%s\" failed (cause:%s)\n", 
			RbtFileName.c_str(), RbtFileName2.c_str(), strerror(errno) );
		SetButtonStateAddToRbtStop();
		return;
	}

	// rename new temp RBT name to default RBT name
	if ( 0 != File::Rename(RbtFileNameTemp, RbtFileName) )
	{
		output_gui("ERROR: Rename from \"%s\" to \"%s\" failed "
			"(cause:%s)\n", RbtFileNameTemp.c_str(),
			RbtFileName.c_str(), strerror(errno) );
		SetButtonStateAddToRbtStop();
		return;
	}

	// delete old RBT
	File::Delete(RbtFileName2);
	//not needed anymore 
	File::Delete(SortedChainFileName);

	SetButtonStateAddToRbtStop();
	Delta = get_time() - Time;

	output_gui("Merge done\n", Delta/1000);
}

} // end of namespace Shared
} // end of namespace PCRTT
