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
#include <core/memalloc.h>

#define DEF_ALLOC_SIZE	(810 * (1024 * 1024)) // 810Mo

namespace PCRTT {

static ushort locked = 0;

#define AUTO_LOCK_MTX

struct MemoryVec_t
{
	void* data;
	uint size;
};

static Vector<MemoryVec_t> __memoryvec;
static uchar* _globalMemory = 0;
static uint _allocated_size = 0;
static uint _currentPosition = 0;

void* MemAlloc::aligned_malloc( int size, uint alignment )
{
	std::cout << "aligned_malloc(size:" << size << ", alignment: "
		<< alignment <<'\n';
#if defined(HAVE_ALIGNED_ALLOC)
	std::cout <<"using sys > aligned_alloc()"<<'\n';
	return ::aligned_alloc( alignment, size );
#elif defined(HAVE__ALIGNED_MALLOC)
	std::cout << "using sys > _aligned_malloc()"<<'\n';
	return ::_aligned_malloc(size, alignment);
#elif defined(HAVE_POSIX_MEMALIGN)
	std::cout <<"using sys > posix_memalign()"<<'\n';
	void* ptr;
	int rc = ::posix_memalign( &ptr, alignment, size );
	if ( rc != 0 )
	{
		if ( rc == EINVAL )
		{
			assert(!"align is not a power of two.");
		}
		else if ( rc == ENOMEM ) 
		{
			assert(!"Not enought memory.");
		}
		else
		{
			assert(!"posix_memalign()");
		}
	}
	return ptr;
#elif defined(HAVE_MALLOC)
	#define ALIGN 64
	std::cout <<"using sys > malloc()"<<'\n';
	void *mem = ::malloc(size+ALIGN+sizeof(void*));
    void **ptr = (void**)((long)(mem+ALIGN+sizeof(void*)) & ~(ALIGN-1));
    ptr[-1] = mem;
    #undef ALIGN
    return ptr;
#else
	// todo
	#error NO MALLOC FUNCTION FOUND
#endif	//
}

void MemAlloc::aligned_free(void *p)
{
	if(!p)
		return;
#if defined(HAVE_ALIGNED_ALLOC)
	::free(p);
#elif defined(HAVE__ALIGEND_FREE)
	::_aligned_free(p);
#elif defined(HAVE_POSIX_MEMALIGN)
	::free(p);
#else // defined(__WIN32__)
	::free(((void**)p)[-1]);
#endif	// WIN32
} 

void MemAlloc::freeMemoryManager(void)
{
	TRACE_DEBUG("freeMemoryManager()");

	//KILLARRAY( _globalMemory );

	aligned_free( _globalMemory );

	//TRACE_DEBUG("freeMemoryManager() - step 2");

	_globalMemory		= 0;
	_currentPosition	= 0;
	_allocated_size		= 0;

	//TRACE_DEBUG("freeMemoryManager() - step 3");

	for (int i =0; i < __memoryvec.size(); ++i)
	{
		//KILLARRAY( __memoryvec[i] );
		aligned_free( __memoryvec[i].data );
	}

	//TRACE_DEBUG("freeMemoryManager() - step 4");

	__memoryvec.clear();
	
	//TRACE_DEBUG("freeMemoryManager() - step 5");
}

uint MemAlloc::countMemoryUsed(void)
{
	uint size = 0;

	for (int i =0; i < __memoryvec.size(); ++i)
	{
		size += __memoryvec[i].size;
	}

	//return (_allocated_size + size);
	return (size);
}

#define DEF_ALIGN 64

int MemAlloc::initMemoryManager(const uint& size)
{
	std::cout << "initMemoryManager("<< size <<")" <<'\n';

	// sanity check 
	if ( !size )
	{
		TRACE_ERROR("error with size (size == 0)");
		return false;
	}
	//TRACE_DEBUG(" Step 2");

	// check if we already got some memory
	if ( _allocated_size >= size )
		return true;
	//TRACE_DEBUG(" Step 3");

	// clean any old buffer
	freeMemoryManager();
	//TRACE_DEBUG(" Step 4");

	// allocate new buffer
	_allocated_size = size;
	TRACE_DEBUG(" Step 5");

	_globalMemory = (uchar*)aligned_malloc( _allocated_size, DEF_ALIGN ); // new uchar[ _allocated_size ];
	//TRACE_DEBUG(" Step 6");

	// check for error
	if ( !_globalMemory )
	{
		TRACE_ERROR("cant allocate memory manager with a size of %d",
			_allocated_size);
		_allocated_size = 0;
		return false;
	}
	//TRACE_DEBUG(" Step 7");

	// zero memory
	memset( _globalMemory, 0x00, _allocated_size );

	//TRACE_DEBUG(" Step 8 - done");

	return true;
}

#if defined( _DEBUG )
void MemAlloc::_freeMemory(const uint& size, const char* filename, int line )
#else
void MemAlloc::_freeMemory(const uint& size )
#endif
{
	//TRACE_DEBUG("freeMemory(" << size << " , " << filename << " , " << line << " )");

	_currentPosition -= size;

	(void)size;
#if defined( _DEBUG )
	(void)line;
	(void)filename;
#endif
}

#if defined( _DEBUG )
uchar* MemAlloc::_getMemory(const uint& size, const char* filename, int line )
{
	TRACE_DEBUG( "getMemory(%d) - %s@%d", size, filename, line );
#else
uchar* MemAlloc::_getMemory(const uint& size )
{
#endif	// DEBUG
	// sanity check 
	if ( _globalMemory == 0 )
	{
		// was not init.. trying to init 650Mo
		if ( !initMemoryManager( DEF_ALLOC_SIZE ) ) // 5Mo by default
		{
			TRACE_ERROR("Can't allocate %d Mo of memory, trying less %d Mo",
				DEF_ALLOC_SIZE/1024/1024, (DEF_ALLOC_SIZE / 2)/1024/1024 );

			if ( !initMemoryManager( DEF_ALLOC_SIZE / 2 ) )
				TRACE_ERROR("Can not allocate memory for memory manager");
		}
	}

	uint pos = _currentPosition;
	_currentPosition += size;
	uchar* data = 0;

#ifdef _DEBUG
	if ( _currentPosition >= _allocated_size )
	{
		// warning
		std::cout <<
			"warning, you have not allocated enought memory!\npos: " <<
			_currentPosition <<	" - allocated: " << _allocated_size <<
			" - needed: " << (_currentPosition - _allocated_size) <<'\n'
		;

		// allocate new memory...
		data = (uchar*)aligned_malloc( size, DEF_ALIGN ); // new uchar[ size ];

		MemoryVec_t m;

		m.data = data;
		m.size = size * sizeof(uchar);

		__memoryvec.push_back( m );

		return data;
	}
#endif	// _DEBUG

	// get the portion of memory we need
	data = _globalMemory+pos;

	// zero memory (not needed)
	//memset(data, 0, size);

	// return buffer at pos 'pos'
	return data;
#if defined( _DEBUG )
	(void)filename;
	(void)line;
#endif
}

// http://www.alexonlinux.com/aligned-vs-unaligned-memory-access
// but :
// http://lemire.me/blog/archives/2012/05/31/data-alignment-for-
//	speed-myth-or-reality/
#if defined( _DEBUG )
uchar* MemAlloc::_getMemory_aligned(
	const uint& size, const uint& alignment, const char* filename, int line )
#else
uchar* _getMemory_aligned( const uint& size, const uint& alignment )
#endif	// _DEBUG
{
	const uint total_size = size + (2 * alignment) + sizeof(uint);
	uchar *data =(uchar*) Core_getMemory(sizeof(char) * total_size);
    return data;
#if defined( _DEBUG )
    (void)filename;
    (void)line;
#endif
}

#if defined( _DEBUG )
void MemAlloc::_freeMemory_aligned(
	const uint& size, const uint& alignment, const char* filename, int line )
#else
void MemAlloc::_freeMemory_aligned(const uint& size, const uint& alignment )
#endif	// _DEBUG
{
	const uint total_size = size + (2 * alignment) + sizeof(uint);
	Core_freeMemory(sizeof(char) * total_size);
#if defined( _DEBUG )
	(void)filename;
	(void)line;
#endif
}

} // namespace PCRTT
