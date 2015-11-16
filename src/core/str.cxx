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

uint String::npos = 99778833;

String::String() : 
	_length(0),
	_capacity(1),
	_data(new char[_capacity])
{
	_data[0]='\0';
}

String::String(const char *s) : 
	_length(0), _capacity(0), _data(0)
{
	append(s);
}

String::String(const String &s) : 
	_length(0), _capacity(0), _data(0)
{
	append(s);
}

String::String(char c) : 
	_length(0), _capacity(0), _data(0)
{
	char buf[128]={0};
	snprintf(buf,sizeof(buf),"%c",c);
	append( buf );
}

String::String(int i) : 
	_length(0), _capacity(0), _data(0)
{
	char buf[128]={0};
	snprintf(buf,sizeof(buf),"%d",i);
	append( buf );
}

String::String(float f) :
	_length(0), _capacity(0), _data(0)
{
	char buf[128]={0};
	snprintf(buf,sizeof(buf),"%g",f);
	append( buf );
}

String::~String()
{
	KILLARRAY( _data );
}

String String::basename( const char* str )
{
	String r(str);

	const char * file = strrchr( str, dirsep() );

	if ( file != 0 )
		file++;

	return (r=file);
}

void String::updatebuffer(int len)
{
	if(len >= _capacity)
	{
		_capacity = len * 2 + 1;
		char *new_data = new char[_capacity];
		memset(new_data,0,_capacity);
		memcpy(new_data,_data,_length);
		KILLARRAY(_data);
		_data = new_data;
	}
}

void String::reserve(int size)
{
	updatebuffer(size);
}

String String::subString(int begin, int length) const
{
	if (length <= 0) return String("");

	String o;
	o.reserve(length + 1);

	for (int i=0; i<length; ++i)
		o._data[i] = _data[i+begin];

	o._length = length;
	o._data[length] = 0;

	return o;
}

void String::insert(int start, int len, const char* str)
{
	assert( start >= 0 && str );

	int dwParamLen = len;
	int dwNewLen = _length + dwParamLen;

	String tempStr( *this );

	updatebuffer( dwNewLen + 1 );

	memcpy( _data, tempStr, sizeof(char) * start);
	memcpy( _data + start, str, sizeof(char) * dwParamLen);
	memcpy( _data + start + dwParamLen, tempStr._data + start,
		sizeof(char) * ( tempStr.size() - start ) );

	_data[ dwNewLen ] = '\0';
	_length = dwNewLen;
}

void String::insert(int start, const char* str)
{
	assert( start >= 0 && str );
	if ( start > _length )
	{
		return;
	}

	int dwParamLen = strlen( str );
	int dwNewLen = _length + dwParamLen;

	String tempStr( *this );

	updatebuffer( dwNewLen + 1 );

	memcpy( _data, tempStr, sizeof(char) * start);
	memcpy( _data + start, str, sizeof(char) * dwParamLen);
	memcpy( _data + start + dwParamLen, tempStr._data + start,  sizeof(char) * ( tempStr.size() - start ) );

	_data[ dwNewLen ] = '\0';
	_length = dwNewLen;
}

String String::copy( int start, int len )
{
	if( !len )
		return String("");
	assert( start < _length );
	if( len > ( size() - start ) )
		len = size() - start;

	char* tmp = new char[ size() ];

	memset(tmp, 0, size());
	memcpy( tmp, &_data[0] + start, sizeof( char ) * len );

	String str = tmp;

	KILLARRAY(tmp);

	return str;
}

void String::remove( int start, int len )
{
	#define is_inside(a)	((a>=start)&&(a<start+len))
	int j=0;
	char buf[4096]={0};

	for(int i=0;i<size();++i)
	{
		if( !is_inside(i) && i < 4096 ) 
			buf[j++] = _data[i];
	}

	buf[j] = 0;

	*this = String( (const char*)buf );

	#undef is_inside
}

int String::operator==(const char *s) const
{
	if(_data == 0) return 0;
	return !strcmp(_data,s);
}

int String::operator==(const String &s) const
{
	if(_data == 0) return 0;
	return !strcmp(_data,s._data);
}

int String::operator!=(const char *s) const
{
	return !(*this == s);
}

int String::operator!=(const String &s) const
{
	return !(*this == s);
}

int String::operator<(const char *s) const
{
	return strcmp(_data,s) < 0;
}

int String::operator<(const String &s) const
{
	return strcmp(_data,s._data) < 0;
}

int String::operator>(const char *s) const
{
	return strcmp(_data,s) > 0;
}

int String::operator>(const String &s) const
{
	return strcmp(_data,s._data) > 0;
}

