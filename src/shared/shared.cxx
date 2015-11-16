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
#include <pcrtt.h>
#include <fstream>	// std::cout
#include <iomanip>	// std::setw

#include <shared/searchcw.h>

extern "C" {
#include <dvbcsa/dvbcsa.h>
#include <dvbcsa/dvbcsa_pv.h>
}

// prefix is 22 bit
#if ( MAX_ELEMENTS_PER_CHAIN_FILE > 0x4000000 )
#error "MAX_ELEMENTS_PER_CHAIN_FILE > 0x4000000 => will not " \
		"match prefix len of 22 bits"
#endif

namespace PCRTT {
namespace Shared {

/** ---------------------------------------------------------------------------
 *  -- local function ---------------------------------------------------------
 *  ---------------------------------------------------------------------------
 */

static void hexToBin(const String& in, uchar *pData, int *pDataLen) 
{
	String str(in);
	int SrcLen;
	uchar *SrcBuffer;

	str.replace(" ", "");
	SrcLen = str.size();

	if ( !pDataLen || !SrcLen || (SrcLen % 2) )
	{
		TRACE_ERROR("SrcLen: %d - (SrcLen % 2): %d", SrcLen, SrcLen % 2);
		return;
	}

	*pDataLen = 0;
	SrcBuffer = (uchar*)str.c_str(); // (SrcLen);

	for(int i=0; i < SrcLen; i += 2 )
	{
		int Temp;
		sscanf((char*)&SrcBuffer[i], "%02X", &Temp);

		pData[(*pDataLen)] = Temp & 0xFF;
		(*pDataLen)++;
	}
}

/** ---------------------------------------------------------------------------
 *  -- Global function --------------------------------------------------------
 *  ---------------------------------------------------------------------------
 */

// void String2Bin(String str, uchar *Bin, int *BinLen);

#define PrintBSafe(_at_,_size_) \
	(((_at_>=_size_) || (_at_<0)) ? (uchar)NULL : theArray[_at_]&0xff)

void logPrintBufferInternal(uchar* data, int size)
{
	//std::ofstream* strm
	//std::cout
	uchar* theArray = data;

	int tSize = size;

	int line_width = std::cout.width();

	if ( line_width == 0 )
		line_width = 16;

	std::cout.width( 0 );

	int indent = std::cout.precision();

	int val_width = ( (std::cout.flags()&std::ios::basefield) == std::ios::hex ) ? 2 : 3;

	int i = 0;
	while ( i < tSize )
	{
		if ( i > 0 )
			std::cout << '\n';
		int j;
		for ( j = 0; j < indent; ++j )
			std::cout << ' ';
		for ( j = 0; j < line_width; ++j )
		{
			if ( j == line_width/2 )
				std::cout << ' ';
			if ( i+j < tSize )
				std::cout << std::setw(val_width) << PrintBSafe(i+j, size);
			else {
				int k;
				for (k = 0; k < val_width; ++k)
					std::cout << ' ';
			}
			std::cout << ' ';
		}
		if ( (std::cout.flags() & std::ios::floatfield) != std::ios::fixed )
		{
			std::cout << "  ";
			for ( j = 0; j < line_width; ++j )
			{
				if (i+j < tSize)
				{
					unsigned val = PrintBSafe(i+j, size);
					if (isprint(val))
						std::cout << (char)val;
					else
						std::cout << '.';
				}
			}
		}
		i += line_width;
	}
}

void printBuffer(const char * fileName, int lineNum, uchar* data, int size)
{

	// print the buffer
	int _indent = 2; // std::cout.precision() + 2;

	std::_Ios_Fmtflags flags = std::cout.flags();

	std::cout << "\tdata = " << size << " octets {\n"
		<< std::hex << std::setfill('0') << std::resetiosflags(std::ios::floatfield)
		<< std::setprecision(_indent) << std::setw(16);

	if ( size <= 32 || (flags&std::ios::floatfield) != std::ios::fixed )
	{
		logPrintBufferInternal(data, size);
		std::cout << '\n';
	}
	else
	{
		logPrintBufferInternal(data, 32 );
		std::cout << '\n' << std::setfill(' ') << std::setw(_indent+4) << "...\n";
	}

	std::cout << std::dec << std::setfill(' ') << std::setw(_indent-1) << "}\n";

	std::cout.flags( flags );
}

void enc23( uchar *out, const uchar *in )
{
	struct dvbcsa_key_s key;
	uchar data[8], Cw8[8];
	int i;

	SearchCw::Cw6ToCw8( Cw8, in );
	dvbcsa_key_set( Cw8, &key );

	memset( data, 0x00, sizeof(data) );

	for(i=0; i<23; ++i)
	{
		dvbcsa_block_encrypt( key.sch, data, data );
	}

	memcpy( out, data, 8 );
}

void ensureDirNameHasBackslashEnding(String& pDirName)
{
	const char* n = pDirName.c_str();
	int size = pDirName.size();

	if ( *n && isdirsep(n[size - 1]) )
		return ;

	pDirName << dirsep();
}

int getStartCw6FromFileName(const String& filename, uchar *Cw6)
{
	int Idx1, Idx2, Idx3, DataLen;
	String Prefix;

	memset( Cw6, 0x00, 6 );

	Idx1 = filename.ReverseFind( '_' );
	Idx2 = filename.ReverseFind( '.' );
	Idx3 = filename.ReverseFind( 'h' );

	if(
		(Idx1 < 0) || (Idx2 < 0) || (Idx3 < 0) || (Idx1 > Idx2) ||
		(Idx3+1 != Idx2)
	)
	{
		TRACE_DEBUG("idx: %d %d %d\n", Idx1, Idx2, Idx3 );
		return 0; // error	
	}

	Prefix = filename.subString( Idx1+1, Idx2 - (Idx1+2) );

	if(Prefix.size() != 6)
	{
		TRACE_DEBUG("Prefix: %s\n", Prefix.c_str() );
		return 0; // error	
	}

	if ( (Prefix.size() % 2) != 0 )
	{
		Prefix += "0";
	}

	hexToBin( Prefix, Cw6, &DataLen );

	if ( (DataLen*2) != Prefix.size() )
	{
		TRACE_DEBUG("Prefix: %s size: %d\n", Prefix.c_str(), Prefix.size() );
		return 0; // error	
	}

	return 1; // ok
}

/*void SendToGui(UINT Message, WPARAM wParam, LPARAM lParam)
{
	if(pTI->fDontSentMsgToGui)
		pTI->fDontSentMsgToGuiOk = true;

	if(!pTI->fDontSentMsgToGui)
		pTI->pView->SendMessage(Message, wParam, lParam);
}
*/
static void saveSettings()
{
	config_t* cfg = load_config();

	if(!cfg)
		return;
	config_setting_t* s;

	config_setting_t *root = config_root_setting(cfg);

	#define SET_CFG(name, value) \
	{ \
		String v(value); \
		s = config_setting_get_member(root, name); \
		if ( !s) s = config_setting_add(root, name, CONFIG_TYPE_STRING ); \
		config_setting_set_string(s, v.c_str() ); \
	}

	SET_CFG("calc_chain_chain_dir", getCalcChainChainDir() );
	SET_CFG("add_to_rbt_src", getAddToRbtSrc() );
	SET_CFG("add_to_rbt_rbt_dir", getAddToRbtRbtDir() );
	SET_CFG("search_crypt8_in_ts_src", getSearchCrypt8InTsSrc() );
	SET_CFG("search_crypt8_in_ts_pid", getSearchCrypt8InTsPid() );
	SET_CFG("search_crypt8_in_ts_payload_size",getSearchCrypt8InTsPayloadSize());
	SET_CFG("search_crypt8_in_ts_file_size", getSearchCrypt8InTsFileLimit() );
	SET_CFG("search_cw_rbt_dir", getSearchCwRbtDir() );
	SET_CFG("search_cw_crypt", getSearchCwCrypt8() );

	#undef SET_CFG
	
	write_config();
}

static void load_Settings()
{
	#define GET_CFG(name,fnc) \
	{ \
		const char *str; \
		if ( config_lookup_string(cfg, name, &str) ) \
		{ \
			String r(str); \
			if ( r.size() ) \
			{ \
				::printf("load_Settings() - " name " : %s\n", str ); \
				fnc( String(str) ); \
			} \
		} else { \
			::printf("No '%s' setting in configuration file.\n",name); \
		} \
	}

	config_t* cfg = load_config();

	if (!cfg)
	{
		printf("Invalid config file.\n");
		return;
	}

	GET_CFG("calc_chain_chain_dir", setCalcChainChainDir );
	GET_CFG("add_to_rbt_src", setAddToRbtSrc );
	GET_CFG("add_to_rbt_rbt_dir", setAddToRbtRbtDir );
	GET_CFG("search_crypt8_in_ts_src", setSearchCrypt8InTsSrc );
	GET_CFG("search_crypt8_in_ts_pid", setSearchCrypt8InTsPid );
	GET_CFG("search_crypt8_in_ts_payload_size",setSearchCrypt8InTsPayloadSize);
	GET_CFG("search_crypt8_in_ts_file_size", setSearchCrypt8InTsFileLimit);
	GET_CFG("search_cw_rbt_dir", setSearchCwRbtDir );
	GET_CFG("search_cw_crypt", setSearchCwCrypt8 );

	#undef GET_CFG
}

void SaveLoadSettings(int save)
{
	if ( 0 == save )
		load_Settings();
	else
		saveSettings();
}

// simple list of string with filename
string_v getStringDirectoryListWithFullPath(const char* path, int subDir)
{
	string_v list;
	DIR *dir;
	struct dirent *ent;

	const char sep = dirsep();

	if ( subDir )
	{
		if ( (dir = (DIR*)opendir(path)) != NULL )
		{
			/* print all the files and directories within directory */
			while ((ent = (dirent*)readdir(dir)) != NULL)
			{
				const String f(ent->d_name);
				const String fullPath( format("%s%c%s", path, sep, f.c_str()));

				if ( (f != ".") && (f != "..") )
				{
					if ( isdirectory(fullPath) )
					{
						string_v l = getStringDirectoryListWithFullPath(
										fullPath,subDir);

						for(int i=0;i<l.size();++i)
							list.push_back( l[i] );
					}
					else
					{
						list.push_back( fullPath );
					}
				}
			}
			closedir (dir);
		}
	}
	else
	{
		if ( (dir = (DIR*)opendir(path)) != NULL )
		{
			/* print all the files and directories within directory */
			while ((ent = (dirent*)readdir(dir)) != NULL)
			{
				const String fullPath( format("%s%c%s", path, sep, ent->d_name ) );
				list.push_back( fullPath );
			}
			closedir (dir);
		}
	}

	return list;
}

} // end of namespace Shared
} // end of namespace PCRTT
