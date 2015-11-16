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
#if !defined(PCRTT_SRC_SHARED_CRYPT8FINDER_HEADER_H_)
#define PCRTT_SRC_SHARED_CRYPT8FINDER_HEADER_H_

namespace PCRTT {
namespace Shared {
	
//limit for showing the results
#define MAX_CRYPT8_PER_PID	8

// _FIND_CRYPT8_INFO
struct FindC8Info_t
{
	FindC8Info_t() : Pid(0), fOddCw(0) { EMPTY_STRUCT }

	/** copy constructor */
	FindC8Info_t(const FindC8Info_t& o) : Pid(o.Pid), fOddCw(o.fOddCw)
	{ copy(o.Crypt8); }
	FindC8Info_t( ushort Pid, uchar fOddCw, const uchar *Crypt8 ) :
		Pid(Pid), fOddCw(fOddCw)
	{ copy(Crypt8); }

	/** simple copy */
	inline void copy(const uchar* c)
	{
		Crypt8[0] = c[0], Crypt8[1] = c[1], Crypt8[2] = c[2], 
		Crypt8[3] = c[3], Crypt8[4] = c[4], Crypt8[5] = c[5], 
		Crypt8[6] = c[6], Crypt8[7] = c[7];
	}

	/** equal operator */
	inline int operator==(const FindC8Info_t &v) const
	{
		return (
			v.Crypt8[0] == Crypt8[0] && v.Crypt8[1] == Crypt8[1] &&
			v.Crypt8[2] == Crypt8[2] && v.Crypt8[3] == Crypt8[3] &&
			v.Crypt8[4] == Crypt8[4] && v.Crypt8[5] == Crypt8[5] && 
			v.Crypt8[6] == Crypt8[6] && v.Crypt8[7] == Crypt8[7] &&
			v.Pid == Pid && v.fOddCw == fOddCw
		);
	}

	static int compare(const void * a, const void * b)
	{
		int Cmp;
		FindC8Info_t* aa = (FindC8Info_t*)a;
		FindC8Info_t* bb = (FindC8Info_t*)b;

		//return (*aa == *bb);
		Cmp = aa->Pid - bb->Pid;
		if(Cmp != 0)
		{
			return Cmp;
		}

		Cmp = memcmp(aa->Crypt8, bb->Crypt8, 8);
		if(Cmp != 0)
		{
			return Cmp;
		}

		Cmp = aa->fOddCw - bb->fOddCw;
		if(Cmp != 0)
		{
			return Cmp;
		}

		return 0;//equal
	}

	void dump();

	/** not equal operator */
	inline int operator!=(const FindC8Info_t &v) const {return !(*this == v);}

	uchar Crypt8[8];
	ushort Pid;
	uchar fOddCw;//true->odd false->even
}; // end of struct FindC8Info_t

typedef Vector<FindC8Info_t> FindC8Info_v;

// _FIND_CRYPT8_INFO_COMBINED
struct FindC8InfoCombined_t
{
	FindC8InfoCombined_t() :
		Count(0), fOddCw(0) { EMPTY_STRUCT }

	/** copy constructor */
	FindC8InfoCombined_t(const FindC8InfoCombined_t& o) :
		fOddCw(o.fOddCw), Count(o.Count) { copy(o.Crypt8); }

	FindC8InfoCombined_t( int Count, uchar fOddCw, const uchar *Crypt8 ) :
		Count(Count), fOddCw(fOddCw)
	{ copy(Crypt8); }
	
	inline void reset()
	{
		fOddCw = 0;
		Count = 0;
		Crypt8[0] = 0, Crypt8[1] = 0, Crypt8[2] = 0, 
		Crypt8[3] = 0, Crypt8[4] = 0, Crypt8[5] = 0, 
		Crypt8[6] = 0, Crypt8[7] = 0;
	}

	/** simple copy */
	inline void copy(const uchar* c)
	{
		Crypt8[0] = c[0], Crypt8[1] = c[1], Crypt8[2] = c[2], 
		Crypt8[3] = c[3], Crypt8[4] = c[4], Crypt8[5] = c[5], 
		Crypt8[6] = c[6], Crypt8[7] = c[7];
	}

	/** equal operator */
	inline int operator==(const FindC8InfoCombined_t &v) const
	{
#if 1 // stop on first diff
		if ( v.Crypt8[0] != Crypt8[0] ) return 0;
		if ( v.Crypt8[1] != Crypt8[1] ) return 0;
		if ( v.Crypt8[2] != Crypt8[2] ) return 0;
		if ( v.Crypt8[3] != Crypt8[3] ) return 0;
		if ( v.Crypt8[4] != Crypt8[4] ) return 0;
		if ( v.Crypt8[5] != Crypt8[5] ) return 0;
		if ( v.Crypt8[6] != Crypt8[6] ) return 0;
		if ( v.Crypt8[7] != Crypt8[7] ) return 0;
		
		if ( v.fOddCw != fOddCw ) return 0;
		if ( v.Count != Count ) return 0;

		return 1;
#else
		return (
			v.Crypt8[0] == Crypt8[0] && v.Crypt8[1] == Crypt8[1] &&
			v.Crypt8[2] == Crypt8[2] && v.Crypt8[3] == Crypt8[3] &&
			v.Crypt8[4] == Crypt8[4] && v.Crypt8[5] == Crypt8[5] && 
			v.Crypt8[6] == Crypt8[6] && v.Crypt8[7] == Crypt8[7] &&
			v.fOddCw == fOddCw && v.Count == Count
		);
#endif // conditionnal
	}

	String toString()
	{ return String::bin2String(Crypt8,8); }

	/** not equal operator */
	inline int operator!=(const FindC8InfoCombined_t &v) const
	{return !(*this == v);}

	uchar Crypt8[8];
	uchar fOddCw;//true->odd false->even
	int Count;
}; // end of struct FindC8InfoCombined_t

typedef Vector<FindC8InfoCombined_t> FindC8InfoCombined_v;
#define fcc_v FindC8InfoCombined_v

/**
 * class Crypt8Finder
 */
class Crypt8Finder
{
public:
	#if defined(__USE_CPP_11__)
	Crypt8Finder(const Crypt8Finder&) = delete;
	void operator=(const Crypt8Finder&) = delete;
	#endif // __USE_CPP_11__
	
	Crypt8Finder(void);
	~Crypt8Finder(void);

	int add( ushort pid, uchar fOddCw, const uchar* crypt8 );

	int search();

	String results();

private: // function declaration

	FindC8Info_t* GetCrypt8InfoElementPtrFromIdx(int Idx);

	int AreCrypt8InfoElementsEqual(int Idx1, int Idx2);
	void CombineElements();
	void sortPidByCount(fcc_v& pCurPidList);
	int GetMinCountFromSortedPidList(const fcc_v& lst);
	void AddToPidListAndKeepTheBest(fcc_v& list, ushort pid,
		uchar fOddCw, const uchar *Crypt8, int Count);
	int CountEqualCrypt8InfoElements(int IdxStart);

private: // variables
	//uchar *m_Buffer;
	FindC8Info_v infos;
	//int m_BufferSize;
	//int m_Elements;

	fcc_v* m_pid_list;

}; // end of class Crypt8Finder

} // end of namespace Shared
} // end of namespace PCRTT

#endif // PCRTT_SRC_SHARED_CRYPT8FINDER_HEADER_H_
