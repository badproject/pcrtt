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

inline const char *String::ascii() { return _data; }
inline const char *String::ascii() const { return _data; }

inline const char *String::c_str() { return _data; }
inline const char *String::c_str() const { return _data; }
inline int String::length() const { return _length; }

inline String::operator const char*() { return _data; }
inline String::operator const char*() const { return _data; }
	
inline char& String::operator[](int index) { return _data[index]; }
inline char String::operator[](int index) const { return _data[index]; }

inline int String::size() const { return _length; }

inline string_v	String::explode( char c )  const { return explode(_data,c); }
inline s_string	String::getFirstWord( char c ) const { return getFirstWord(_data,c); }
inline s_string	String::getMiddleWord( char c, int /*decal*/ ) const { return getMiddleWord(_data,c); }
inline s_string	String::getLastWord( char c ) const { return getLastWord(_data,c); }

// Function to convert uchar to string of length 2
inline char* String::Char2Hex( const uchar ch, char* szHex )
{
	int byte[2];
	byte[0] = ch/16;
	byte[1] = ch%16;
	for(int i=0; i<2; ++i)
	{
		if(byte[i] >= 0 && byte[i] <= 9)
			szHex[i] = '0' + byte[i];
		else
			szHex[i] = 'A' + byte[i] - 10;
	}
	szHex[2] = 0;

	return szHex;
}

// Function to convert string of length 2 to uchar
inline uchar String::Hex2Char( const char* szHex )
{
	char rch = 0;
	for(int i=0; i<2; ++i)
	{
		if(*(szHex + i) >='0' && *(szHex + i) <= '9')
			rch = (rch << 4) + (*(szHex + i) - '0');
		else if(*(szHex + i) >='A' && *(szHex + i) <= 'F')
			rch = (rch << 4) + (*(szHex + i) - 'A' + 10);
		else
			break;
	}

	return rch;
}

inline String& String::operator << ( const char* c )
{
	*this += c;
	return( *this );
}
inline String& String::operator << ( const String& c )
{
	*this += c;
	return( *this );
}
inline String& String::operator << ( char c )
{
	*this += c;
	return( *this );
}
inline String& String::operator << ( bool c )
{
	*this += String((int)(c?1:0));
	return( *this );
}
inline String& String::operator << ( uchar c )
{
	*this += String((int)c);
	return( *this );
}
inline String& String::operator << ( short c )
{
	*this += String((int)c);
	return( *this );
}
inline String& String::operator << ( ushort c )
{
	*this += String((int)c);
	return( *this );
}
inline String& String::operator << ( int c )
{
	*this += String(c);
	return( *this );
}
inline String& String::operator << ( uint c )
{
	*this += String((int)c);
	return( *this );
}
inline String& String::operator << ( float c )
{
	*this += String(c);
	return( *this );
}
inline String& String::operator << ( long c )
{
	*this += String((int)c);
	return( *this );
}
inline String& String::operator << ( ulong c )
{
	*this += String((int)c);
	return( *this );
}
inline String& String::operator << ( long double c )
{
	*this += String((float)c);
	return( *this );
}

String String::bin2String(const uchar *data, int size)
{
	String r;
	for(int i=0; i < size; ++i)
	{
		r << Char2Hex( data[i] ) << ' ';
		//r << format("%02X ", data[i]);
	}
	return r;
}

int String::string2Bin(const String& in, uchar *Bin, int *BinLen)
{
	//BinLen input is max. length
	String str(in), str2;
	int Byte2;
	char ByteString[3];
	int i;
	int DestPos;
	int MaxBinLen;

	if( !Bin || !BinLen )
	{
		return 0;
	}

	/** remove space */
	str.replace(" ", "");

	MaxBinLen = *BinLen;
	ByteString[2] = 0;
	*BinLen = 0;

	if( MaxBinLen <= 0 )
	{
		return 0;
	}
	if( str.size() <= 0)
	{
		return 0;
	}
	if(str.size() > (MaxBinLen * 2))
	{
		return 0;
	}
	if((str.size() % 2) != 0)
	{
		return 0;
	}

	str = str.toupper();

	DestPos = 0;

	for(i=0; i< str.size(); i += 2 )
	{
		ByteString[0] = str[i];
		ByteString[1] = str[i+1];
		sscanf(ByteString, "%02X", &Byte2);
		Bin[DestPos++] = Byte2;
	}

	// cross check
	str2 = String::bin2String( Bin, DestPos );

	/** remove space */
	str2.replace(" ", "");

	if ( str2.compareNoCase(str) != 0 )
	{
		// error: String is invalid
		*BinLen = 0;
		return 0;
	}

	*BinLen = DestPos;

	return 1;
}
