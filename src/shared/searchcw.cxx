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
#include <shared/define.h>
#include <shared/shared.h>
#include <shared/rainbow.h>
#include <shared/searchcw.h>

namespace PCRTT {
namespace Shared {

/** ---------------------------------------------------------------------------
 *  -- Search CW --------------------------------------------------------------
 *  ---------------------------------------------------------------------------
 */
static void free_uchar(uchar_vp& Cw6StartEndIdxList)
{
	int size = Cw6StartEndIdxList.size();
	for(int i = 0; i < size; ++i)
	{
		uchar* data = Cw6StartEndIdxList[i];
		KILLARRAY(data);
	}
	Cw6StartEndIdxList.clear();
}

static String GetRbtFileName(const String& name)
{
	return
	 format("%s%s_%s_%Xh.%s", name.c_str(), FILE_PART1_ALGO, FILE_PART2_PLAIN,
		RAINBOW_CHAIN_LEN, FILE_EXT_RB_TABLE
	);
}

static String GetEndCwCacheFileNameFromCrypt(const uchar *Crypt)
{
	// cache file contains all possible end CWs calculated from crypt8
	return format("%s_%s_%Xh_%02X%02X%02X%02X%02X%02X%02X%02X.%s",
		FILE_PART1_ALGO, FILE_PART2_PLAIN, RAINBOW_CHAIN_LEN, 
		Crypt[0], Crypt[1], Crypt[2], Crypt[3], Crypt[4], Crypt[5], Crypt[6],
		Crypt[7], FILE_EXT_END_CW
	);
}

void SearchCw::display_cw(const char* cw)
{
	output_gui("CW: %s\n", cw);
}

int SearchCw::get_key_from_cw_start(const uint flags, uchar *Cw6,
	const uchar *Crypt, uchar_vp& pCw6StartEndIdxList)
{
	// Cw6 [out]
	// Crypt [in]
	// pCw6StartEndIdxList [in]
	// return true=Cw6 found that encrypts to Crypt
	DEBUG_STRUCT Debug;
	uchar *Cw6Array, *Cw6StartEndIdx;
	ulong *CurRoundNrArray, *RemRoundsArray, *StatisticRoundsArray;
	int i, m, Repeat, RemListElements, result;
	//POSITION pos;

	// todo: element_size per worker type (cuda, opencl, vulkan,...)
	// Check
	#define MAX_ELEMENTS_FOR_RAINBOW_XROUNDS (BLOCKS_RB * THREADS_RB)
	assert(MAX_ELEMENTS_FOR_RAINBOW_XROUNDS <= (BLOCKS_RB * THREADS_RB));

	// Alloc
	Cw6Array = new uchar[BLOCKS_RB * THREADS_RB * 6];
	assert(Cw6Array);

	CurRoundNrArray = new ulong[BLOCKS_RB * THREADS_RB];
	assert(CurRoundNrArray);

	RemRoundsArray = new ulong[BLOCKS_RB * THREADS_RB];
	assert(RemRoundsArray);

	StatisticRoundsArray = new ulong[BLOCKS_RB * THREADS_RB];
	assert(StatisticRoundsArray);

	RemListElements = pCw6StartEndIdxList.size();
	//pos = 0; // pCw6StartEndIdxList->GetHeadPosition();

	while(RemListElements > 0)
	{
		ulong Idx;
		int PartCount;

		PartCount = M_MIN(RemListElements, MAX_ELEMENTS_FOR_RAINBOW_XROUNDS);

		memset( RemRoundsArray, 0x00, BLOCKS_RB * THREADS_RB * sizeof(ulong) );
		memset( CurRoundNrArray, 0x00, BLOCKS_RB * THREADS_RB * sizeof(ulong) );

		for(i=0; i < PartCount; ++i)
		{
			Cw6StartEndIdx = pCw6StartEndIdxList[i];
			memcpy( &Cw6Array[i*6], Cw6StartEndIdx, 6 );

			Idx = Cw6StartEndIdx[12];
			Idx |= (Cw6StartEndIdx[13] << 8);
			Idx |= (Cw6StartEndIdx[14] << 16);
			Idx |= (Cw6StartEndIdx[15] << 24);

			RemRoundsArray[i] = Idx;
			StatisticRoundsArray[i] = Idx;
		}

		Repeat = true;
		for(m=0; ( m < (RAINBOW_CHAIN_LEN / RAINBOW_CHAIN_LEN_PER_KERNEL)) &&
			Repeat; ++m)
		{
			if ( 0 ) {}
#if defined(__CUDA__)
			else if ( IS_CUDA(flags) )
			{
				result = CsaCudaRainbowDoXRounds(Cw6Array,
						CurRoundNrArray, RemRoundsArray, &Debug );
			}
#endif // __CUDA__
#if defined(__OPENCL__)
			else if ( IS_OPENCL(flags) )
			{
				result = PCRTT::OpenCL::CsaRainbowDoXRounds(Cw6Array,
						CurRoundNrArray, RemRoundsArray);
			}
#endif // __OPENCL__
#if defined(__VULKAN__)
			else if ( IS_VULKAN(flags) )
			{
				result = PCRTT::Vulkan::CsaRainbowDoXRounds(Cw6Array,
						CurRoundNrArray, RemRoundsArray, &Debug );
			}
#endif // __VULKAN__

			assert( result );

			Repeat = false;
			for(i=0; i<(BLOCKS_RB * THREADS_RB) && !Repeat; i++)
			{
				if(RemRoundsArray[i] != 0)
				{
					Repeat = true;
				}
			}
		}

		assert( !Repeat );

		for(i=0; i<PartCount; ++i)
		{
			uchar TempCrypt[8];
			const uchar *x = &Cw6Array[i*6];

			//TRACE(">>>CW: %02X%02X%02X %02X%02X%02X\n", x[0], x[1], x[2], x[3], x[4], x[5]);

			enc23(TempCrypt,  &Cw6Array[i*6]);

			//TRACE(">>>enc: %02X %02X %02X %02X %02X %02X %02X %02X\n", TempCrypt[0], TempCrypt[1], TempCrypt[2], TempCrypt[3], TempCrypt[4], TempCrypt[5], TempCrypt[6], TempCrypt[7]);

			if( memcmp(TempCrypt, Crypt, 8) == 0 )
			{
				//match
				memcpy(Cw6, &Cw6Array[i*6], 6);
			
				//TRACE("StatisticRoundsArray[i]:%04Xh\n", StatisticRoundsArray[i]);

				// Free
				KILLARRAY( Cw6Array );
				KILLARRAY( CurRoundNrArray );
				KILLARRAY( RemRoundsArray );
				KILLARRAY( StatisticRoundsArray );

				return 1; // key found
			}
		}

		RemListElements -= PartCount;
	}

	// Free
	KILLARRAY( Cw6Array );
	KILLARRAY( CurRoundNrArray );
	KILLARRAY( RemRoundsArray );
	KILLARRAY( StatisticRoundsArray );

	return 0; // no match
}
// SearchCwStart
void SearchCw::start(Worker_t* worker)
{
	int Crypt8Len, Len;
	uint64_t Time, Delta;
	File RbtFile;
	String str, EndCwFileName, EndCwFileNameWithoutPath, RbtFileName;
	String search_cw_rbt_dir(getSearchCwRbtDir());
	uchar *Cw6EndArray, Cw6[6], Cw8[8], Crypt8[20];
	uchar_vp Cw6StartEndIdxList;
	String searchcw_crypt8(getSearchCwCrypt8());

	Time = get_time();

	clear_output_gui();

	//display_cw("");

	SetButtonStateSearchCwStart();
	output_gui("Search CW Start\n");
	//pTI->fSearchCwStart = false;//got it

	ensureDirNameHasBackslashEnding( search_cw_rbt_dir );

	RbtFileName = GetRbtFileName(getSearchCwRbtDir());

	output_gui("RBT file: %s\n", RbtFileName.c_str() );

	if ( !RbtFile.Open(RbtFileName.c_str(), "rb") )
	{
		output_gui("ERROR: Can't open file \"%s\"\n", RbtFileName.c_str());
		SetButtonStateSearchCwStop();
		return;
	}

	Crypt8Len = 20; // max len

	String::string2Bin( searchcw_crypt8, Crypt8, &Crypt8Len );

	if(Crypt8Len != 8)
	{
		output_gui("ERROR: The Crypt8 hex lengh is not 8\n");
		SetButtonStateSearchCwStop();
		return;	
	}

	output_gui("Calc all 10000h end values for this crypt ... (using file "
		"cache)\n");

	CalcRainbowEndValueArrayFromCrypt(worker->flags,Crypt8,search_cw_rbt_dir);

	if ( CW_STOP_ASKED || STOP_JOB_ASKED )
	{
		SetButtonStateSearchCwStop();
		output_gui("Search CW Stop\n");
		//pTI->fSearchCwStop = false;//got it
		return;
	}

	EndCwFileNameWithoutPath = GetEndCwCacheFileNameFromCrypt(Crypt8);
	EndCwFileName = format("%s%s", search_cw_rbt_dir.c_str(),
						EndCwFileNameWithoutPath.c_str());

	output_gui("Search end values in RBT ...\n");
	Rainbow::LoadCwEndValuesFromFile(&Cw6EndArray, &Len, EndCwFileName);
	if(Len != (0x10000 * 6))
	{
		if(Cw6EndArray != NULL)
		{
			KILLARRAY( Cw6EndArray );
		}

		output_gui("ERROR: The file \"%s\" has a wrong length (%Xh instead "
				"of %Xh). Please delete this cache file and try again.\n", 
			EndCwFileName.c_str(), Len, 0x10000 * 6);

		KILLARRAY( Cw6EndArray );

		SetButtonStateSearchCwStop();

		return;
	}

	if( CW_STOP_ASKED || STOP_JOB_ASKED)
	{
		KILLARRAY( Cw6EndArray );
		SetButtonStateSearchCwStop();
		output_gui("Search CW Stop\n");
		//pTI->fSearchCwStop = false;//got it
		return;
	}

	output_gui("Searching CW in RBT ...\n");

	// 0x10000 
	// (fcc_v *pCw6StartEndIdxList, const uchar *Cw6EndArray,int Cw6EndArrayElements, File *pSrcFile)
	if(!Rainbow::FindCw6EndAndGetCs6StartFromSortedFile(Cw6StartEndIdxList,
		Cw6EndArray, 0x10000, &RbtFile))
	{
		output_gui("ERROR: FindCw6EndAndGetCs6StartFromSortedFile failed\n");
		KILLARRAY( Cw6EndArray );
		SetButtonStateSearchCwStop();
		return;
	}
	output_gui("Found %d possible chains\n", Cw6StartEndIdxList.size());

	if(CW_STOP_ASKED || STOP_JOB_ASKED )
	{
		KILLARRAY( Cw6EndArray );
		SetButtonStateSearchCwStop();
		output_gui("Search CW Stop\n");
		//pTI->fSearchCwStop = false;//got it
		return;
	}

	output_gui("Analysing chains ... (will be 10 times slower if an other thread "
		"is keeping the GPU busy)\n");

	if ( get_key_from_cw_start( worker->flags, Cw6, Crypt8,
			Cw6StartEndIdxList) )
	{
		Cw6ToCw8(Cw8, Cw6);
		str = String::bin2String(Cw8, 8);
		display_cw(str);
		setSearchCwCw(str);

		output_gui("found CW: %s\n", str.c_str() );
	}
	else
	{
		output_gui("CW not found\nIt seems you need a bigger RBT\n");
	}

	free_uchar( Cw6StartEndIdxList );

	KILLARRAY( Cw6EndArray );

	SetButtonStateSearchCwStop();
	Delta = get_time() - Time;

	output_gui("Search CW done (%llu sec.)\n", Delta/1000);
}


void SearchCw::CalcRainbowEndValueArrayFromCrypt(const uint flags, const uchar *Crypt,
	const String& DstDirName)
{
	String FileNameWithoutPath;
	DEBUG_STRUCT Debug;
	uchar *Cw6EndArray;
	ulong *RoundNrArray;
	uchar *CryptArray;
	ulong Elements;
	int i, result;
	String Text;
	//String str;
	String filename;
	FILE* fp;

	//alloc memory
	Elements = BLOCKS_RB * THREADS_RB;

	CryptArray = new uchar[Elements*8];		assert( CryptArray );
	RoundNrArray = new ulong[Elements];		assert( RoundNrArray );
	Cw6EndArray = new uchar[Elements*6];	assert( Cw6EndArray );

	for( i = 0; i < (int)Elements; ++i )
	{
		memcpy( &CryptArray[i*8], Crypt, 8 );
		RoundNrArray[i] = i;	
	}

	FileNameWithoutPath = GetEndCwCacheFileNameFromCrypt(Crypt);

	filename = format( "%s%s", DstDirName.c_str(),
			FileNameWithoutPath.c_str() );

	if ( file_exists(filename) )
	{
		TRACE_DEBUG("skip already present file: %s\n", filename.c_str() );
		KILLARRAY( CryptArray );
		KILLARRAY( RoundNrArray );
		KILLARRAY( Cw6EndArray );

		return;
	}

	assert( (RAINBOW_CHAIN_LEN % RAINBOW_CHAIN_LEN_PER_KERNEL) == 0 );

	for(i=0; i<(RAINBOW_CHAIN_LEN / RAINBOW_CHAIN_LEN_PER_KERNEL); i++)
	{
		TRACE_DEBUG("i:%02Xh\n", i);

			if ( 0 ) {}
#if defined(__CUDA__)
			else if ( IS_CUDA(flags) )
			{
				result = PCRTT::Cuda::CsaRainbowFindA(Cw6EndArray,
							RoundNrArray,CryptArray,&Debug);
			}
#endif // __CUDA__
#if defined(__OPENCL__)
			else if ( IS_OPENCL(flags) )
			{
				result = PCRTT::OpenCL::CsaRainbowFindA(Cw6EndArray,
							RoundNrArray,CryptArray);
			}
#endif // __OPENCL__
#if defined(__VULKAN__)
			else if ( IS_VULKAN(flags) )
			{
				result = PCRTT::Vulkan::CsaRainbowFindA(Cw6EndArray,
							RoundNrArray,CryptArray,&Debug);
			}
#endif // __VULKAN__

//		result = CsaRainbowFindA(Cw6EndArray,RoundNrArray,CryptArray,&Debug);

		assert( result == 0 );
	}

	for(i=0; i<Elements; ++i)
	{
		assert( RoundNrArray[i] == Elements );
	}

	fp = fopen( filename, "wb" );

	if( !fp )
	{
		// free memory
		KILLARRAY( CryptArray );
		KILLARRAY( RoundNrArray );
		KILLARRAY( Cw6EndArray );

		assert(!"Can't open dst file");
		return;
	}

	fwrite( Cw6EndArray, 1, Elements * 6, fp );
	fclose( fp );

	// free memory
	KILLARRAY( CryptArray );
	KILLARRAY( RoundNrArray );
	KILLARRAY( Cw6EndArray );
}

} // end of namespace Shared
} // end of namespace PCRTT
