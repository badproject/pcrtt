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
#if !defined(PCRTT_SRC_CORE_MEMALLOC_HEADER_H_)
#define PCRTT_SRC_CORE_MEMALLOC_HEADER_H_

// very simple linear memory allocator, do not use it !

namespace PCRTT {

class MemAlloc
{
public:
	static int initMemoryManager(const uint& size);
	static void freeMemoryManager(void);

	static uint countMemoryUsed(void);
	static void* aligned_malloc( int size, uint alignment );
	static void aligned_free(void *p);

#if defined( _DEBUG )
	/** \brief todo */
	//! \brief todo
	static uchar* _getMemory(const uint& size, const char* filename, int line);

	/** \brief todo */
	//! \brief todo
	static void _freeMemory(const uint& size, const char* filename, int line );

	/** \brief todo */
	//! \brief todo
	static uchar* _getMemory_aligned(
	const uint& size, const uint& alignment, const char* filename, int line );

	/** \brief todo */
	//! \brief todo
	static void _freeMemory_aligned(
	const uint& size, const uint& alignment, const char* filename, int line );

	#define Core_getMemory(size) \
		MemAlloc::_getMemory( size, __FILE__, __LINE__ )
	#define Core_getMemory_aligned(size,alignment) \
		MemAlloc::_getMemory_aligned( size, alignment, __FILE__, __LINE__ )
	#define Core_freeMemory(size) \
		MemAlloc::_freeMemory( size, __FILE__, __LINE__ )
	#define Core_freeMemory_aligned(size,alignment) \
		MemAlloc::_freeMemory_aligned( size, alignment, __FILE__, __LINE__ )
	#else
	/** \brief todo */
	//! \brief todo
	static uchar* _getMemory(const uint& size );
	/** \brief todo */
	//! \brief todo
	static void _freeMemory(const uint& size );

	/** \brief todo */
	//! \brief todo
	static uchar* _getMemory_aligned(const uint& size, const uint& alignment );
	/** \brief todo */
	//! \brief todo
	static void _freeMemory_aligned(const uint& size, const uint& alignment );

	#define Core_getMemory(size) \
		MemAlloc::_getMemory( size )
	#define Core_getMemory_aligned(size,alignment) \
		MemAlloc::_getMemory_aligned( size, alignment )
	#define Core_freeMemory(size) \
		MemAlloc::_freeMemory( size )
	#define Core_freeMemory_aligned(size,alignment) \
		MemAlloc::_freeMemory_aligned( size, alignment )
	#endif	// _DEBUG

}; // end of class MemAlloc

} // namespace PCRTT

#endif // PCRTT_SRC_CORE_MEMALLOC_HEADER_H_
