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
#if !defined(PCRTT_SRC_SHARED_SEARCHCW_HEADER_H_)
#define PCRTT_SRC_SHARED_SEARCHCW_HEADER_H_

#include "crypt8finder.h"

namespace PCRTT {
namespace Shared {

/**
 * static class to manage
 * all action on search CW
 */
class SearchCw
{
public:
	/** global action */
	static void start(Worker_t* worker);

	static void display_cw(const char* str);

	static int get_key_from_cw_start(const uint flags, uchar *Cw6,
		const uchar *Crypt, uchar_vp& pCw6StartEndIdxList);

	static void CalcRainbowEndValueArrayFromCrypt(const uint flags,
				const uchar *Crypt, const String& DstDirName);

	static inline void Cw6ToCw8(uchar *Cw8, const uchar *Cw6)
	{
		Cw8[0] = Cw6[0], Cw8[1] = Cw6[1], Cw8[2] = Cw6[2];
		Cw8[3] = Cw8[0] + Cw8[1] + Cw8[2], Cw8[4] = Cw6[3],
		Cw8[5] = Cw6[4], Cw8[6] = Cw6[5], Cw8[7] = Cw8[4] + Cw8[5] + Cw8[6];
	}

}; // end of class SearchCw

} // end of namespace Shared
} // end of namespace PCRTT

#endif // PCRTT_SRC_SHARED_SEARCHCW_HEADER_H_
