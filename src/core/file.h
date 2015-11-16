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
#if !defined(PCRTT_SRC_FILE_HEADER_H_)
#define PCRTT_SRC_FILE_HEADER_H_

namespace PCRTT {

class File
{
public:
	#if defined(__USE_CPP_11__)
	File(const File&) = delete;
	void operator=(const File&) = delete;
	#endif // __USE_CPP_11__

	File( const String& path = "", const String& filename = "",
		std::ios_base::openmode uiOpenMode = ( std::ios_base::in |
			std::ios_base::binary ), bool bOpen = false );
	virtual ~File();

	virtual bool Open( const String& rstrPath, const String& rstrFilename,
		std::ios_base::openmode uiOpenMode);

	virtual bool Open( const String& filename, const String& mode );

	virtual void Close();
	virtual bool IsValid() const { return( m_pkStream && !( !*m_pkStream  ) ); }
	inline std::iostream* GetStream() { return m_pkStream; }

	virtual ulong GetSize();
	inline std::streampos Tellg() const;
	inline std::streampos Tellp() const;
	inline long GetPosition();
	virtual File& Read( void *pBuffer, int iNumBytes );
	virtual int ReadSome( void *pBuffer, int iNumBytes );
	virtual File& Write( const void *pBuffer, int iNumBytes );
	virtual int WriteSome( const void *pBuffer, int iNumBytes );

	virtual int read( void *pBuffer, int size, int iNumBytes );
	virtual int write( const void *pBuffer, int size, int iNumBytes );
	
	virtual File& putBack( char c );
	#undef getc
	virtual int getc();
	virtual int scanf( const char* fmt, ... );

	virtual int ReadString(char* buffer, int len);
	virtual int WriteString(const char* buffer, int len);

	virtual File& GetLine( char *pcDest, uint uiCount, char cDelimiter = '\n' );
	inline File& Seekg( std::streamoff uiOffset, std::ios_base::seekdir uiDirection );
	inline File& Seekp( std::streamoff uiOffset, std::ios_base::seekdir uiDirection );
	inline File& SeekFromStart( long length );
	inline File& SeekFromHere( long length );
	inline File& SeekFromEnd( long length );
	inline File& SeekToBegin(void) { Seekg( 0, std::ios_base::beg ); }

	const String& GetPath() const { return m_strPath; }

	const String& GetName() const { return m_strName; }

	bool DetermineByteOrder( int iReferenceValue );

	bool SetBinary( bool bBinary );

	bool IsBinary() const;

	bool DetermineBinaryMode( const uchar *pucReferenceValues );

	File& operator << ( bool bData );
	inline File& operator << ( char cData );
	inline File& operator << ( uchar ucData );
	inline File& operator << ( short sData );
	inline File& operator << ( ushort usData );
	inline File& operator << ( int iData );
	inline File& operator << ( uint uiData );
	inline File& operator << ( float fData );
	inline File& operator << ( long fData );
	inline File& operator << ( ulong fData );
	File 	&operator << ( const char *pszData );
	File 	&operator << ( const String& rstrData );

	File 	&operator >> ( bool& rbData );
	File 	&operator >> ( char& rcData );
	File 	&operator >> ( uchar& rucData );
	File 	&operator >> ( short& rsData );
	File 	&operator >> ( ushort& rusData );
	File 	&operator >> ( int& riData );
	File 	&operator >> ( uint& ruiData );
	File 	&operator >> ( float& rfData );
	File 	&operator >> ( long& rfData );
	File 	&operator >> ( ulong& rfData );
	File 	&operator >> ( String& rstrData );

	inline File& operator << ( std::ios_base& ( *pfnManip )( std::ios_base& ) );
	inline File& operator << ( std::basic_ostream< char >& ( *pfnManip )( std::basic_ostream< char >& ) );
	inline File& operator >> ( std::ios_base& ( *pfnManip )( std::ios_base& ) );
	inline File& operator >> ( std::basic_ostream< char >& ( *pfnManip )( std::basic_ostream< char >& ) );

	inline bool operator !() const;

	inline bool IsEOF() const;

	/**
	 * return the creation date size of the file
	 * ptr must be a pointer to a time_t struct
	 */
	static String creationDate(const void *ptr);

	/** return the size of the file */
	static ulong Size(const char* filename);

	/** return the size of the file in string version (Ko,Mo,Go,To, etc) */
	static String fileSize(const char* name,const bool& isDir,
					const bool& isFile, uint* sz = 0);
	/* filename must be fullpath */
	static String fileSize(struct stat* st,const char* filename);

	/** */
	static bool Exists(const char* filename);
	/** */
	static void Delete(const char* filename);
	/** */
	static void TempFile( char* dest, uint max_len );
	/** */
	static int Copy(const char* oldname,const char* newname,int overwrite,
						int deleteOld );
	/** */
	static int isDirectory( const char *path );

	/**
	 * change the name or location of a file
	 * On success, zero is returned.  On error, -1 is returned
	 * and errno is set appropriately.
	 */
	static int Rename(const char *oldpath, const char *newpath);

	/** merge callback, must return 1 to continue, 0 to stop merge. */
	typedef int (*cb_merge)( const char* message, const uint& total,
				const uint& writted, void* user_data );

	static int merge(const char* dst, const char* src, cb_merge cb,
				void* user_data );

	static int merge(File& dst,File& src,cb_merge cb = 0, void* user_data = 0);

	static int merge(File& dst, File& src1, File& src2, cb_merge cb = 0,
				void* user_data = 0 );

	// memory sorter
	static int sort( File& dst, File& src1, File& src2, uint chunkSize,
				uint scmp, uint offset, int uniq = 0,
				cb_merge cb = 0, void* user_data = 0 );

	static int sort_external( File& dst, File& src1, File& src2,
				uint chunkSize, uint scmp, uint offset, cb_merge cb = 0,
				void* user_data = 0 );

protected:
	virtual void 	AllocStream( const String& rstrFullPath );

	std::iostream 	*m_pkStream;
	std::ios_base::openmode m_uiOpenMode;
	int 			m_bBinary;
	String 			m_strPath;
	String 			m_strName;

}; // End of class File

#include "file.inl"

} // end of namespace PCRTT

#endif	// PCRTT_SRC_FILE_HEADER_H_
