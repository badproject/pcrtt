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
#if !defined(PCRTT_SRC_CORE_RAINBOW_TAB_HEADER_H_)
#define PCRTT_SRC_CORE_RAINBOW_TAB_HEADER_H_

namespace PCRTT {

// 12 MByte buffer per file
#define ALLOC_SIZE_PER_FILE	( 0x100000 * (2*6) )

#if 0

class RainbowTab
{
public:
	#if defined(__USE_CPP_11__)
	RainbowTab(const RainbowTab&) = delete;
	void operator=(const RainbowTab&) = delete;
	#endif // __USE_CPP_11__

	static void FillCw6ArrayWithIncCw6Start(uchar *Cw6, ulong Elements, const uchar *Cw6Start);
	static void GetInfoFromChainFile(uchar *NextCw6Start, ulong *pElements, File *pFile, String *pErrorText);
	static void ValidateChainFile(CFile *pFile, const uchar *Cw6StartRef, String *pErrorText);
	static void SaveCwStartAndEndArray(const uchar *Cw6Start, const uchar *Cw6End, ulong Elements, CFile *pDstFile);
	static int MergeSortedFiles(CFile *pDstFile, CFile *pSrcFile1, CFile *pSrcFile2);
	static int SortRBTableFile(String UnsortedSrcFileName, String SortedDstFileName);
	static void CheckForIncStartValues(CFile *pFile, const uchar *FirstCw6StartValueRef, String *pErrorText);
	static void GetRndChainFromFile(CFile *pFile, uchar *StartCw6, uchar *EndCw6, String *pErrorText);
	static int FindCw6EndAndGetCs6StartFromSortedFile(void_vp *pCw6StartEndIdxList, const uchar *Cw6EndArray, int Cw6EndArrayElements, CFile *pSrcFile);
	//static void FreeListElemets(	 *pList);
	static void LoadCwEndValuesFromFile(uchar **pBuffer, int *pLen, String SrcFileName);
	static int IsStartPrefixInList(const uchar *StartPrefix, void_vp *pCw6StartEndIdxList);
};	// end of class RainbowTab

#endif // 0

} // end of namespace PCRTT

#endif // PCRTT_SRC_CORE_RAINBOW_TAB_HEADER_H_
