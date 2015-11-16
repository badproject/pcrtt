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
#include <shared/crypt8.h>
#include <shared/crypt8finder.h>

namespace PCRTT {
namespace Shared {

/** ---------------------------------------------------------------------------
 *  -- Crypt8 -----------------------------------------------------------------
 *  ---------------------------------------------------------------------------
 */

//int SearchCrypt8InTsStart()
int Crypt8::start(Worker_t* worker)
{
	String Text, ts_src, ts_limit, ts_pid, ts_payload_size;
	File TsSrcFile;
	int FileLimitInMB, RefPid, EnablePidFilter, PayloadSize;
	uint64_t FileLen, Time, Delta;

	ts_src = getSearchCrypt8InTsSrc();
	ts_limit = getSearchCrypt8InTsFileLimit();
	ts_pid = getSearchCrypt8InTsPid();
	ts_payload_size = getSearchCrypt8InTsPayloadSize();

	Time = get_time();
	
	clear_output_gui();

	SetButtonStateSearchCrypt8InTsStart();
	output_gui("Search Crypt8 in TS Start\n");
	//pTI->fSearchCrypt8InTsStart = false;//got it

	if( !ts_src.size() )
	{
		TRACE_DEBUG("ERROR: TS file name is empty\n");
		SetButtonStateSearchCrypt8InTsStop();
		return 0;
	}

	EnablePidFilter = false;
	if( ts_pid.size() )
	{
		// not empty -> filter for a special PID
		EnablePidFilter = true;

		RefPid = atoi( ts_pid.c_str() );

		//check lower and upper limit
		if(RefPid < 0)
		{
			TRACE_DEBUG("ERROR: PID is negative\n");
			SetButtonStateSearchCrypt8InTsStop();
			return 0;
		}
		if(RefPid > 0x1FFF)
		{
			TRACE_DEBUG("ERROR: PID %d is greater than %d.\n", RefPid, 0x1FFF);
			SetButtonStateSearchCrypt8InTsStop();
			return 0;
		}
	}

	PayloadSize = 184;//default value

	if( ts_payload_size.size() )
	{
		//not empty
		PayloadSize = atoi( ts_payload_size.c_str() );
	}

	//check limits
	if(PayloadSize < 0)
	{
		TRACE_DEBUG("ERROR: Payload size is negative\n");
		SetButtonStateSearchCrypt8InTsStop();
		return 0;
	}
	if(PayloadSize == 0)
	{
		TRACE_DEBUG("ERROR: Payload size is zero.\n");
		SetButtonStateSearchCrypt8InTsStop();
		return 0;
	}
	if(PayloadSize > 184)
	{
		TRACE_DEBUG("ERROR: Payload size greater than 184.\n");
		SetButtonStateSearchCrypt8InTsStop();
		return 0;
	}
	if((PayloadSize % 8) != 0)
	{
		TRACE_DEBUG("ERROR: Payload size %d isn't a multiple of 8.\n",
			PayloadSize);
		SetButtonStateSearchCrypt8InTsStop();
		return 0;
	}
	if(PayloadSize != 184)
	{
		TRACE_DEBUG("WARNING: The payload size %d isn't 184. The crypt8 can't"
			" be used to calculate the CW with this tool\n", PayloadSize);
	}

	FileLimitInMB = atoi( ts_limit.c_str() );

	if ( !ts_limit.size() )
	{
		//empty -> use max. supported value
		FileLimitInMB = SEARCH_CRYPT8_IN_TS_MAX_FILE_SIZE_MB;
	}

	//check lower and upper limit
	if ( FileLimitInMB <= 0 )
	{
		TRACE_DEBUG("ERROR: File limit must be greater then 0 MByte\n");
		SetButtonStateSearchCrypt8InTsStop();
		return 0;
	}

	if(FileLimitInMB > SEARCH_CRYPT8_IN_TS_MAX_FILE_SIZE_MB)
	{
		TRACE_DEBUG("ERROR: File limit must be lower or eqal %d MByte\n",
			SEARCH_CRYPT8_IN_TS_MAX_FILE_SIZE_MB);
		SetButtonStateSearchCrypt8InTsStop();

		return 0;
	}

	//Using the following results
	output_gui("TS file: %s\n", ts_src.c_str() );

	if(EnablePidFilter)
	{
		output_gui("Using PID: %d\n", RefPid);
	}
	else
	{
		output_gui("Using all PIDs\n");
	}

	output_gui("Using payload size: %d\n"
		"Using file limit: %d MByte\n", PayloadSize, FileLimitInMB);

	if( !TsSrcFile.Open(ts_src.c_str(), "rb") )
	{
		TRACE_ERROR("Can't open file \"%s\"\n", ts_src.c_str() );
		SetButtonStateSearchCrypt8InTsStop();

		return 0;
	}
	
	FileLen = TsSrcFile.GetSize();

	output_gui("File length: %llu MByte\n", (FileLen / 1024) / 1024);
	uint64_t UsingFileLen;
	UsingFileLen = FileLen;
	if(((UsingFileLen / 1024) / 1024) > FileLimitInMB)
	{
		UsingFileLen = FileLimitInMB * 1024 * 1024;
	}

	output_gui("Using FileLen: %llu bytes\n", UsingFileLen);

	int i, Packets, RemPackets, TempPackets;
	uchar *Buffer;
	int BufferSizeInPackets = 100 * 1024;

	Packets = UsingFileLen / TS_SIZE;

	Buffer = new uchar[BufferSizeInPackets * TS_SIZE];
	if ( !Buffer )
	{
		TRACE_ERROR("ERROR: Alloc memory failed\n");
		SetButtonStateSearchCrypt8InTsStop();
		return 0;
	}

	RemPackets = Packets;

	struct FindC8Info_t *pInfo;
	Crypt8Finder finder;
//	static int Cnt = 0;

	output_gui("Reading file ...\n");
	while(RemPackets > 0)
	{
		//if(pTI->fSearchCrypt8InTsStop || pTI->fExitThread)
		if ( worker->job == C8_STOP || worker->job == STOP_JOB )
		{
			SetButtonStateSearchCrypt8InTsStop();
			output_gui("Search Crypt8 in TS Stop\n");
			//pTI->fSearchCrypt8InTsStop = false;//got it
			KILLARRAY(Buffer);

			return 1;
		}

		worker->stats.ProcSearchCrypt8InTsFile = 
			(float)(Packets - RemPackets) * 100.0 / (float)Packets;

		UpdateStatistic( &worker->stats, Stats_t::CRYPT8 );

		TempPackets = M_MIN(RemPackets, BufferSizeInPackets);
		TsSrcFile.Read(Buffer, TempPackets * TS_SIZE);

		for(i=0; i<TempPackets; i++)
		{
			ushort Pid;
			uchar fOddCw;
			int fAf, fPayload;
			
			const uchar *Ts = &Buffer[i * TS_SIZE];

			if(Ts[0] != SYNC_BYTE)
			{
				assert(false);
				//not a sync byte -> skip
				continue;
			}

			Pid = ((Ts[1]<<8)|Ts[2]) & 0x1FFF;
			if(EnablePidFilter)
			{
				if(Pid != RefPid)
				{
					continue;//skip wrong pid
				}
			}

			if(((Ts[3]>>6) & 0x02) == 0)
			{
				//not scrambled -> skip
				continue;
			}
			fOddCw = (Ts[3]>>6) & 0x01;

			int Crypt8Pos;
			int fFound;

			fAf = (Ts[3]>>5) & 0x01;
			fPayload = (Ts[3]>>4) & 0x01;

			fFound = false;
			if(PayloadSize == 184)
			{
				//serach for packets without AF but payload
				if((!fAf) && fPayload)
				{
					//match
					Crypt8Pos = 4;
					fFound = true;
				}			
			}
			else
			{
				//serach for packets with AF and payload
				if(fAf && fPayload)
				{
					//check payload len
					int TempPayloadSize;
					TempPayloadSize = 184 - (1 + Ts[4]);
					if(TempPayloadSize >= 8)
					{
						/**
						 * ignore 3 lsb, because they are not used
						 * in block cipher
						 */
						if((TempPayloadSize & 0xF8) == PayloadSize)
						{
							Crypt8Pos = 4 + 1 + Ts[4];
							fFound = true;
						}				
					}
				}			
			}
			
			if(fFound)
			{
				finder.add( Pid, fOddCw, &Ts[Crypt8Pos] );
				/*{
					//error
					SetButtonStateSearchCrypt8InTsStop();
					TRACE_DEBUG("ERROR: AddCrypt8InfoElement failed\n");
					KILLARRAY(Buffer);
					return;
				}*/
			}
		}
	
		RemPackets -= TempPackets;
	}

	KILLARRAY(Buffer);

	worker->stats.ProcSearchCrypt8InTsFile = (float)0;//done
	//SendToGui(ID_MSG1, UPDATE_STATISTIC, (LPARAM)0);
	UpdateStatistic( &worker->stats, Stats_t::CRYPT8 );

	output_gui("Searching ...\n");

	if( !finder.search() )
	{
		// error
		SetButtonStateSearchCrypt8InTsStop();
		TRACE_ERROR("function Search() failed\n");

		return 0;
	}

	// show results
	String result( finder.results() );

	if ( result.size() )
	{
		output_gui("[O] == odd [E] == even\n");
		output_gui( result );
	}

	// done
	Delta = get_time() - Time;
	output_gui("Time for searching Crypt8 = %llu sec.\n", Delta / 1000 );

	SetButtonStateSearchCrypt8InTsStop();

	return 1;
}

} // end of namespace Shared
} // end of namespace PCRTT
