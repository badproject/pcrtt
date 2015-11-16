/**
 * Posix compliant version of CSA-Rainbow-Table-Tool
 * 
 * Copyright (C) 2015 NEngine Team
 * 
 * This file is part of CSA-Rainbow-Table-Tool.
 * 
 * CSA-Rainbow-Table-Tool is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * CSA-Rainbow-Table-Tool is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with CSA-Rainbow-Table-Tool.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * This software is based on the windows version of CSA-Rainbow-Table-Tool
 * 
 * Copyright (C) 2012  Colibri <colibri.dvb@googlemail.com>  
 * http://colibri.net63.net/ alias http://colibri-dvb.info/
 * 
 */
#if !defined(PCRTT_SRC_CORE_SORT_HEADER_H_)
#define PCRTT_SRC_CORE_SORT_HEADER_H_

#include <vector>

namespace PCRTT {
	
struct Chunk_t
{
	Chunk_t() : data(0), size(0), scmp(0), offset(0) {}

	Chunk_t(const Chunk_t& rhs) : data(rhs.data), size(rhs.size),
		scmp(rhs.scmp), offset(rhs.offset) {}

	Chunk_t& operator=(const Chunk_t& rhs)
	{
		data = rhs.data;
		size = rhs.size;
		scmp = rhs.scmp;
		offset = rhs.offset;
	}

	bool operator < (const Chunk_t& rhs) const
	{
		assert ( size == rhs.size );
		return (memcmp(data+offset,rhs.data+rhs.offset,scmp) < 0);
	}

	bool operator == (const Chunk_t& rhs) const
	{
		assert ( size == rhs.size );
		return (memcmp(data+offset,rhs.data+rhs.offset,scmp) == 0);
	}

	bool operator()(const Chunk_t& lhs, const Chunk_t& rhs)
	{
		assert ( lhs.size == rhs.size );
		return (lhs < rhs);
	}

	uchar* data;	// data
	uint size;		// size of data
	uint scmp;		// size of compare
	uint offset;	// offset
};

typedef std::vector<Chunk_t> Chunk_v;

typedef bool (*compare_fnc)(const Chunk_t& i,const Chunk_t& j);

double sort(Chunk_v &V, uint parts, uint mem, compare_fnc compare=0);

int unit_test_sort_file();

} // end of namespace PCRTT

#endif // PCRTT_SRC_CORE_SORT_HEADER_H_