String &String::operator=(const char *s)
{
	_length = 0;
	append(s);
	return *this;
}

String &String::operator=(const String &s)
{
	_length = 0;
	append(s);

	return *this;
}

String String::operator+(char c) const
{
	String ret = *this;
	ret.append(c);
	return ret;
}

String String::operator+(const char *s) const
{
	String ret = *this;
	ret.append(s);
	return ret;
}

String String::operator+(const String &s) const
{
	String ret = *this;
	ret.append(s);
	return ret;
}

String &String::operator+=(char c)
{
	append(c);
	return *this;
}

String &String::operator+=(const char *s)
{
	append(s);
	return *this;
}

String &String::operator+=(const String &s)
{
	append(s);
	return *this;
}

void String::clear()
{
	_length = 0;
	if(_data) 
		_data[_length] = '\0';
}

void String::append(char c)
{
	append(&c,1);
}

void String::append(const char *s)
{
	if(s == 0) return;
	append(s,(int)strlen(s));
}

void String::append(const String &s)
{
	append(s._data,s._length);
}

void String::append(const char *s,int s_length)
{
	int new_length = _length + s_length;
	if(_data == 0)
	{
		_capacity = new_length * 2 + 1;
		_data = new char[_capacity];
		memcpy(_data,s,s_length);
		memset(_data+s_length,0,_capacity-s_length);
	}
	else if(new_length >= _capacity)
	{
		_capacity = new_length * 2 + 1;
		char *new_data = new char[_capacity];
		memset(new_data,0,_capacity);
		memcpy(new_data,_data,_length);
		memcpy(new_data + _length,s,s_length);
		KILLARRAY(_data); // delete [] _data;
		_data = new_data;
	}
	else {
		memcpy(_data + _length,s,s_length);
	}
	_length = new_length;
	_data[_length] = '\0';
}

String String::tolower(const char* data)
{
	char buf[256]={0};
	strncpy(buf,data,sizeof(buf));
	String r(strlwr(buf) );
	return r;
}

char String::tolower(const char c)
{
#if defined(HAVE_TOLOWER)
	return ::tolower(c);
#else
	return isupper(c) ? (c) - 'A' + 'a' : c;
#endif
}

String String::tolower() const
{
	String r(_data);
	r = strlwr( (char*)r._data );
	return r;
}

String String::toupper() const
{
	String r(*this);
	uchar *ucs = (uchar *)r._data;
	for ( ; *ucs != '\0'; ucs++)
	{
		*ucs = ::toupper(*ucs);
	}
	return r;
}

char *String::strlwr(char *s)
{
#if defined(HAVE_STRLWR)
	return ::strlwr(s);
#else
	uchar *ucs = (uchar *)s;
	for ( ; *ucs != '\0'; ucs++)
	{
		*ucs = String::tolower(*ucs);
	}
	return s;
#endif
}


String String::substr( int pos, int n ) const
{
	if ( n == (int)npos )
		n = size();

	String r = subString( pos, n );

	return r;
}

int String::find(const char* data, const char* search) const
{
	char* str = strstr( (char*)data, search );
	if ( str )
	{
		return ( str - data );
	}
	return -1;
}

int String::find( const char* search ) const
{
	char* str = strstr( _data, search );
	if ( str )
	{
		return ( str - _data );
	}
	return -1;
}

int String::find( const char c ) const
{
	char *s = _data;
	while(*s)
	{
		if ( *s == c )
			return ( s - _data );
		(void)*s++;
	}
	return -1;
}

void String::replace(const char* oldStr, const char* newStr)
{
	for(;;)
	{
		int start = find(_data, oldStr);
		if(start==-1) return;
		int end = start + strlen(oldStr);
		if(!end) return;
		int len = end - start;
		if(!len) return;

		remove( start, len );
		if ( newStr ) insert( start, newStr );
	}
}

