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

namespace PCRTT {

static inline void cw6ToValue(const uchar *Cw6, uint64_t *pullValue)
{
	*pullValue = 0;
	((uchar*)pullValue)[0] = Cw6[5];
	((uchar*)pullValue)[1] = Cw6[4];
	((uchar*)pullValue)[2] = Cw6[3];
	((uchar*)pullValue)[3] = Cw6[2];
	((uchar*)pullValue)[4] = Cw6[1];
	((uchar*)pullValue)[5] = Cw6[0];
}

static inline void valueToCw6(uchar *Cw6, const uint64_t ullValue)
{
	Cw6[0] = (ullValue >> 40) & 0xFF;
	Cw6[1] = (ullValue >> 32) & 0xFF;
	Cw6[2] = (ullValue >> 24) & 0xFF;
	Cw6[3] = (ullValue >> 16) & 0xFF;
	Cw6[4] = (ullValue >> 8) & 0xFF;
	Cw6[5] = ullValue & 0xFF;
}

static inline void incCw6(uchar *Cw6)
{
	uint64_t ullValue;
	cw6ToValue( Cw6, &ullValue );	++ullValue;
	valueToCw6( Cw6, ullValue );
}

static inline void getRnd(uint64_t *pRnd)
{
	int i;

	for(i=0; i<sizeof(uint64_t); ++i)
	{
		((uchar*)pRnd)[i] = rand() & 0xFF;
	}
}


static void addElementToList(void_vp& pCw6StartEndIdxList,
	uchar *TableCw6Start, uchar *TableCw6End, ulong Cw6EndArrayIdx)
{
	// [in/out] void_vp *pCw6StartEndIdxList (result will be added to the list)
	// [in] uchar TableCw6Start[6]
	// [in] uchar TableCw6End[6]
	// [in] ulong Cw6EndArrayIdx (from search - not from table)

	uchar *Cw6StartEndIdx = new uchar[6+6+4];

	memcpy( Cw6StartEndIdx, TableCw6Start, 6 );
	memcpy( &Cw6StartEndIdx[6], TableCw6End, 6 );

	Cw6StartEndIdx[12] = Cw6EndArrayIdx; // save Cw6EndArrayIdx
	Cw6StartEndIdx[13] = Cw6EndArrayIdx >> 8;
	Cw6StartEndIdx[14] = Cw6EndArrayIdx >> 16;
	Cw6StartEndIdx[15] = Cw6EndArrayIdx >> 24;

	pCw6StartEndIdxList.push_back( Cw6StartEndIdx );
}

int CRainbowTab::IsStartPrefixInList(const uchar *StartPrefix,
	void_vp& pCw6StartEndIdxList)
{
	// cmp 22bit start prefix
	int size, i;
	uchar *Cw6StartEndIdx;

	size = pCw6StartEndIdxList.size();

	if( !size )
		return 0; // StartPrefix not in list

	for(i=0; i < size; ++i)
	{
		Cw6StartEndIdx = (uchar*)pCw6StartEndIdxList[i];

		if((Cw6StartEndIdx[0] == StartPrefix[0])
			&& (Cw6StartEndIdx[1] == StartPrefix[1])
			&& ((Cw6StartEndIdx[2] & 0xFC) == (StartPrefix[2] & 0xFC)))
		{
			return 1; // StartPrefix in list
		}
	}

	return 0;
}

void CRainbowTab::FillCw6ArrayWithIncCw6Start(uchar *Cw6, ulong Elements,
	const uchar *Cw6Start)
{
	int i;
	uint64_t StartValue;

	cw6ToValue( Cw6Start, &StartValue );

	for(i=0; i < Elements; ++i)
	{
		Cw6[(i*6) + 0] = (StartValue >> 40) & 0xFF;
		Cw6[(i*6) + 1] = (StartValue >> 32) & 0xFF;
		Cw6[(i*6) + 2] = (StartValue >> 24) & 0xFF;
		Cw6[(i*6) + 3] = (StartValue >> 16) & 0xFF;
		Cw6[(i*6) + 4] = (StartValue >> 8) & 0xFF;
		Cw6[(i*6) + 5] = StartValue & 0xFF;

		++StartValue;
	}
}

int CRainbowTab::ValidateChainFile(File* file,const uchar *Cw6StartRef,
	String *pErrorText)
{
	uint64_t FileLen;
	ulong Elements;
	uchar FirstCw6Start[6], LastCw6Start[6];
	String str1, str2;

	//default
	Elements = 0;
	*pErrorText.clear();

	FileLen = file->GetSize();

	if( (FileLen % (6+6)) != 0 )
	{
		*pErrorText = format("ERROR: The file \"%s\" has a length (%llu) that"
			" isn't a multiple of (6+6)\n", fp->GetName(), FileLen);
		return 0;
	}

	Elements = FileLen / (6+6);

	if(Elements > 0)
	{
		fp->SeekToBegin();
		fp->Read(FirstCw6Start, 6);
		if(memcmp(FirstCw6Start, Cw6StartRef, 6) != 0)
		{
			str1 = String::bin2String( FirstCw6Start, 6 );
			str2 = String::bin2String( Cw6StartRef, 6 );

			pErrorText = format("ERROR: The file \"%s\" has a wrong StartCw6 (%s) at pos (%Xh) (should be %s)\n", 
				fp->GetName(), str1, 0, str2);

			return 0;
		}

		fp->Seek(-(6+6), CFile::end);
		uint64_t LastPos;
		LastPos = fp->GetPosition();
		fp->Read(LastCw6Start, 6);

		uchar Cw6StartRef2[6];
		uint64_t ullValue;

		cw6ToValue(Cw6StartRef, &ullValue);
		ullValue += (LastPos/(6+6));

		valueToCw6(Cw6StartRef2, ullValue);

		if( memcmp( LastCw6Start, Cw6StartRef2, 6) != 0 )
		{
			str1 = String::bin2String( LastCw6Start, 6 );
			str2 = String::bin2String( Cw6StartRef2, 6 );

			pErrorText = format("ERROR: The file \"%s\" has a wrong StartCw6 (%s) at pos (%llXh) (should be %s)\n", 
				fp->GetName(), str1, LastPos, str2);
			return 0;
		}
	}

	return 1;
}

void CRainbowTab::GetInfoFromChainFile(uchar *NextCw6Start, ulong *pElements,
	File* file, String *pErrorText)
{
	//NextCw6Start can be NULL it the info is not required
	uint64_t FileLen;

	// default
	if( NextCw6Start )
		memset( NextCw6Start, 0x00, 6 );

	*pElements = 0;
	if ( pErrorText ) *pErrorText.clear();

	FileLen = file->GetSize();

	if ( pErrorText && ((FileLen % (6+6)) != 0) )
	{
		*pErrorText = format("ERROR: The file \"%s\" has a length (%llu) that "
						"isn't a multiple of (6+6)\n",file->GetName(),FileLen);
	}

	*pElements = FileLen / (6+6);

	if(NextCw6Start)
	{
		if(*pElements > 0)
		{
			file->Seek(-(6+6), ios_base::end);
			file->Read( NextCw6Start, 6 );

			incCw6( NextCw6Start );
		}
	}
}

void CRainbowTab::SaveCwStartAndEndArray(const uchar *Cw6Start, const uchar *Cw6End, ulong Elements, CFile *pDstFile)
{
	int i;
	String Text;

	//Save
	uchar *DataToSave;
	DataToSave = new uchar[Elements*(6+6)];
	assert(DataToSave);
	ZeroMemory(DataToSave, Elements*(6+6));

	for(i=0; i<Elements; i++)
	{
		memcpy(&DataToSave[i*(6+6)], &Cw6Start[i*6], 6);
		memcpy(&DataToSave[(i*(6+6))+6], &Cw6End[i*6], 6);
	}

	pDstFile->SeekToEnd();
	pDstFile->Write(DataToSave, Elements*(6+6));

	delete DataToSave;
}

void CRainbowTab::GetRndChainFromFile(File* file, uchar *StartCw6, uchar *EndCw6, String *pErrorText)
{
	uint64_t FileLen, Elements, Rnd, ElementOffset;
	FileLen = file->GetSize();

	if((FileLen % (6+6)) != 0)
	{
		pErrorText = format("ERROR: The file \"%s\" has a length (%llu) "
			"that isn't a multiple of (6+6)\n", 
			file->GetName(), FileLen);
		return;
	}

	Elements = FileLen / (6+6);
	getRnd( &Rnd );
	ElementOffset = (Rnd % Elements) * (6+6);

	fp->Seek( ElementOffset, ios_base::begin );

	fp->Read( StartCw6, 6 );
	fp->Read( EndCw6, 6 );
}

void CRainbowTab::CheckForIncStartValues(FILE* fp, const uchar *FirstCw6StartValueRef, String *pErrorText)
{
	uchar Cw6StartValue[6];
	uchar *Buffer;
	uint64_t FilePos;
	String str1;
	String str2;
	int i;

	fp->SeekToBegin();

	uint64_t FileLen;
	FileLen = fp->GetSize();

	if(FileLen != (MAX_ELEMENTS_PER_CHAIN_FILE * (6+6)))
	{
		pErrorText = format("ERROR: File \"%s\" has length %llu instead of %llu\n", 
			fp->GetName(), FileLen, MAX_ELEMENTS_PER_CHAIN_FILE * (6+6));
		return;
	}

	assert(((MAX_ELEMENTS_PER_CHAIN_FILE * (6+6)) % ALLOC_SIZE_PER_FILE) == 0);

	Buffer = new uchar[ALLOC_SIZE_PER_FILE]; assert(Buffer);

	memcpy(Cw6StartValue, FirstCw6StartValueRef, 6);

	for(FilePos=0; FilePos<FileLen; FilePos+=ALLOC_SIZE_PER_FILE)
	{
		if(fp->Read(Buffer, ALLOC_SIZE_PER_FILE) != ALLOC_SIZE_PER_FILE)
		{
			pErrorText = format("ERROR: Can't read from file \"%s\"\n", 
				fp->GetName());
			KILLARRAY( Buffer );
			return;
		}

		for(i=0; i<ALLOC_SIZE_PER_FILE; i+=(6+6))
		{
			if(memcmp(&Buffer[i], Cw6StartValue, 6) != 0)
			{
				str1 = String::bin2String( &Buffer[i], 6 );
				str2 = String::bin2String( Cw6StartValue, 6 );

				pErrorText = format("ERROR: File \"%s\" has a wrong start "
					"value (%s instead of %s) at pos %llu\n",
					file->GetName(), str1, str2, FilePos+i);

				KILLARRAY( Buffer );
				return;
			}

			incCw6(Cw6StartValue);
		}
	}

	KILLARRAY( Buffer );
}

int CRainbowTab::MergeSortedFiles(CFile *pDstFile, CFile *pSrcFile1, CFile *pSrcFile2)
{
	//return false=error true=ok
	uint64_t SrcLen1;
	uint64_t SrcLen2;
	uchar *Buffer1;
	uchar *Buffer2;
	uchar *BufferDst;
	int UsedSizeBuffer1 = 0;
	int UsedSizeBuffer2 = 0;
	int UsedSizeDstBuffer = 0;
	LONGLONG RemainingFromSrcFile1;
	LONGLONG RemainingFromSrcFile2;
	LONGLONG DstPos;
	int Buffer1Pos;
	int Buffer2Pos;

	pSrcFile1->SeekToBegin();
	pSrcFile2->SeekToBegin();

	SrcLen1 = pSrcFile1->GetSize();
	SrcLen2 = pSrcFile2->GetSize();
	RemainingFromSrcFile1 = SrcLen1;
	RemainingFromSrcFile2 = SrcLen2;

	Buffer1 = new uchar[ALLOC_SIZE_PER_FILE];
	Buffer2 = new uchar[ALLOC_SIZE_PER_FILE];
	BufferDst = new uchar[ALLOC_SIZE_PER_FILE];

	Buffer1Pos = 0;
	Buffer2Pos = 0;

	for(DstPos=0; DstPos<(SrcLen1+SrcLen2); DstPos+=(2*6))
	{
		if((pTI->fExitThread) || (pTI->fAddToRbtStop))
		{
			//merge canceled
			delete Buffer1;
			delete Buffer2;
			delete BufferDst;
			return false;//error
		}

		if((DstPos & 0xFFFFFF) == 0)
		{
			pTI->ProcAddToRbtFile = (float)DstPos * 100.0 / (float)(SrcLen1+SrcLen2);
			SendToGui(ID_MSG1, UPDATE_STATISTIC, (LPARAM)0);
		}

		//refill Buffer
		if(UsedSizeBuffer1 == 0)
		{
			if(RemainingFromSrcFile1 > 0)
			{
				int Len;
				Len = min(RemainingFromSrcFile1, ALLOC_SIZE_PER_FILE);
				UINT Read;
				if((Read = pSrcFile1->Read(Buffer1, Len)) != Len)
				{
					assert(false);
					return false;//error
				}
				RemainingFromSrcFile1 -= Len;
				UsedSizeBuffer1 += Len;
				Buffer1Pos = 0;
			}
		}
		if(UsedSizeBuffer2 == 0)
		{
			if(RemainingFromSrcFile2 > 0)
			{
				int Len;
				Len = min(RemainingFromSrcFile2, ALLOC_SIZE_PER_FILE);
				if(pSrcFile2->Read(Buffer2, Len) != Len)
				{
					assert(false);
					return false;//error
				}
				RemainingFromSrcFile2 -= Len;
				UsedSizeBuffer2 += Len;
				Buffer2Pos = 0;
			}
		}

		//write DstBuffer to file
		if(UsedSizeDstBuffer == ALLOC_SIZE_PER_FILE)
		{
			pDstFile->Write(BufferDst, ALLOC_SIZE_PER_FILE);
			UsedSizeDstBuffer = 0;
		}

		//cmp
		if(UsedSizeBuffer1 == 0)
		{
			//no more Buffer1 data -> copy Buffer2
			memcpy(&BufferDst[UsedSizeDstBuffer], &Buffer2[Buffer2Pos], 2*6);
			UsedSizeDstBuffer += (2*6);
			Buffer2Pos += (2*6);
			UsedSizeBuffer2 -= (2*6);
		}
		else
		{
			if(UsedSizeBuffer2 == 0)
			{
				//no more Buffer2 data -> copy Buffer1
				memcpy(&BufferDst[UsedSizeDstBuffer], &Buffer1[Buffer1Pos], 2*6);
				UsedSizeDstBuffer += (2*6);
				Buffer1Pos += (2*6);
				UsedSizeBuffer1 -= (2*6);
			}
			else
			{
				//cmp buffer
				if(memcmp(&Buffer1[Buffer1Pos+6], &Buffer2[Buffer2Pos+6], 6) < 0)
				{
					//Buffer1 is lower -> copy Buffer1
					memcpy(&BufferDst[UsedSizeDstBuffer], &Buffer1[Buffer1Pos], 2*6);
					UsedSizeDstBuffer += (2*6);
					Buffer1Pos += (2*6);
					UsedSizeBuffer1 -= (2*6);
				}
				else
				{
					memcpy(&BufferDst[UsedSizeDstBuffer], &Buffer2[Buffer2Pos], 2*6);
					UsedSizeDstBuffer += (2*6);
					Buffer2Pos += (2*6);
					UsedSizeBuffer2 -= (2*6);
				}
			}
		}
	}

	//write remaining DstBuffer to file
	if(UsedSizeDstBuffer > 0)
	{
		pDstFile->Write(BufferDst, UsedSizeDstBuffer);
		UsedSizeDstBuffer = 0;
	}

	if((UsedSizeBuffer1 > 0) || (UsedSizeBuffer2 > 0))
	{
		assert(false);
		return false;//error
	}

	delete Buffer1;
	delete Buffer2;
	delete BufferDst;

	return true;//ok
}

int compare(const void * a, const void * b)
{
	//cmp EndCw
	int i;
	const uchar *aa = (uchar*)a;
	const uchar *bb = (uchar*)b;

	for(i=0; i<6; i++)
	{
		if(aa[6+i] != bb[6+i])
		{
			return aa[6+i] - bb[6+i];
		}
	}
	return 0;//equal
}

int CRainbowTab::SortRBTableFile(String UnsortedSrcFileName, String SortedDstFileName)
{
	// read unsorted file -> write to sorted file
	CFile SrcFile, DstFile;
	uchar *Data;
	String Text;
	uint64_t SrcLen, Time, Delta;

	if ( !SrcFile.Open( UnsortedSrcFileName, CFile::modeRead) )
		return 0;

	SrcLen = SrcFile.GetSize();

	if((SrcLen % (2*6)) != 0)
		return 0;

	if(!DstFile.Open(SortedDstFileName, CFile::modeCreate | CFile::modeWrite))
		return 0;

	Data = new uchar[SrcLen];	assert( Data );

	Time = GetTickCount();
	SrcFile.Read(Data, SrcLen);
	Delta = GetTickCount() - Time;
	Text = format("Time for reading = %llu sec.\n", Delta/1000);
//	Log(Text);
	
	Time = GetTickCount();

	qsort((void*)Data, SrcLen/(2*6), 2*6, compare);

	Delta = GetTickCount() - Time;
	Text = format("Time for sorting = %llu sec.\n", Delta/1000);
	Log(Text);

	Time = GetTickCount();
	DstFile.Write(Data, SrcLen);
	Delta = GetTickCount() - Time;
	Text = format("Time for writing = %llu sec.\n", Delta/1000);
//	Log(Text);
	
	//free buffer
	delete Data;

	return true;//ok
}

int CRainbowTab::FindCw6EndAndGetCs6StartFromSortedFile(
	void_vp *pCw6StartEndIdxList, const uchar *Cw6EndArray,
	int Cw6EndArrayElements, CFile *pSrcFile)
{
	// [out] pCw6StartEndIdxList: 6Byte Cw6Start, 6Byte Cw6End, 4Byte Idx LSB
	// [in] const uchar *Cw6EndArray
	// [in] int Cw6EndArrayElements
	// [in] CFile SrcFile (Rainbow table)
	// find all matches and not only one
	// return true=ok false=error
	// binary search in sorted file
	LONGLONG i;
	uchar TableCw6Start[6];
	uchar TableCw6End[6];
	String str1;
	String str2;
	LONGLONG ElementIdx;
	LONGLONG Offset;
	ulong m;


	//List must be empty
	if(!pCw6StartEndIdxList->IsEmpty())
	{
		return false;//error
	}

	pSrcFile->SeekToBegin();

	uint64_t Len = pSrcFile->GetSize();
	if((Len % (2*6)) != 0)
	{
		return false;//error
	}
	if(Len == 0)
	{
		return true;//nothing to do
	}

	for(m=0; m<Cw6EndArrayElements; m++)
	{
		const uchar *Cw6End = &Cw6EndArray[m * 6];

		LONGLONG LowerIdx = 0;
		LONGLONG UpperIdxOutside = Len / (2*6);
		int CmpResult;
		for(i=0;;i++)
		{
			ElementIdx = LowerIdx + (((UpperIdxOutside-1) - LowerIdx) / 2);
			Offset = ElementIdx * (2*6);
			if(Offset >= Len)
			{
				return false;//error
			}
			pSrcFile->Seek(Offset, CFile::begin);
			pSrcFile->Read(&TableCw6Start, 6);
			pSrcFile->Read(&TableCw6End, 6);

			CmpResult = memcmp(TableCw6End, Cw6End, 6);
			if(CmpResult == 0)
			{
				//found
				addElementToList(pCw6StartEndIdxList, TableCw6Start, TableCw6End, m);

				//may be the element left or right has the same CwEnd => check also

				int ContSearch;
				LONGLONG ElementIdxLower;
				LONGLONG ElementIdxUpper;
				ElementIdxLower = ElementIdx;
				ElementIdxUpper = ElementIdx;

				//check lower idx
				do
				{
					ContSearch = false;
					ElementIdxLower--;
					if(ElementIdx >= 0)
					{
						Offset = ElementIdxLower * (2*6);
						assert(Offset < Len);
						pSrcFile->Seek(Offset, CFile::begin);
						pSrcFile->Read(&TableCw6Start, 6);
						pSrcFile->Read(&TableCw6End, 6);
						CmpResult = memcmp(TableCw6End, Cw6End, 6);
						if(CmpResult == 0)
						{
							//found
							addElementToList(pCw6StartEndIdxList, TableCw6Start, TableCw6End, m);
							ContSearch = true;
						}
					}
				}while(ContSearch);

				//check upper idx
				do
				{
					ContSearch = false;
					ElementIdxUpper++;
					Offset = ElementIdxUpper * (2*6);
					if(Offset < Len)
					{
						pSrcFile->Seek(Offset, CFile::begin);
						pSrcFile->Read(&TableCw6Start, 6);
						pSrcFile->Read(&TableCw6End, 6);
						CmpResult = memcmp(TableCw6End, Cw6End, 6);
						if(CmpResult == 0)
						{
							//found
							addElementToList(pCw6StartEndIdxList, TableCw6Start, TableCw6End, m);
							ContSearch = true;
						}
					}
				}while(ContSearch);

				break;
			}

			if((UpperIdxOutside-1) <= LowerIdx)
			{
				break;//not found
			}

			if(CmpResult < 0)
			{
				//table element to low
				LowerIdx = ElementIdx+1;
			}
			else
			{
				//table element to high
				UpperIdxOutside = ElementIdx;
			}
		}
	}

	return true;//ok
}

void CRainbowTab::FreeListElemets(void_vp *pList)
{
	//free list elements
	int Count;
	int i;
	uchar *Ptr;
	
	Count = pList->GetCount();
	for(i=0; i<Count; i++)
	{
		Ptr = (uchar*) pList->RemoveHead();
		assert(Ptr != NULL);
		delete Ptr;
	}

	assert(pList->IsEmpty());
}

void CRainbowTab::LoadCwEndValuesFromFile(uchar **pBuffer, int *pLen, String SrcFileName)
{
	//this fctn will alloc and caller must free Buffer
	CFile SrcFile;

	*pBuffer = NULL;
	*pLen = 0;

	if(!SrcFile.Open(SrcFileName, CFile::modeRead))
	{
		return;//file not present
	}

	uint64_t SrcLen = SrcFile.GetSize();
	if((SrcLen == 0) || ((SrcLen % 6) != 0))
	{
		return;//invalid content len
	}

	*pBuffer = new uchar[SrcLen];

	if(SrcFile.Read(*pBuffer, SrcLen) != SrcLen)
	{
		delete pBuffer;
		*pBuffer = NULL;
		return;//error reading file
	}
	*pLen = SrcLen;
}

} // end of namespace PCRTT
