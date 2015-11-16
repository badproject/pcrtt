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
#if !defined(PCRTT_SRC_SHARED_RAINBOW_TABLE_HEADER_H_)
#define PCRTT_SRC_SHARED_RAINBOW_TABLE_HEADER_H_

#include "crypt8finder.h"

namespace PCRTT {
namespace Shared {

/**
 * static class to manage
 * all action on rainbow table
 */
class Rainbow
{
public:
	/** global action */
	static void start(Worker_t* worker);

	/** */
	static void FillCw6ArrayWithIncCw6Start(uchar *Cw6, ulong Elements,
					const uchar *Cw6Start);

	/** */
	static String GetInfoFromChainFile(uchar *NextCw6Start, ulong *pElements,
					File *pFile);

	/** */
	static String ValidateChainFile(File *file, const uchar *Cw6StartRef);

	/** */
	static void SaveCwStartAndEndArray(const uchar *Cw6Start,
					const uchar *Cw6End, ulong Elements, File *pDstFile);

	/** */
	static int MergeSortedFiles(Worker_t* worker,File *dst,File *src1,
				File *src2);

	/** */
	static int SortRBTableFile(String UnsortedSrcFileName,
				String SortedDstFileName);

	/** */
	static void CheckForIncStartValues(File *f, 
			const uchar *FirstCw6StartValueRef, String *pErrorText);

	/** */
	static String GetRndChainFromFile(File *pFile, uchar *StartCw6,
					uchar *EndCw6);

	/** */
	static int FindCw6EndAndGetCs6StartFromSortedFile(
			uchar_vp& pCw6StartEndIdxList, const uchar *Cw6EndArray, 
			int Cw6EndArrayElements, File *pSrcFile);

	static void FreeListElemets(uchar_vp& list);

	/** */
	static void LoadCwEndValuesFromFile(uchar **pBuffer, int *pLen,
					String SrcFileName);

	/** */
	static int IsStartPrefixInList(const uchar *StartPrefix,
					uchar_vp& pCw6StartEndIdxList);

}; // end of class Rainbow

} // end of namespace Shared
} // end of namespace PCRTT

#endif // PCRTT_SRC_SHARED_RAINBOW_TABLE_HEADER_H_
