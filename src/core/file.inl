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
inline std::streampos File::Tellg() const
{
	return( m_pkStream ? m_pkStream->tellg() : (std::streampos)0 );
}

inline std::streampos File::Tellp() const
{
	return( m_pkStream ? m_pkStream->tellp() : (std::streampos)0 );
}

inline File& File::Seekg( std::streamoff uiOffset, std::ios_base::seekdir uiDirection )
{
	if( m_pkStream ) m_pkStream->seekg( uiOffset, uiDirection );
	return( *this );
}

inline File& File::Seekp( std::streamoff uiOffset, std::ios_base::seekdir uiDirection )
{
	if( m_pkStream ) m_pkStream->seekp( uiOffset, uiDirection );
	return( *this );
}

inline long File::GetPosition()
{
	return Tellg();
}
	
inline File& File::SeekFromStart( long length )
{
	return Seekg(length, std::ios_base::beg);
}

inline File& File::SeekFromHere( long length )
{
	return Seekg(length, std::ios_base::cur);
}

inline File& File::SeekFromEnd( long length )
{
	return Seekg(length, std::ios_base::end);
}

inline bool File::operator !() const
{
	return !IsValid();
}

//! expand write operator
#define WRITE_READ(_t_)	inline File& File::operator << ( _t_ cData ) { \
			if( m_bBinary ) m_pkStream->write( (char*)&cData,sizeof( _t_ ) ); \
			else		*m_pkStream << cData; return( *this ); }

WRITE_READ(char)
WRITE_READ(uchar)
WRITE_READ(short)
WRITE_READ(ushort)
WRITE_READ(int)
WRITE_READ(uint)
WRITE_READ(float)
WRITE_READ(long)
WRITE_READ(ulong)

#undef WRITE_READ

inline File& File::operator << ( std::ios_base& ( *pfnManip )( std::ios_base& ) )
{
	(*pfnManip)( *( std::ios_base* )m_pkStream );
	return( *this );
}

inline File& File::operator << ( std::basic_ostream< char >& ( *pfnManip )( std::basic_ostream< char >& ) )
{
	(*pfnManip)( *( std::basic_ostream< char >* )m_pkStream );
	return( *this );
}

inline File& File::operator >> ( std::ios_base& ( *pfnManip )( std::ios_base& ) )
{
	(*pfnManip)( *( std::ios_base* )m_pkStream );
	return( *this );
}

inline File& File::operator >> ( std::basic_ostream< char >& ( *pfnManip )( std::basic_ostream< char >& ) )
{
	(*pfnManip)( *( std::basic_ostream< char >* )m_pkStream );
	return( *this );
}

inline bool File::IsEOF() const
{
	return ( ( m_pkStream ) ? m_pkStream->eof () : true );
}

// -----------------------------------------------------------------------------
// file.inl - Last Change: $Date: 2012-02-20 13:46:16 $ - End Of File
// -----------------------------------------------------------------------------
