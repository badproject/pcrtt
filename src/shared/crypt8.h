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
#if !defined(PCRTT_SRC_SHARED_CRYPT8_HEADER_H_)
#define PCRTT_SRC_SHARED_CRYPT8_HEADER_H_

namespace PCRTT {
namespace Shared {

/**
 * static class to manage
 * all action on Crypt8
 */
class Crypt8
{
public:
	/** global action */
	static int start(Worker_t* worker);

#if defined(__CUDA__)
	/** cuda specific */
	class Cuda
	{
	public:
		
	}; // end of class Cuda
#endif // __CUDA__

#if defined(__OPENCL__)
	/** opencl specific */
	class OpenCL
	{
	public:
		
	}; // end of class OpenCL
#endif // __OPENCL__

#if defined(__VULKAN__)
	/** vulkan specific */
	class Vulkan
	{
	public:
		
	}; // end of class Vulkan
#endif // __VULKAN__
}; // end of class Crypt8

} // end of namespace Shared
} // end of namespace PCRTT

#endif // PCRTT_SRC_SHARED_CRYPT8_HEADER_H_
