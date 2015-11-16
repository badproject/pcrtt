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

#include <time.h> // for clock
#include <math.h> // for fmod
#include <cstdlib> //for system
#include <stdio.h> //for delay

#include <string.h>

#include <sched.h>
#include <linux/unistd.h>
#include <sys/syscall.h>

#include <shared/define.h>
#include <shared/shared.h>
#include <shared/rainbow.h>
#include <shared/calcchain.h>
#include <shared/cpu.h>

namespace PCRTT {
namespace Shared {


/** ---------------------------------------------------------------------------
 *  -- local function ---------------------------------------------------------
 *  ---------------------------------------------------------------------------
 */

static inline void SetRndPrefix(uchar *Cw6)
{
	memset( Cw6, 0x00, sizeof(Cw6) );

#if 0 // force to use only this prefix (debug only, keep me)
	Cw6[0] = 0xFF;
	Cw6[1] = 0x3C;
	Cw6[2] = 0xB4;
#else
	Cw6[0] = rand() & 0xFF;
	Cw6[1] = rand() & 0xFF;
	Cw6[2] = rand() & 0xFC;
#endif // debug
}

static String getChainFileNameFromCw6(const uchar *Cw6)
{
	String filename( format("%s_%s_%Xh_%02X%02X%02Xh.%s", 
		FILE_PART1_ALGO, FILE_PART2_PLAIN, RAINBOW_CHAIN_LEN, Cw6[0], Cw6[1],
		Cw6[2] & 0xFC, FILE_EXT_RB_CHAINS
	));
	return filename;
}

static String getFileNameForChainCalc()
{
	// String filename;
	String filename;
	//CFileFind finder;
	String ErrorText, FileMask, filenameWithoutPath;
	File file;
	uchar Cw6[6];
	int FoundFile = false;
	String chain_dir( getCalcChainChainDir() );

	FileMask = format("%s_%s_%Xh_*h.%s", 
		FILE_PART1_ALGO, FILE_PART2_PLAIN, 
		RAINBOW_CHAIN_LEN, FILE_EXT_RB_CHAINS );

	string_v ar = getStringDirectoryListWithFullPath( chain_dir, 1 );

	for(int i=0; i < ar.size(); ++i )
	{
		filename = ar[i];

		TRACE_DEBUG("FindNextFile: %s\n", filename.c_str());

		uint64_t FileLen = File::Size( filename );

		if((FileLen % (6+6)) != 0)
		{
			TRACE_ERROR("ERROR: The file \"%s\" has a length (%llu) that "
					"isn't a multiple of (6+6)\n", 
				filename.c_str(), FileLen);

			continue;
		}

		if(FileLen >= (MAX_ELEMENTS_PER_CHAIN_FILE * (6+6)))
		{
			//nothing to do -> file is complete already
			TRACE_ERROR( "The file \"%s\" is complete already\n", 
				filename.c_str(), FileLen);
			continue;
		}

		if(!getStartCw6FromFileName(filename, Cw6))
		{
			//error getting prefix from file name
			TRACE_ERROR("ERROR: Can't get prefix from file name \"%s"
							"\"\n", filename.c_str() );
			continue;
		}

		//filenameWithoutPath = filename;

		if( !file.Open(filename.c_str(), "rb") )
		{
			TRACE_ERROR("ERROR: File \"%s\" is in use already\n",	
							filename.c_str() );
			continue;
		}

		ErrorText = Rainbow::ValidateChainFile(&file, Cw6);

		if ( ErrorText.size() )
		{
			TRACE_ERROR( ErrorText );
			continue;
		}

		TRACE_DEBUG("\"%s\" is ok\n", String::basename(filename).c_str() );

		FoundFile = true;
		break;
	}

	if(!FoundFile)
	{
		for(;;)
		{
			SetRndPrefix(Cw6);
			filenameWithoutPath = getChainFileNameFromCw6(Cw6);

			filename = format( "%s/%s", chain_dir.c_str(),
						filenameWithoutPath.c_str() );

			if( File::Exists( filename ) )
			{
				//file exists already
				TRACE_ERROR("ERROR: Random file \"%s\" exists "
								"already\n", filename.c_str() );
			}
			else
			{
				//open failed -> unique file name found
				break;
			}
		}
	}

	//filename = format( "%s/%s", 
	//	chain_dir.c_str(), filenameWithoutPath.c_str() );
	
	TRACE_DEBUG("filename: %s", filename.c_str() );

	return filename;
}

/** ---------------------------------------------------------------------------
 *  -- calc chain -------------------------------------------------------------
 *  ---------------------------------------------------------------------------
 */

int ClacChain::CsaRainbow1(Worker_t* worker, uchar *a, const uchar *b, ulong c)
{
	/** 
	 * a == Cw6EndArray_d,
	 * b == Cw6StartArray_d
	 * c == StartRoundNr
	 */
	return start_cpu_rainbow_1(a,b,c);

#if defined(__CUDA__)
	if ( IS_CUDA(worker->flags) )
	{
		return CsaCudaRainbow1(a,b,c,d);
	}
#endif // __CUDA__
#if defined(__OPENCL__)
	if ( IS_OPENCL(worker->flags) )
	{
		return PCRTT::OpenCL::CsaRainbow1(a,b,c);
	}
#endif // __OPENCL__
#if defined(__VULKAN__)
	if ( IS_VULKAN(worker->flags) )
	{
		return CsaVulkanRainbow1(a,b,c,d);
	}
#endif // __VULKAN__

	// fall back to cpu worker
	return start_cpu_rainbow_1(a,b,c);
}

// CalcChain renamed to : ClacChain::start
struct TimeToFinish_t
{
	void start()
	{
		clock_gettime(CLOCK_MONOTONIC, &_start);
	}
	void stop()
	{
		 clock_gettime(CLOCK_MONOTONIC, &_stop);
	}

