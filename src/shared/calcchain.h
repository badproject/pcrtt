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
#if !defined(PCRTT_SRC_SHARED_CALCCHAIN_HEADER_H_)
#define PCRTT_SRC_SHARED_CALCCHAIN_HEADER_H_

namespace PCRTT {
namespace Shared {

void Bin2String(String& pString, const uchar *data, int size);

/** global function */

/**
 * static class to manage
 * calc chain
 */
class ClacChain
{
public:
	/** global action */
	static int start(Worker_t* worker);

	static int CsaRainbow1(Worker_t* worker, uchar *Cw6EndArray_h,
		const uchar *Cw6StartArray_h,ulong StartRoundNr);

#if defined(__CUDA__)
	/** cuda specific */
	class Cuda
	{
	public:
		static int CsaRainbow1(uchar *,const uchar *,ulong,DEBUG_STRUCT *);
	}; // end of class Cuda
#endif // __CUDA__

#if defined(__OPENCL__)
	/** opencl specific */
	class OpenCL
	{
	public:
		static int CsaRainbow1(uchar *,const uchar *,ulong,DEBUG_STRUCT *);
	}; // end of class OpenCL
#endif // __OPENCL__

#if defined(__VULKAN__)
	/** vulkan specific */
	class Vulkan
	{
	public:
		static int CsaRainbow1(uchar *,const uchar *,ulong,DEBUG_STRUCT *);
	}; // end of class Vulkan
#endif // __VULKAN__
}; // end of class ClacChain

} // end of namespace Shared
} // end of namespace PCRTT

#endif // PCRTT_SRC_SHARED_CALCCHAIN_HEADER_H_
