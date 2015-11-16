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
#include <pcrtt.h>
#include "shared.h"
#include "crypt8finder.h"
#include <algorithm> // std::sort
 
namespace PCRTT {
namespace Shared {

Crypt8Finder::Crypt8Finder() : infos(), m_pid_list( new fcc_v[0x2000] )
{ }

Crypt8Finder::~Crypt8Finder()
{ }

void Crypt8Finder::CombineElements()
{
	int k, idx, fCreateNewElement, LowerIsSame, UpperIsSame, Count=1;

	for( idx = 0; idx < infos.size(); idx += Count )
	{
		FindC8Info_t& info = infos[idx];

		fcc_v& list = m_pid_list[info.Pid];
		Count = CountEqualCrypt8InfoElements( idx );

		if(Count > 1)
		{
			AddToPidListAndKeepTheBest(
				list, info.Pid, info.fOddCw, info.Crypt8, Count );
		}
	}
}

String Crypt8Finder::results()
{
	String result(""), str;
	ushort pid;
	int k, idx, fAddNewLine;

	for(pid=0; pid < 0x2000; ++pid)
	{
		fAddNewLine = 0;
		fcc_v& list = m_pid_list[pid];

		sortPidByCount( list );
		
		for ( k = 0; k < list.size(); ++k )
		{
			if ( k < MAX_CRYPT8_PER_PID )
			{
				FindC8InfoCombined_t& info = list[k];
				str = info.toString();

				result << format(
					"PID:%4Xh Crypt8:%s [%s] Count:%d\n", pid, str.c_str(),
					info.fOddCw ? "O":"E", info.Count
				);
				fAddNewLine = 1;
			}
			else
			{
				if(k == MAX_CRYPT8_PER_PID)
				{
					result << "...\n";
				}
			}
		}		

		if(fAddNewLine)
			result << "\n";
	}

	return result;
}

void FindC8Info_t::dump()
{
	printf("pid: %d\n", Pid);
	printf("fOddCw: %d\n", fOddCw);
	printBuffer(0,0,Crypt8,8);
}

int Crypt8Finder::search()
{
	std::qsort( infos, infos.size(), sizeof(FindC8Info_t),
		FindC8Info_t::compare );

	CombineElements();

	return 1;//OK
}

int Crypt8Finder::CountEqualCrypt8InfoElements(int IdxStart)
{
	int i, fStop = 0, Count = 1;

	if ( IdxStart >= infos.size() )
		assert( !"IdxStart >= infos.size()" );

	// last element
	if( (IdxStart+1) == infos.size() )
		return 1;

	for ( i = IdxStart+1; ( i < infos.size() ) && !fStop; ++i)
	{
		if ( infos[IdxStart] == infos[IdxStart + Count] )
			++Count;
		else
			fStop = 1;
	}

	return Count;
}

void Crypt8Finder::AddToPidListAndKeepTheBest(fcc_v& list,
	ushort Pid, uchar fOddCw, const uchar *Crypt8, int Count)
{
	assert(Crypt8);

	/**
	 * add combined element to list
	 * delete the worst element if the list exceeds
	 * the max (MAX_CRYPT8_PER_PID + 1)
	 * The PidList must be sorted by Count before calling
	 */
	int MinCountFromList;
	FindC8InfoCombined_t info(Count,fOddCw,Crypt8);

	MinCountFromList = GetMinCountFromSortedPidList(list);

	// new element is not better than the elements in the list
	if(Count <= MinCountFromList)
	{
		return; // nothing to do
	}
	
	list.push_back( info );

	sortPidByCount( list );

	// keep only the best
	while( list.size() > (MAX_CRYPT8_PER_PID+1) )
	{ list.pop_back(); }
}

int Crypt8Finder::GetMinCountFromSortedPidList(const fcc_v& list)
{
	if( list.size() )
		return list[list.size()-1].Count;
	return 0;
}

static int sort_cnt(const void* as,const void* bs)
{
	FindC8InfoCombined_t* a = (FindC8InfoCombined_t*)as;
	FindC8InfoCombined_t* b = (FindC8InfoCombined_t*)bs;
	return (a->Count > b->Count);
}

static inline void copy_list(const fcc_v& src, fcc_v& dst)
{
	dst.clear();
	for(int i=0; i < src.size(); ++i)
	{
		dst.push_back(src[i]);
	}
}

void Crypt8Finder::sortPidByCount(fcc_v& list)
{
	//std::qsort( list, list.size(), sizeof(FindC8InfoCombined_t), sort_cnt );
	int i, maxCnt = 0, maxPos = 0, oldPos;
	fcc_v lst;

	while( list.size() )
	{
		maxCnt = 0;
		for(i=0; i < list.size(); ++i)
		{
			oldPos = i;
			const int cnt = list[i].Count;
			if(cnt > maxCnt)
			{
				maxCnt = cnt;
				maxPos = oldPos;
			}
		}
		lst.push_back( list[maxPos] );
		list.remove(maxPos);
	}
	copy_list(lst,list);
}

FindC8Info_t* Crypt8Finder::GetCrypt8InfoElementPtrFromIdx(int Idx)
{
	if( Idx <= infos.size() )
		return &infos[Idx];
	return NULL; // error
}

int Crypt8Finder::AreCrypt8InfoElementsEqual(int Idx1, int Idx2)
{
	FindC8Info_t *info1 = GetCrypt8InfoElementPtrFromIdx(Idx1);
	FindC8Info_t *info2 = GetCrypt8InfoElementPtrFromIdx(Idx2);
#if 1
	return ( *info1 == *info2 );
#else
	if ( memcmp(info1, info2, sizeof(FindC8Info_t)) == 0 )
	{
		return 1; // equal
	}
	return 0; // different
#endif // 
}

int Crypt8Finder::add( ushort p, uchar f, const uchar *c )
{
	FindC8Info_t i(p,f,c);
	infos.push_back( i );
	return 1; // ok
}

} // end of namespace Shared
} // end of namespace PCRTT