void String::code_optimizer( String& code, const int& maxPass )
{
	char* s = code._data;
	char* d = code._data;
	int retry = 0;
	int pass = maxPass;

	// minimum pass for efficient result
	if ( pass <= 1 )
		pass = 2;

	// start optimizer

	// check for (space,tabs) before/after a character
	#define CHK_C_BA(xx)					\
		/* space before xx */				\
		if((*s==' ')&&(*(s+1)==xx))			\
		{ s++; *d++ = *s++ = xx; }			\
		/* space after xx */				\
		else if((*s==xx)&&(*(s+1)==' '))	\
		{ *d++ = *s++ = xx; s++; }			\
		/* tabs before xx */				\
		else if((*s=='\t')&&(*(s+1)== xx))	\
		{ s++; *d++ = *s++ = xx; }			\
		/* tabs after xx */					\
		else if((*s==xx)&&(*(s+1)=='\t'))	\
		{ *d++ = *s++ = xx; s++; }

RetryOpcode:

	while( *s )
	{
		// first pass we remove only
		if ( retry == 0 )
		{
			// comment '//'
			if( *s == '/' && *(s + 1) == '/' )
			{
				while(*s && *s != '\n') s++;
				while(*s && *s == '\n') s++; *d++ = ' ';
			}
			// start comment '/*'
			else if ( *s == '/' && *(s + 1) == '*' )
			{
				while( *s && (*s != '*' || *(s + 1) != '/') ) s++; s += 2;
				while( *s && *s == '\n' ) s++; *d++ = ' ';
			}
			// simple tabs
			else if( *s == '\t' ) { *d++ = *s++ = ' '; }
			// double ;;
			else if( *s == ';' && *(s + 1) == ';' ) { *d++ = *s++; s++; }
			/** CR / LF + '#' special case for shader ... */
			else if( (*s == '\r' || *s == '\n') && *(s+1) == '#' )
			{
				do { 
					*d++ = *s++;
				} while( *s && *s != '\n' ) ;
				*d++ = *s++;
			}
			// CR LF
			else if( (*s == '\r' || *s == '\n') && *(s+1) != '#' ) { s++; }
			/** special case for shader */
			else if( *s == '#' )
			{
				*d++ = '\n';
				do { 
					*d++ = *s++;
				} while( *s && *s != '\n' ) ;
				*d++ = *s++;
			}
			else
				*d++ = *s++;

			continue;
		}
		// double space
		if ( *s == ' ' && *(s + 1) == ' ' ) { s++; *d++ = *s++; }
		else CHK_C_BA(':') else CHK_C_BA('=') else CHK_C_BA('.')
		else CHK_C_BA(';') else CHK_C_BA(',') else CHK_C_BA('&')
		else CHK_C_BA('!') else CHK_C_BA('+') else CHK_C_BA('-')
		else CHK_C_BA('*') else CHK_C_BA('/') else CHK_C_BA('(')
		else CHK_C_BA(')') else CHK_C_BA('{') else CHK_C_BA('}')
		else CHK_C_BA('[') else CHK_C_BA(']')
		// check for small error
		// '};' or ';{'
		else if ( *s == '}' && *(s + 1) == ';' ) { *d++ = *s++; s++; }
		else if ( *s == ';' && *(s + 1) == '{' ) { s++; *d++ = *s++; }
		// none found, copy
		else
			*d++ = *s++;
	}

	*d = '\0';

	code._length = strlen( code._data );

	// more pass == less space
	if ( retry < pass )
	{
		retry++;

		s = code._data;
		d = code._data;

		goto RetryOpcode;
	}
}

string_v String::explode( const String& buffer, char c ) 
{
	string_v result;

	if ( !buffer.size() )
		return result;

	String str( "dummy" );

	uint Len = buffer.size();
	uint pos = 0;

	while ( str.size() )
	{
		str = "";

		do {
			if (pos>=Len) break;
			if ( buffer[pos] != c )
				str += buffer[pos];
			++pos;
		}
		while(buffer[pos] != c);

		++pos;

		if ( str.size() )
			result.push_back( str );
	}

	return result;
}

String String::getFirstWord( const String& buffer, char c )
{
	if ( !buffer.size() )
		return "empty";

	string_v mar = explode( buffer, c );

	return mar[0];
}

String String::getMiddleWord( const String& buffer, char c )
{
	String out;

	if ( !buffer.size() )
		return out;

	string_v mar = explode( buffer, c );

	if ( !mar.size() )
		return out;

	int size = int( (mar.size() / 2) /*+ decal*/ );

	out = mar[(size > mar.size()) ? (mar.size() / 2) : size];

	return out;
}

String String::getLastWord( const String& buffer, char c )
{
	String out;

	if ( !buffer.size() )
		return out;

	string_v mar = explode( buffer, c );

	if ( !mar.size() )
		return out;

	int size = mar.size();

	out = mar[size - 1];

	return out;
}

int String::compareNoCase(const String& a, const String& b)
{
#if defined(HAVE_STRNCASECMP)
	return strncasecmp(a,b,a.size());
#else
	int sz = a.size();

	if (b.size() != sz)
		return 0;

	for (int i = 0; i < sz; ++i)
		if (tolower(a[i]) != tolower(b[i]))
			return 0;
	return 1;
#endif // 0
}

