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
#include <fstream>
#include <limits>
#include <cstddef>
#include <time.h>
#include <stdlib.h>
#include <chrono>
#include <ctime>
#include <iostream>
#include <vector>


//#if defined(__USE_CPP_11__)
//#include <utility>
//#else
#include <algorithm>
//#endif // 

#if defined(__STXXL__)
#include <stxxl/io>
#include <stxxl/vector>
#include <stxxl/stream>
#else
extern "C" {
#include <extsort/IntegerSorting.h>
#include <extsort/BitArray.h>
#include <extsort/ExternalSortAPI.h>
#include <extsort/ExternalSort.h>
#include <extsort/CrashInfo.h>
};
#endif // __STXXL__

#include <core/sort.h>

#undef max

using namespace std;

namespace PCRTT {

#define _1KO	(1024.0f)
#define _1MO	(_1KO * _1KO)
#define _1GO	(_1MO * _1KO)
#define _1TO	(_1GO * _1KO)

#if defined(__BIG_ENDIAN__)

#define bswap_16(x) (((x) & 0x00ff) << 8 | ((x) & 0xff00) >> 8)

#define bswap_32(x) \
	((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) | \
	(((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24))

static inline unsigned long long bswap_64(unsigned long long x) {
  return (((unsigned long long)bswap_32(x&0xffffffffull))<<32) | (bswap_32(x>>32));
}

static inline short shortSwap( short s )
{
	uchar b1, b2;
  
	b1 = s & 255;
	b2 = (s >> 8) & 255;

	return (b1 << 8) + b2;
}

static inline int longSwap( int i )
{
	uchar b1, b2, b3, b4;

	b1 = i & 255;
	b2 = ( i >> 8 ) & 255;
	b3 = ( i>>16 ) & 255;
	b4 = ( i>>24 ) & 255;

	return ((int)b1 << 24) + ((int)b2 << 16) + ((int)b3 << 8) + b4;
}

static inline float floatSwap( float f )
{
	union {
		float f;
		uchar b[4];
	} dat1, dat2;

	dat1.f = f;

	dat2.b[0] = dat1.b[3];
	dat2.b[1] = dat1.b[2];
	dat2.b[2] = dat1.b[1];
	dat2.b[3] = dat1.b[0];

	return dat2.f;
}

#endif	// __BIG_ENDIAN__

#define IN std::ios_base::in
#define OUT	std::ios_base::out
#define BINARY std::ios_base::binary
#define TRUNC std::ios_base::trunc
#define APP std::ios_base::app


File::File(const String &rstrPath,const String &rstrName,
	std::ios_base::openmode uiOpenMode, bool bOpen
) : m_pkStream( NULL ),
	m_uiOpenMode( uiOpenMode ),
	m_bBinary( 1 ),
	m_strPath( ( !rstrPath.length() || 
		( rstrPath[ rstrPath.length() - 1 ] != '/' ) ) ? 
			rstrPath : rstrPath.substr( 0, rstrPath.length() - 1 ) ),
	m_strName( rstrName )
{
	if( bOpen )
	{
		Open("","",(std::ios_base::openmode)0);
	}
}

File::~File()
{
	Close();
}

bool File::Open(const String &rstrPath,const String &rstrFilename,
	std::ios_base::openmode uiOpenMode )
{
	Close();

	if( rstrFilename.length() )
	{
		m_strName = rstrFilename;
	}

	if( rstrPath.length() )
	{
		m_strPath = ( !rstrPath.length() || 
						( rstrPath[ rstrPath.length() - 1 ] != '/' ) ) ?
							rstrPath : rstrPath.substr(0,rstrPath.length()-1);
	}

	if( uiOpenMode != 0 )
	{
		m_uiOpenMode = uiOpenMode;
	}

	if( !m_uiOpenMode )
	{
		m_uiOpenMode = IN | BINARY;
	}

	m_bBinary = ( ( m_uiOpenMode & BINARY ) != 0 );

	String strFullPath;

	if( m_strPath.length() )
	{
		strFullPath = m_strPath;

		if( strFullPath[ strFullPath.length() - 1 ] != '/' )
		{
			strFullPath += '/';
		}

		strFullPath += m_strName;
	}
	else
	{
		strFullPath = m_strName;
	}

	AllocStream( strFullPath );

	if( !m_pkStream || !( *m_pkStream ) )
	{
		Close();
		return false;
	}

	return true;
}

bool File::Open( const String& filename, const String& mode )
{
	const int len = mode.size();
	Close();

	m_strName = filename;

	#define M1(a)	  ((1==len)&&(mode[0]==a))
	#define M2(a,b)	  ((2==len)&&(mode[0]==a)&&(mode[1]==b) )
	#define M3(a,b,c) ((3==len)&&(mode[0]==a)&&(mode[1]==b)&&(mode[2]==c))

	// we work always in binary mode
	m_uiOpenMode = BINARY;

	if ( M1('w') )				m_uiOpenMode |= OUT;
	else if ( M1('r') )			m_uiOpenMode |= IN;
	else if ( M1('a') )			m_uiOpenMode |= OUT | APP;
	else if ( M2('w','b') )		m_uiOpenMode |= OUT;
	else if ( M2('r','b') )		m_uiOpenMode |= IN;
	else if ( M2('r','+') )		m_uiOpenMode |= IN | OUT;
	else if ( M2('w','+') )		m_uiOpenMode |= IN | OUT | TRUNC;
	else if ( M2('a','+') )		m_uiOpenMode |= IN | OUT | APP;
	else if ( M3('w','b','+') )	m_uiOpenMode |= IN | OUT;
	else if ( M3('r','b','+') )	m_uiOpenMode |= IN | OUT;

	#undef M1
	#undef M2
	#undef M3

	m_bBinary = ( ( m_uiOpenMode & BINARY ) != 0 );

	AllocStream( m_strName );

	if( !m_pkStream || !( *m_pkStream ) )
	{
		Close();
		return false;
	}

	return true;
}

void File::Close()
{
	KILLOBJECT(m_pkStream);
}

void File::AllocStream( const String &rstrFullPath )
{
	m_pkStream = new fstream( rstrFullPath.c_str(), m_uiOpenMode | BINARY );
}

int File::ReadString(char* buffer, int len)
{
	m_pkStream->read( buffer, len );
	return len;
}

int File::WriteString(const char* buffer, int len)
{
	if( m_bBinary )
		do m_pkStream->write( buffer, 1 ); while( *buffer++ );
	else
		*m_pkStream << buffer;

	return len;
}

ulong File::GetSize()
{
#if 1
	return File::Size(m_strName);
#else
	bool bClose = false;

	if ( !IsValid() )
	{
		if( !Open("","",(std::ios_base::openmode)0) )
			return -1;

		bClose = true;
	}

	streampos uiCur = Tellg();

	Seekg( 0, ios_base::end );

	ulong iSize = (ulong)Tellg();

	Seekg( uiCur, ios_base::beg );

	if( bClose )
		Close();

	return iSize;
#endif
}

File &File::GetLine( char *pcDest, uint uiCount, char cDelimiter )
{
	if( !uiCount )
		uiCount = numeric_limits< int >::max();

	if( pcDest )
		m_pkStream->getline( pcDest, uiCount, cDelimiter );
	else
		m_pkStream->ignore( uiCount, cDelimiter );

	return( *this );
}

File &File::operator << ( bool bData )
{
	if( m_bBinary )
	{
		uchar ucVal = ( bData ? 1 : 0 );
		m_pkStream->write( (char*)&ucVal, 1 );
	}
	else
		*m_pkStream << ( bData ? "true" : "false" );

	return( *this );
}

File &File::operator << ( const char *pszData )
{
	if( m_bBinary )
		do m_pkStream->write( pszData, 1 ); while( *pszData++ );
	else
		*m_pkStream << pszData;

	return( *this );
}

File &File::operator << ( const String &rstrData )
{
	if( m_bBinary )
		*this << rstrData.c_str();
	else
		*m_pkStream << rstrData;

	return( *this );
}

File &File::operator >> ( bool &rbData )
{
	if( m_bBinary )
	{
		uchar ucVal = 0;
		m_pkStream->read( (char*)&ucVal, 1 );
		rbData = ( ucVal ? true : false );
	}
	else
	{
		//String strVal;
		char str[256];
		*m_pkStream >> str;
		rbData = ( ( strstr(str,"false") ) ? false : true );
	}

	return( *this );
}

File &File::operator >> ( uchar &rucData )
{
	if( m_bBinary )
		m_pkStream->read( (char*)&rucData, 1 );
	else
		*m_pkStream >> rucData;

	return( *this );
}

File &File::operator >> ( short &rsData )
{
	if( m_bBinary )
		m_pkStream->read( (char*)&rsData, 2 );
	else
		*m_pkStream >> rsData;

	return( *this );
}

File &File::operator >> ( ushort &rusData )
{
	if( m_bBinary )
		m_pkStream->read( (char*)&rusData, 2 );
	else
		*m_pkStream >> rusData;

#if defined(__BIG_ENDIAN__)
	rusData = bswap_16( rusData );
#endif	// BIG_ENDIAN

	return( *this );
}

File &File::operator >> ( int &riData )
{
	if( m_bBinary )
		m_pkStream->read( (char*)&riData, 4 );
	else
		*m_pkStream >> riData;

#if defined(__BIG_ENDIAN__)
	riData = bswap_32( riData ) ;	// ntohl( riData );
#endif	// BIG_ENDIAN

	return( *this );
}

File &File::operator >> ( uint &ruiData )
{
	if( m_bBinary )
		m_pkStream->read( (char*)&ruiData, 4 );
	else
		*m_pkStream >> ruiData;

#if defined(__BIG_ENDIAN__)
	ruiData = bswap_32( ruiData );
#endif	// BIG_ENDIAN

	return( *this );
}

File &File::operator >> ( float &rfData )
{
	if( m_bBinary )
		m_pkStream->read( (char*)&rfData, 4 );
	else
		*m_pkStream >> rfData;

#if defined(__BIG_ENDIAN__)

	// rfData = floatSwap( rfData );
	uint* data = (uint*)&rfData;
	*data = bswap_32( *data );

#endif	// BIG_ENDIAN

	return( *this );
}

File &File::operator >> ( long &rfData )
{
	if( m_bBinary )
		m_pkStream->read( (char*)&rfData, sizeof (long) );
	else
		*m_pkStream >> rfData;

#if defined(__BIG_ENDIAN__)
	rfData = bswap_32( rfData );
#endif	// BIG_ENDIAN

	return( *this );
}

File &File::operator >> ( ulong &rfData )
{
	if( m_bBinary )
		m_pkStream->read( (char*)&rfData, sizeof (ulong) );
	else
		*m_pkStream >> rfData;

#if defined(__BIG_ENDIAN__)
	rfData = bswap_32( rfData );
#endif	// BIG_ENDIAN

	return( *this );
}

File &File::operator >> ( String &rstrData )
{
	//if( m_bBinary )
	{
		static const int uiSize = 128;

		char  acData[uiSize];
		char *pcData = acData;
		char *pcEnd  = acData + uiSize;

		while( true )
		{
			if( !m_pkStream->read( pcData, 1 ) )
				break;

			if( !*pcData )
				break;

			if( ++pcData == pcEnd )
			{
				rstrData.append( acData, uiSize );
				pcData = acData;
			}
		}

		if( pcData != acData )
			rstrData.append( acData, pcData - acData );
	}
	//else
	//	*m_pkStream >> str;

	return( *this );
}

File &File::Read( void *pBuffer, int iNumBytes )
{
	assert(m_pkStream);

	m_pkStream->read( (char*)pBuffer, iNumBytes );

#if defined(__BIG_ENDIAN__)

	uchar* data = (uchar*)pBuffer;

	for ( int i=0;i < iNumBytes; i+=4 )
	{
		uint* d = (uint*)&data[i];
		*d = bswap_32( *d );
	}

#endif	// __BIG_ENDIAN__

	return( *this );
}

File &File::putBack( char c )
{
	m_pkStream->putback ( c );
	return( *this );
}

int File::getc()
{
	char c;
	m_pkStream->get( c );
	return c;
}

int File::read( void *pBuffer, int /*size*/, int iNumBytes )
{
	return m_pkStream->readsome( (char*)pBuffer, iNumBytes );
}

int File::scanf( const char* fmt, ... )
{
	char str[257];
	int result = 0;
	va_list argList;

	read( str, 1, 256 );

	va_start( argList, fmt );
#if defined(__WIN32__)
	result = vsnprintf( str, 256, fmt, argList );
#else
	result = vsscanf( str, fmt, argList );
#endif	// 
	va_end( argList );

	return result;
}

int File::ReadSome( void *pBuffer, int iNumBytes )
{
	return m_pkStream->readsome( (char*)pBuffer, iNumBytes );
}

File &File::Write( const void *pBuffer, int iNumBytes )
{
	m_pkStream->write( (char*)pBuffer, iNumBytes );

	return( *this );
}

int File::WriteSome( const void *pBuffer, int iNumBytes )
{
	size_t b = Tellp();
	m_pkStream->write( (char*)pBuffer, iNumBytes );
	size_t a = Tellp();
	return (a - b);
}

int File::write( const void *pBuffer, int /*size*/, int iNumBytes )
{
	long s = 0, e=0;
	s = GetPosition();
	m_pkStream->write( (char*)pBuffer, iNumBytes );
	e = GetPosition();
	return e-s;
}

bool File::DetermineByteOrder( int iReferenceValue )
{
	bool bBinary   = IsBinary();
	int  iCurValue = 0;

	int iCurPos = Tellg();

	SetBinary( true );

	*this >> iCurValue;

	SetBinary( bBinary );

	Seekg( iCurPos, ios_base::beg );

	if( iCurValue == iReferenceValue )
		return true; //we have correct byte ordering

	//try swapping bytes
	uchar cTemp;
	uchar *pucCur = (uchar*)&iCurValue;
	uchar *pucSwap = pucCur + ( sizeof( int ) - 1 );

	while( pucCur < pucSwap )
	{
		cTemp = *pucCur;

		*pucCur  = *pucSwap;
		*pucSwap = cTemp;

		++pucCur;
		--pucSwap;
	}

	return true;
}

bool File::SetBinary( bool bBinary )
{
	bool bRet = IsBinary();

	m_bBinary = bBinary;

	return bRet;
}

bool File::IsBinary() const
{
	return m_bBinary;
}

bool File::DetermineBinaryMode( const uchar *pucReferenceValues )
{
	uchar aucRead[4];

	streampos kCurPos = m_pkStream->tellg();

	m_pkStream->read( (char*)aucRead, 4 );

	m_pkStream->seekg( kCurPos );

	return( m_bBinary = ( ( aucRead[0] == pucReferenceValues[0] ) && 
		( aucRead[1] == pucReferenceValues[1] ) && 
		( aucRead[2] == pucReferenceValues[2] ) && 
		( aucRead[3] == pucReferenceValues[3] ) ) );
}

ulong File::Size(const char* filename)
{
	ulong size = 0;

	// posix compliant - handle large file
#if defined(__WIN32__) && !defined(__MINGW__) && !defined(__CYGWIN__)
	int fd;
	long file_size;

	_sopen_s(&fd, filename, _O_RDONLY, _SH_DENYRW, _S_IREAD);

	if (fd == -1)
	{
		TRACE_ERROR( "Can't open file: %s error: %s", filename,
			strerror(errno) );
		return(0);
	}

	file_size = _filelength( fd );

	if (file_size == -1)
	{
		TRACE_ERROR( "Can't open file: %s error: %s", filename,
			strerror(errno) );
		if(fd) _close(fd);
		return(0);
	}

	if(fd) _close(fd);

	size = (ulong)file_size;

#elif defined(HAVE_OPEN) && defined(HAVE_FSTAT) && defined(HAVE_FDOPEN) && \
	defined(HAVE_FSEEKO)

	FILE* fp;
	int fd;
	off_t file_size;
	char *buffer;
	struct stat st;

	fd = open(filename, O_RDONLY);

	if (fd == -1)
	{
		TRACE_ERROR( "Can't open file: %s error: %s", filename,
			strerror(errno) );
		return(0);
	}

	fp = fdopen(fd, "r");

	if (fp == NULL)
	{
		TRACE_ERROR( "Can't open file: %s error: %s", filename,
			strerror(errno) );
		return(0);
	}

	/* Ensure that the file is a regular file */
	if ((fstat(fd, &st) != 0) || (!S_ISREG(st.st_mode)))
	{
		TRACE_ERROR( "Can't open file: %s error: %s", filename,
			strerror(errno) );
		if (fp) fclose(fp);
		return(0);
	}

	if (fseeko(fp, 0 , SEEK_END) != 0)
	{
		TRACE_ERROR( "Can't open file: %s error: %s", filename,
			strerror(errno) );
		if (fp) fclose(fp);
		return(0);
	}
	   
	file_size = ftello(fp);

	if (file_size == -1)
	{
		TRACE_ERROR( "Can't open file: %s error: %s", filename,
			strerror(errno) );
		if (fp) fclose(fp);
		return(0);
	}

	if (fp) fclose(fp);

	size = (ulong)file_size;

#elif defined(HAVE_OPEN) && defined(HAVE_FSTAT)
	off_t file_size;
	struct stat stbuf;
	int fd;

	fd = open(filename, O_RDONLY);

	if (fd == -1)
	{
		TRACE_ERROR( "Can't open file: %s error: %s", filename,
			strerror(errno) );
		return(0);
	}
  
	if ((fstat(fd, &stbuf) != 0) || (!S_ISREG(stbuf.st_mode)))
	{
		TRACE_ERROR( "Can't open file: %s error: %s", filename,
			strerror(errno) );
		if(fd) close(fd);
		return(0);
	}

 	if(fd) close(fd);

	file_size = stbuf.st_size;

	size = (ulong)file_size;
#else
	FILE *fp = 0;

	if( (fp = fopen( filename, "rb" )) == 0)
		return 0;

	// read the size
	fseek( fp, 0, SEEK_END );
	size = (ulong)ftell( fp );
	fseek( fp, 0, SEEK_SET );
	fclose(fp);
#endif // 0

	return size;
}

bool File::Exists(const char* filename)
{
	FILE* fp = fopen( filename, "rb" );
	if(fp)
	{
		fclose(fp);
		return true;
	}

	return false;
}

void File::Delete(const char* filename)
{
	unlink( filename );
}

void File::TempFile( char* dest, uint max_len )
{
	static int id = 0;
	//int snprintf (char *str, size_t size, const char  *format, ...);
	sprintf(dest, /*max_len,*/ "/tmp/.temp_%04d", id++ );
	while ( Exists(dest) )
		TempFile(dest, max_len);
}

int File::Copy(const char* oldname,const char* newname,int overwrite,
	int deleteOld)
{
	uchar buff[2048]; // << this one can be optimized
	int readBytes = 1;
	FILE* f_in, *f_out;

	// if destination exist and user does not want to overwrite it, just exit.
	if ( Exists(newname) && !overwrite )
		return 1;

	// open source
	f_in = fopen( oldname, "rb" );

	// cant open file source
	if ( !f_in )
		return 0;

	// open destination (we can overwrite it)
	f_out = fopen( newname, "wb" );

	// cant open file destination
	if ( !f_out )
	{
		if ( f_in ) fclose(f_in);
		return 0;
	}

	// copy
	while ( readBytes )
	{
		readBytes = fread( buff, 1, 2048, f_in );
		fwrite( buff, 1, readBytes, f_out );
	}

	// close both file
	fclose(f_in);
	fclose(f_out);

	// if user want to delete file source
	if ( deleteOld )
		unlink( oldname );

	return 1;
}

int File::isDirectory( const char *path )
{ return isdirectory(path); }

String File::creationDate(const void *ptr)
{
	const time_t *timep = (const time_t *)ptr;
	char str[100] = {0};
	const struct tm* time = ::localtime( timep );
	strftime( str, sizeof(str), "%d/%m/%y", time );
	return String(str);
}

String File::fileSize(const char* name,const bool& isDir,const bool& isFile,
	uint* sz )
{
	if ( isDir )
	{
		if (sz) *sz = 4096;
		return String("4096");
	}
	if (!isFile)
	{
		if (sz) *sz = 0;
		return String("0");
	}

	String r;
	float size;

	if ( sz && *sz > 0 )
	{
		size = (float)*sz;
	}
	else
	{
		size = (float) Size( name );
		if ( sz )
			*sz = (uint)size;
	}

	if ( size > _1TO ) r = format("%0.1f To", (size / _1TO));
	else if ( size > _1GO ) r = format("%0.1f Go", (size / _1GO));
	else if ( size > _1MO ) r = format("%0.1f Mo", (size / _1MO));
	else if ( size > _1KO ) r = format("%0.1f Ko", (size / _1KO));
	else r = format("%u Octet(s)", size);

	return r;
}

String File::fileSize(struct stat* st,const char* filename)
{
	const bool isFile = S_ISREG(st->st_mode);
	const bool isDir = S_ISDIR(st->st_mode);

	if ( isDir )
		return String("4096");
	if (!isFile)
		return String("0");

	String r;
	float size;

	if ( st->st_size > 0 )
		size = (float)st->st_size;
	else
		size = (float)Size( filename );

	if ( size > _1TO ) r = format("%0.1f To", (size / _1TO));
	else if ( size > _1GO ) r = format("%0.1f Go", (size / _1GO));
	else if ( size > _1MO ) r = format("%0.1f Mo", (size / _1MO));
	else if ( size > _1KO ) r = format("%0.1f Ko", (size / _1KO));
	else r = format("%u Octet(s)", size);

	return r;
}

int File::Rename(const char *oldpath, const char *newpath)
{
	return ::rename(oldpath,newpath);
}

int File::merge(const char* sdst, const char* ssrc, File::cb_merge cb,
	void* user_data )
{
	File dst, src;
	if ( !sdst || !ssrc )
	{
		TRACE_ERROR(
			"Invalid destination file nor source file:\n"
			"dst: %s src: %s", 
			sdst ? "ok": "ko", ssrc ? "ok": "ko"
		);
		return 0;
	}

	if(!dst.Open( sdst, "a+" ))
	{
		TRACE_ERROR( "can't open destination file: %s\n", sdst );
		return 0;
	}

	if(!src.Open( ssrc, "rb" ))
	{
		TRACE_ERROR( "can't open source file: %s\n", ssrc );
		return 0;
	}

	const uint total	= src.GetSize();
	const uint sz		= 8192;
	uint writted		= 0;
	uchar buff[8192+1]	= {0};
	char str[256];

	if ( cb )
	{
		snprintf(str, sizeof(str), "Merging %s into %s ...", ssrc, sdst );
		cb(str, total, writted, user_data );
	}

	/** append src to dst */
	for( ;; )
	{
		int len = src.ReadSome( buff, sz );

		if (!len)
			break;

		int wr = dst.WriteSome( buff, len );

		if( wr != len )
		{
			TRACE_ERROR("Can't write to dest file.(len: %d, wr: %d)", len,wr);
			return 0;
		}

		writted += len;

		if ( cb )
		{
			if ( !cb( 0, total, writted, user_data ) )
			{
				break; // user request
			}
		}
	}

	/** all done */
	if ( cb )
	{
		cb( "Merge done.", total, writted, user_data );
	}

	return writted;
}

int File::merge(File& dst, File& src, File::cb_merge cb, void* user_data )
{
	if ( !dst.IsValid() || !src.IsValid() )
	{
		TRACE_ERROR(
			"Invalid destination file nor source file:\n"
			"dst: %s src: %s", 
			dst.IsValid() ? "ok": "ko",	src.IsValid() ? "ok": "ko"
		);
		return 0;
	}

	const uint total	= src.GetSize();
	const uint sz		= 8192;
	uint writted		= 0;
	uchar buff[8192+1]	= {0};

	/** append src to dst */
	for( ;; )
	{
		int len = src.ReadSome( buff, sz );

		if (!len)
			break;

		int wr = dst.WriteSome( buff, len );

		if( wr != len )
		{
			TRACE_ERROR("Can't write to dest file.(len: %d, wr: %d)", len,wr);
			return 0;
		}

		writted += len;

		if ( cb )
		{
			if ( !cb( 0, total, writted, user_data ) )
			{
				break; // user request
			}
		}
	}

	/** all done */
	if ( cb )
	{
		cb( 0, total, writted, user_data );
	}

	return writted;
}


int File::merge( File& dst, File& src1, File& src2, File::cb_merge cb,
	void* user_data )
{
	if (!dst.IsValid() || !src1.IsValid() || !src2.IsValid() )
	{
		TRACE_ERROR(
			"Invalid destination file nor source file:\n"
			"dst: %s src1: %s src2: %s", 
			dst.IsValid() ? "ok": "ko",
			src1.IsValid() ? "ok": "ko",
			src2.IsValid() ?  "ok": "ko"
		);
		return 0;
	}

	const uint total	= src1.GetSize() + src2.GetSize();
	const uint sz		= 8192;
	uint writted		= 0;
	uchar buff[8192+1]	= {0};

	if ( cb )
	{
		/** append src1 to dst */
		for( ;; )
		{
			int len = src1.ReadSome( buff, sz );

			if (!len)
				break;

			int wr = dst.WriteSome( buff, len );

			if( wr != len )
			{
				TRACE_ERROR("Can't write to dest file.(len: %d, wr: %d)",
					len,wr);
				return 0;
			}

			writted += len;

			if (!cb( 0, total, writted, user_data ))
			{
				break; // user request
			}
		}

		/** store src2 to dst */
		for( ;; )
		{
			int len = src2.ReadSome( buff, sz );

			if (!len)
				break;

			int wr = dst.WriteSome( buff, len );

			if( wr != len )
			{
				TRACE_ERROR("Can't write to dest file.(len: %d, wr: %d)",
					len,wr);
				return 0;
			}

			writted += len;

			if (!cb( 0, total, writted, user_data ))
			{
				break; // user request
			}
		}

		/** all done */
		cb( 0, total, writted, user_data );
	}
	else // no cb
	{
		/** append src1 to dst */
		for( ;; )
		{
			int len = src1.ReadSome( buff, sz );

			if (!len)
				break;

			int wr = dst.WriteSome( buff, len );

			if( wr != len )
			{
				TRACE_ERROR("Can't write to dest file.(len: %d, wr: %d)",
					len,wr);
				return 0;
			}

			writted += len;
		}

		/** store src2 to dst */
		for( ;; )
		{
			int len = src2.ReadSome( buff, sz );

			if (!len)
				break;

			int wr = dst.WriteSome( buff, len );

			if( wr != len )
			{
				TRACE_ERROR("Can't write to dest file.(len: %d, wr: %d)",
					len,wr);
				return 0;
			}

			writted += len;
		}
	}

	return writted;
}

int File::sort(File& dst, File& src1, File& src2, uint chunkSize, uint sCmp,
	uint offset, int uniq, cb_merge cb, void* user_data
)
{
	uint total=0;
	int i, result=0;
	File of;
	char temp[256]={0};
	Chunk_v buffer;
	Chunk_v::iterator last;

	File::TempFile( temp, sizeof(temp) );

	of.Open( temp, "w+" );

	TRACE_DEBUG("Merging to %s", temp );

	if ( !merge( of, src1, src2 ) )
	{
		of.Close();
		File::Delete( temp );
		return 0;
	}

	// reset position
	uint size = of.GetSize();

	of.Seekg( 0, ios_base::beg );
	of.Seekp( 0, ios_base::beg );

	/** copy whole file to memory (warning) */
	total = size;

	if ( cb )
	{
		cb( "Copying file to memory... be patient.",
			total, result, user_data );
	}

	for( ;; )
	{
		Chunk_t t;
		t.scmp = sCmp;
		t.offset = offset;
		t.data = new uchar[chunkSize];
		int len = of.ReadSome( t.data, chunkSize );

		if ( !len )
		{
			KILLARRAY( t.data );
			break;
		}

		t.size = len;
		buffer.push_back(t);
		result += len;
		
		if ( cb )
		{
			if (!cb( 0, total, result, user_data ))
			{
				of.Close();
				File::Delete( temp );
				return 0;
			}
		}
	}

	result = 0;

	if ( cb )
	{
		cb( "Sorting data...",
			total, result, user_data );
	}

	// 
	TRACE_DEBUG("Size readed: %d", result );

#if 1 // CPU parallel sort
	double time_elapsed = PCRTT::sort( buffer, 8, 20 );
#else
	time_t begin = std::clock();

	std::sort( buffer.begin(), buffer.end(), chunk_t );

	time_t end = std::clock();
	double time_elapsed = double(end-begin)/CLOCKS_PER_SEC;
#endif //

	std::cout << "time to sort file: " << time_elapsed << std::endl;

	/** unique */
	if ( uniq )
	{
		last = std::unique(buffer.begin(), buffer.end());
		buffer.erase(last, buffer.end()); 
	}

	// write sorted data to file
	of.Seekg( 0, ios_base::beg );
	of.Seekp( 0, ios_base::beg );
	result = 0;

	if ( cb )
	{
		cb( "Writing sorted data...",
			total, result, user_data );
	}

	for( i=0; i < buffer.size(); ++i )
	{
		int len = of.WriteSome( buffer[i].data, buffer[i].size );

		if ( len != buffer[i].size )
		{
			TRACE_ERROR("Can't write data to file (chunk: %d, total: %d)",
				i, buffer.size() );
			break;
		}
		result += buffer[i].size;

		/* delete this chunk of memory */
		KILLARRAY(buffer[i].data);
		buffer[i].size = 0;

		if ( cb )
		{
			if (!cb( 0, total, result, user_data ))
			{
				of.Close();
				File::Delete( temp );
				return 0;
			}
		}
	}

	/** free remaining memory (in case of error) */
	for( i=0; i < buffer.size(); ++i )
	{
		if(!buffer[i].size) continue;

		KILLARRAY(buffer[i].data);
		buffer[i].size = 0;
	}

	/** reset position */
	of.Seekg( 0, ios_base::beg );
	of.Seekp( 0, ios_base::beg );

	result = 0;
	if ( cb )
	{
		cb( "Merging final data...",
			total, result, user_data );
	}

	result = merge( dst, of, cb, user_data);

	of.Close();
	File::Delete( temp );

	return result;
}

int File::sort_external( File& dst, File& src1, File& src2,
				uint chunkSize, uint scmp, uint offset, File::cb_merge cb,
				void* user_data )
{
#if defined(__STXXL__)
	char temp[256]={0}, outname[256];
	File of;
	File::TempFile( temp, sizeof(temp) );

	strncpy( outname, pDstFile->GetName(), sizeof(outname) );
	pDstFile->Close();

	of.Open( temp, "w+" );

	// pass 1 merge src1 and src2 to temp
	// pass 2 copy and sort temp to dst
	if ( !File::merge( of, *src1, *src2, cb_merge, worker ) )
	{
		return 0;
	}
	of.Close();

	const stxxl::internal_size_type memory_to_use = 512 * 1024 * 1024;
	const stxxl::internal_size_type block_size = sizeof(my_type) * 4096;

	typedef stxxl::vector<my_type, 1, stxxl::lru_pager<2>, block_size> vector_type;

	stxxl::syscall_file in_file(temp, stxxl::file::DIRECT | stxxl::file::RDONLY);
	stxxl::syscall_file out_file(outname, stxxl::file::DIRECT | stxxl::file::RDWR | stxxl::file::CREAT);

	vector_type input(&in_file);
	vector_type output(&out_file);

	output.resize(input.size());

	typedef stxxl::stream::streamify_traits<vector_type::iterator>::stream_type input_stream_type;
	input_stream_type input_stream = stxxl::stream::streamify(input.begin(), input.end());

	typedef Cmp comparator_type;

	typedef stxxl::stream::sort<input_stream_type, comparator_type, block_size> sort_stream_type;
	sort_stream_type sort_stream(input_stream, comparator_type(), memory_to_use);

	vector_type::iterator o = stxxl::stream::materialize(sort_stream, output.begin(), output.end());

	STXXL_ASSERT(o == output.end());
#else
	char temp[256]={0}, outname[256];
	File of;
	File::TempFile( temp, sizeof(temp) );

	strncpy( outname, dst.GetName(), sizeof(outname) );
	dst.Close();

	of.Open( temp, "w+" );

	// pass 1: merge src1 and src2 to temp
	if ( !File::merge( of, src1, src2, cb, user_data ) )
	{
		return 0;
	}
	of.Close();

	/**
	 * pass 2a: if destiation exist
	 * sort temp to tmp2 then merge tmp2 to dst
	 */
	if ( File::Exists(outname) )
	{
		char tmp2[256]={0};
		File::TempFile( tmp2, sizeof(tmp2) );
		
		if ( cb )
		{
			cb("External sort in progress ...", 100,0,user_data);
		}

		ExSort( temp, tmp2, chunkSize, 0, 1, 0 );
		
		if ( cb )
		{
			cb("External sort done.", 100,100,user_data);
		}

		File::Delete( temp );

		if ( !File::merge( outname, tmp2, cb, user_data ) )
		{
			TRACE_ERROR("Can't merge %s into %s", tmp2, outname );
			File::Delete( tmp2 );
			return 0;
		}
		File::Delete( tmp2 );
	}
	// pass 2b: sort temp to dst
	else
	{
		// key_size sort_key_start sort_key_end in_file out_file endian
		ExSort( temp, outname, chunkSize, 0, 1, 0 );

		File::Delete( temp );
	}

#endif // __STXXL__

	return 1;
}

} // end of namespace PCRTT