	void update(uint64_t size)
	{
		#define NB_ELEMENTS (BLOCKS_RB * THREADS_RB)

		double duration_d = (double)(_stop.tv_sec - _start.tv_sec) + 
							(_stop.tv_nsec - _start.tv_nsec) / 1000000000.0;

		//double bytes_sec = (NB_ELEMENTS/1024);
		#define  _1Ko (1024)
		#define  _1Mo (1024*1024)

		if ( size >= _1Mo )
			printf("Elapsed: %.5lfs %u Mo\n", duration_d, size / _1Mo);
		else if ( size >= _1Ko)
			printf("Elapsed: %.5lfs %u Ko\n", duration_d, size / _1Ko);
		else 
			printf("Elapsed: %.5lfs %u Ko\n", duration_d, size);
	}

	struct timespec _start, _stop;
};

int ClacChain::start(Worker_t* worker)
{
	String filename, chain_dir;
	Rainbow RBTab;
	uchar *Cw6StartArray, *Cw6EndArray, *Cw6TempArray, NextCw6Start[6];
	ulong Elements, StartRoundNr, ElementsInFile;
	int i;
	File file;
	DEBUG_STRUCT Debug;
	uint64_t TimeOld = 0, Time = 0;
	String ErrorText;
	//TimeToFinish_t ttfinish;

	SetButtonStateCalcChainStart();

	chain_dir = getCalcChainChainDir();

	// pTI->fCalcChainStart = false; // got it
	TRACE_DEBUG("Calc Chain Start\nChain Dir: %s\n", chain_dir.c_str() );

	if ( !chain_dir.size() )
	{
		TRACE_ERROR("ERROR: Directory name is empty\n");
		SetButtonStateCalcChainStop();
		return 0;
	}

	// alloc memory
	Elements = BLOCKS_RB * THREADS_RB;

	Cw6StartArray = new uchar[Elements*6];
	Cw6EndArray = new uchar[Elements*6];
	Cw6TempArray = new uchar[Elements*6];

	// calculate
	assert((RAINBOW_CHAIN_LEN % RAINBOW_CHAIN_LEN_PER_KERNEL) == 0);

	int FileIsOpen = false;

	for( i=0; (worker->job != CHAIN_STOP) && (worker->job != STOP_JOB); )
	{
		if(!FileIsOpen)
		{
			filename = getFileNameForChainCalc();

			TRACE_DEBUG("getFileNameForChainCalc: %s\n", filename.c_str() );

			if( !file.Open( filename.c_str(), "a+") )
			{
				TRACE_ERROR("ERROR: Can't open file \"%s\"\n",
						filename.c_str() );

				SetButtonStateCalcChainStop();

				KILLARRAY(Cw6StartArray);
				KILLARRAY(Cw6EndArray);
				KILLARRAY(Cw6TempArray);

				return 0; // error
			}
			FileIsOpen = true;
		}

		ErrorText = Rainbow::GetInfoFromChainFile( NextCw6Start,
					&ElementsInFile, &file );

		if ( ErrorText.size() )
		{
			TRACE_ERROR(ErrorText);
			SetButtonStateCalcChainStop();

			KILLARRAY(Cw6StartArray);
			KILLARRAY(Cw6EndArray);
			KILLARRAY(Cw6TempArray);

			return 0;
		}

		if(ElementsInFile == 0)
		{
			//Empty file -> get prefix from file name
			if( !getStartCw6FromFileName(file.GetName(), NextCw6Start) )
			{
				//error getting prefix from file name
				TRACE_ERROR("ERROR: Can't get prefix from file name \"%s\"\n",
					file.GetName().c_str() );

				SetButtonStateCalcChainStop();

				KILLARRAY(Cw6StartArray);
				KILLARRAY(Cw6EndArray);
				KILLARRAY(Cw6TempArray);

				return 0;
			}
		}

		Rainbow::FillCw6ArrayWithIncCw6Start( Cw6StartArray, Elements,
			NextCw6Start);

		memcpy( Cw6TempArray, Cw6StartArray, Elements * 6 );

		String StartChainStr(
			format("%02X%02X%02X%02X", Cw6TempArray[0], Cw6TempArray[1],
			Cw6TempArray[2], Cw6TempArray[3])
		);

		//calc part chains
		//ttfinish.start();
		int k = 0;
		for( StartRoundNr = 0 ; StartRoundNr < RAINBOW_CHAIN_LEN ; )
		{
			if ( CHAIN_STOP_ASKED || STOP_JOB_ASKED )
			{
				TRACE_DEBUG("stop requested.");
				break;
			}
			//if ( CHAIN_START_ASKED )
			//{
				//SetButtonStateCalcChainStart();
				//TRACE_DEBUG("Calc Chain Start\n");
				//pTI->fCalcChainStart = false;//got it
			//}

			if ( CHAIN_PAUSE_ASKED )
			{
				TRACE_DEBUG("Calc Chain Pause\n");
				SetButtonStateCalcChainPause();
				worker->job = NO_JOB;

				//pTI->fCalcChainPause = false;//got it

				//send statistic to GUI
				worker->stats.DeltaTime = 0;
				worker->stats.kCW_per_sec = 0;
				//SendToGui(ID_MSG1, UPDATE_STATISTIC, (LPARAM)0);
				UpdateStatistic( &worker->stats, Stats_t::CALC );

				while(
					!CHAIN_START_ASKED && !CHAIN_STOP_ASKED && !STOP_JOB_ASKED
				) {
					usleep( 2000 ); // Keep CPU utilization low during pause
				}
				if ( CHAIN_START_ASKED )
					worker->job = NO_JOB;
				continue;
			}

			TimeOld = Time;
			Time = get_time();
			uint64_t Delta;
			ulong kKeysSec;

			Delta = (Time - TimeOld);
			if(Delta > 0)
			{
				kKeysSec = (BLOCKS_RB*THREADS_RB*RAINBOW_CHAIN_LEN_PER_KERNEL)/Delta;
			}
			else
			{
				kKeysSec = 0;
			}

			// fake for now :
			// todo: reactivate me
			//printBuffer(0,0,Cw6EndArray,8);
			CsaRainbow1(worker,Cw6EndArray,Cw6TempArray,StartRoundNr);
//			usleep( 1000 );

			memcpy( Cw6TempArray, Cw6EndArray, Elements * 6 );

		//	if( 1 )
		//	{
				//TRACE_DEBUG("i:%3d k:%02X / Delta: %llu /  %ld kK/s / %s\n", 
				//	i, k++, Delta, kKeysSec, StartChainStr.c_str() );
		//	}

			StartRoundNr += RAINBOW_CHAIN_LEN_PER_KERNEL;

			// send statistic to GUI
			worker->stats.DeltaTime = Delta;
			worker->stats.kCW_per_sec = kKeysSec;
			worker->stats.ProcCalcChainPart = 
				(float) StartRoundNr * 100.0 / (float)RAINBOW_CHAIN_LEN;

			//SendToGui(ID_MSG1, UPDATE_STATISTIC, (LPARAM)0);
			UpdateStatistic( &worker->stats, Stats_t::CALC );

			//printBuffer( 0,0, Cw6TempArray, 8 );
		}
		//ttfinish.stop();
		//ttfinish.update(ElementsInFile);

		if( !CHAIN_STOP_ASKED && !STOP_JOB_ASKED )
		{
			if((ElementsInFile + Elements) > MAX_ELEMENTS_PER_CHAIN_FILE)
			{
				//don't write to many elements to file
				Elements = MAX_ELEMENTS_PER_CHAIN_FILE - ElementsInFile;
			}
			Rainbow::SaveCwStartAndEndArray(Cw6StartArray, Cw6EndArray,
				Elements, &file);
			ElementsInFile += Elements;

			if(ElementsInFile >= MAX_ELEMENTS_PER_CHAIN_FILE)
			{
				file.Close();
				FileIsOpen = false;
			}
		}

		//send statistic to GUI
		worker->stats.ProcCalcChainFile = 
			(float)ElementsInFile * 100.0 / (float)MAX_ELEMENTS_PER_CHAIN_FILE;
		//SendToGui(ID_MSG1, UPDATE_STATISTIC, (LPARAM)0);
		UpdateStatistic( &worker->stats, Stats_t::CALC );

		++i;
	}

	if ( worker->job == CHAIN_STOP )
	{
		SetButtonStateCalcChainStop();
		TRACE_DEBUG("Calc Chain Stop\n");

		//pTI->fCalcChainStop = false;//got it
	}

	// send statistic to GUI
	worker->stats.DeltaTime = 0;
	worker->stats.kCW_per_sec = 0;
	worker->stats.ProcCalcChainPart = 0;

	//SendToGui(ID_MSG1, UPDATE_STATISTIC, (LPARAM)0);
	UpdateStatistic( &worker->stats, Stats_t::CALC );

	// free memory
	KILLARRAY( Cw6StartArray );
	KILLARRAY( Cw6EndArray );
	KILLARRAY( Cw6TempArray );

	return 1;
}

} // end of namespace Shared
} // end of namespace PCRTT
