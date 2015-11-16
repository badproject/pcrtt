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
#if !defined( PCRTT_SRC_STR_HEADER_H_ )
#define PCRTT_SRC_STR_HEADER_H_

namespace PCRTT {

class String;

typedef Vector<String> string_v;
typedef String s_string;

/**
 * a string class
 */
//! 
class String
{
public:
	String();
	String(const char *s);
	String(const String &s);
	explicit String(char c);
	explicit String(int i);
	explicit String(float f);
	virtual ~String();

	static uint npos;

	void reserve(int size);

	int operator==(const char *s) const;
	int operator==(const String &s) const;
	int operator!=(const char *s) const;
	int operator!=(const String &s) const;
	int operator<(const char *s) const;
	int operator<(const String &s) const;
	int operator>(const char *s) const;
	int operator>(const String &s) const;

	String &operator=(const char *s);
	String &operator=(const String &s);

	String operator+(char c) const;
	String operator+(const char *s) const;
	String operator+(const String &s) const;
	String &operator+=(char c);
	String &operator+=(const char *s);
	String &operator+=(const String &s);

	inline const char *ascii();
	inline const char *ascii() const;

	/** complient with std::string */
	inline const char *c_str();
	inline const char *c_str() const;
	inline int length() const;
	String substr ( int pos = 0, int n = npos ) const;

	inline operator const char*();
	inline operator const char*() const;
	
	inline char &operator[](int index);
	inline char operator[](int index) const;

	inline int size() const;

	static String basename( const char* name );

	int find( const char* data, const char* search ) const;
	int find( const char* search ) const;
	int find( char c ) const;
	
	int find_last_of(char c) const;

	/**
	 * The index of the last character in this CString object that matches
	 * the requested character; –1 if the character is not found.
	 */
	int ReverseFind( char c ) const;

	/** A String object containing a copy of the specified range of characters.
	 * Note that the returned String object may be empty.
	 */
	String Left( int nCount ) const;
	String Right( int length ) const;

	/** The target characters to be trimmed. */
	void TrimRight();
	void TrimLeft();

	String tolower() const;
	String toupper() const;

	static String tolower(const char* data);
	static char tolower(const char c);
	static char* strlwr(char *s);

	void clear();

	void append( char c );
	void append( const char *s );
	void append( const String &s );
	void append(const char *s,int s_length);

	String subString(int begin, int length) const;

	String copy( int start, int len );
	void remove( int pos, int len );
	void insert(int pos, const char* str);
	void insert(int start, int len, const char* str);

	void replace(const char* oldStr, const char* newStr);

	/** handy stuff */

	//
	static String getFileExtension( const String& filename );
	static String getPath( const String& filename );

	// special for coder
	static void code_optimizer( String& code, const int& maxPass = 5 );
	inline void code_optimizer(){ code_optimizer( *this ); }
	inline void removeComment() { code_optimizer( *this ); }

	static string_v	explode( const String& buffer, char c = ' ' ) ;
	static s_string	getFirstWord( const s_string& buffer, char c = ' ' ) ;
	static s_string	getMiddleWord( const s_string& buffer, char c = ' ') ;
	static s_string	getLastWord( const s_string& buffer, char c = ' ' ) ;

	inline string_v	explode( char c = ' ' )  const;
	inline s_string	getFirstWord( char c = ' ' ) const;
	inline s_string	getMiddleWord( char c = ' ', int decal=0 ) const;
	inline s_string	getLastWord( char c = ' ' ) const;

	static inline char* Char2Hex( const uchar ch, char* szHex );
	static inline uchar Hex2Char( const char* szHex );
	
	static String Char2Hex( const uchar ch );
	uchar Hex2Char();
	
	static inline String bin2String(const uchar *data, int size);
	static inline int string2Bin(const String& str, uchar *Bin, int *BinLen);

	static int compareNoCase(const String& a, const String& b);
	int compareNoCase(const String& o);

	/** operator */
	inline String		&operator << ( bool d );
	inline String		&operator << ( char d );
	inline String		&operator << ( uchar d );
	inline String		&operator << ( short d );
	inline String		&operator << ( ushort d );
	inline String		&operator << ( int d );
	inline String		&operator << ( uint d );
	inline String		&operator << ( float d );
	inline String		&operator << ( long d );
	inline String		&operator << ( ulong d );
	inline String		&operator << ( long double d );
	inline String		&operator << ( const char *d );
	inline String		&operator << ( const String &d );

protected:
	/** direct access to data (take care) */
	char*	data() {return _data;}
	void	setLen(int l) { _length = l; }

private:

	void updatebuffer(int len);

	int _length;
	int _capacity;
	char *_data;
}; // end of class String

#include "str.inl"

String format_str(const char* fmt, ... );
String format(const char* fmt, ... );

String replace( String buffer, const char* oldValue, const char* newValue );
String replace(const String& data, char k, char r);

void replace_str( char* str, char old, char n );

string_v Explode(const String& buffer, char lChar);
String GetFirstWord(const String& buffer, char lChar);
String GetMiddleWord(const String& buffer, char lChar, int Decal);
String GetLastWord(const String& buffer, char lChar);

} // end of namespace PCRTT

#endif	// PCRTT_SRC_STR_HEADER_H_
