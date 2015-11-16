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
#if !defined(PCRTT_SRC_SHARED_SHARED_HEADER_H_)
#define PCRTT_SRC_SHARED_SHARED_HEADER_H_

namespace PCRTT {
namespace Shared {

/** global function */
void printBuffer(const char * fileName, int lineNum, uchar* data, int size);

void enc23( uchar *Crypt, const uchar *Cw6 );

void ensureDirNameHasBackslashEnding(String& pDirName);

int getStartCw6FromFileName(const String& FileName, uchar *Cw6);

void SaveLoadSettings(int Save);

string_v getStringDirectoryListWithFullPath(const char* path, int subDir);

static inline void RainbowXorRoundNr(uchar *Cw6, ulong RoundNr)
{
	//Reduction part 2 (XOR RoundNr)
	Cw6[2] ^= ((RoundNr >> 24) & 0xFF);
	Cw6[3] ^= ((RoundNr >> 16) & 0xFF);
	Cw6[4] ^= ((RoundNr >> 8) & 0xFF);
	Cw6[5] ^= (RoundNr & 0xFF);
}

} // end of namespace Shared
} // end of namespace PCRTT

#endif // PCRTT_SRC_SHARED_SHARED_HEADER_H_