int String::compareNoCase(const String& o)
{ return compareNoCase(*this, o); }

String String::Char2Hex( const uchar ch )
{
	String r("FF");
	Char2Hex( ch, r._data );
	return r;
}

uchar String::Hex2Char()
{
	return Hex2Char( c_str() );
}

int String::ReverseFind( char c ) const
{
	char *s = &_data[0] + size() - 1;

	while( *s )
	{
		if ( *s == c )
			return -( _data - s );
		(void)*s--;
	}

	return -1;
}

String String::Left( int length ) const
{
	if ( length > size() ) length = size();

	return subString( 0, length );
}

String String::Right( int length ) const
{
	if ( length > size() ) length = size();

	return subString( size() - length, length );
}

void String::TrimRight()
{
	char* s = _data + size() - 1;

	if ( *s == ' ' || *s == '\n' || *s == '\t' )
	{
		*s = '\0';
		_length = strlen( _data );
		// search again
		TrimRight();
	}
}

void String::TrimLeft()
{
	char *s = _data;
	char *d = _data;

	while( *s )
	{
		if ( *s == ' ' || *s == '\n' || *s == '\t' )
		{
			(void)*s++;
		}
		else
			*d++ = *s++;
	}

	*d = '\0';

	_length = strlen( _data );
}

int String::find_last_of(char c) const
{
	int i=0;
	int pos = -1;
	char *s = _data;

	while( *s )
	{
		if (*s==c) pos=i;
		i++;
		(void)*s++;
	}
	return pos;
}

String String::getFileExtension( const String& filename )
{
	int pos = filename.find_last_of( '.' );

	if ( pos != -1 )
		return filename.substr( pos + 1, filename.size() );

	return "";
}

String String::getPath( const String& filename )
{
#if defined(__WIN32__)
	int pos = filename.find_last_of( '\\' );
#else
	int pos = filename.find_last_of( '/' );
#endif

	if ( pos != -1 )
		return filename.substr( 0, pos );

	return "";
}

// ------ global string function ----------------------------------------------
String format_str(const char* fmt, ... )
{
	char txt[256]={0};
	va_list argptr;
	va_start( argptr, fmt );
	vsnprintf( txt, sizeof(txt), fmt, argptr );
	va_end( argptr );

	return String( txt );
}

String format(const char* fmt, ... )
{
	char txt[256]={0};
	va_list argptr;
	va_start( argptr, fmt );
	vsnprintf( txt, sizeof(txt), fmt, argptr );
	va_end( argptr );

	return String( txt );
}

String replace( String buffer, const char* oldValue, const char* newValue )
{
	String result;

	String s = String( buffer.c_str() );
	s.replace( oldValue, newValue );

	result = s.ascii();

	return result;
}

String replace(const String& data, char k, char r)
{
	char str[256]={0};
	strncpy(str,data.c_str(),sizeof(str));
	replace_str(str, k, r );
	return String(str);
}

void replace_str( char* str, char old, char n )
{
	char* ptr = str;

	while ( *ptr++ )
		if ( *ptr == old )
			*ptr = n; 
}

string_v Explode(const String& buffer, char lChar)
{
	string_v	result;

	if ( !buffer.size() ) return result;

	String Data(buffer), tmpExplode("dummy start");

	uint Len = Data.size();
	uint	lPos = 0;

	#define _LOOP(_Dest_, _Char_) \
		{ do { if (lPos>=Len) break; _Dest_ += Data[lPos]; lPos++; } while(Data[lPos] != _Char_); lPos++; }

	while (tmpExplode.size())
	{
		tmpExplode = "";

		_LOOP(tmpExplode, lChar);

		if (tmpExplode.size()) {
			result.push_back( tmpExplode );
		}
	}

	#undef _LOOP

	return result;

}

String GetFirstWord(const String& buffer, char lChar)
{
	if (!buffer.size()) return "Empty Buffer";

	string_v mar = Explode(buffer, lChar);

	return mar[0];
}

String GetMiddleWord(const String& buffer, char lChar, int Decal)
{
	if (!buffer.size()) return "Empty Buffer";

	string_v mar= Explode(buffer, lChar);

	if (!mar.size()) return "Bad Buffer";

	int size	= int((mar.size() / 2) + Decal);

	return mar[(size > mar.size()) ? (mar.size() / 2) : size];
}

String GetLastWord(const String& buffer, char lChar)
{
	if (!buffer.size()) return "Empty Buffer";

	string_v mar = Explode(buffer, lChar);

	int size = mar.size();

	return mar[size - 1];
}

} // end of namespace PCRTT
