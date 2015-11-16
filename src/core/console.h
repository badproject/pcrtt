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
#if !defined( PCRTT_SRC_CORE_CONSOLE_HEADER_H_ )
#define PCRTT_SRC_CORE_CONSOLE_HEADER_H_

#if defined(__CONSOLE__)

#ifdef __cplusplus

#define output_gui(...) _debug_gui(__FILE__, __LINE__, __VA_ARGS__)

void _debug_gui(const char* file, const int& line, const char* fmt,...);
void clear_output_gui();

// todo: plugin / template class ....

int gui_create_console(int argc, char* argv[]);
void destroy_gui();

// must be thread safe
void SetButtonStateCalcChainStart();
void SetButtonStateCalcChainPause();
void SetButtonStateCalcChainStop();

void SetButtonStateaddToRbtStart();
void SetButtonStateAddToRbtStop();

void SetButtonStateSearchCrypt8InTsStart();
void SetButtonStateSearchCrypt8InTsStop();

void SetButtonStateSearchCwStart();
void SetButtonStateSearchCwStop();

void UpdateStatistic(const Stats_t* stats, const int& type);

/** string from edit */
PCRTT::String getCalcChainChainDir(void);
PCRTT::String getAddToRbtSrc(void);
PCRTT::String getAddToRbtRbtDir(void);
PCRTT::String getSearchCrypt8InTsSrc(void);
PCRTT::String getSearchCrypt8InTsPid(void);
PCRTT::String getSearchCrypt8InTsPayloadSize(void);
PCRTT::String getSearchCrypt8InTsFileLimit(void);
PCRTT::String getSearchCwRbtDir(void);
PCRTT::String getSearchCwCrypt8(void);

/** to edit */
void setSearchCwCw(const PCRTT::String& str);

void setCalcChainChainDir(const PCRTT::String&);
void setAddToRbtSrc(const PCRTT::String&);
void setAddToRbtRbtDir(const PCRTT::String&);
void setSearchCrypt8InTsSrc(const PCRTT::String&);
void setSearchCrypt8InTsPid(const PCRTT::String&);
void setSearchCrypt8InTsPayloadSize(const PCRTT::String&);
void setSearchCrypt8InTsFileLimit(const PCRTT::String&);
void setSearchCwRbtDir(const PCRTT::String&);
void setSearchCwCrypt8(const PCRTT::String&);

#endif // __cplusplus

#endif // __CONSOLE__

#endif // PCRTT_SRC_CORE_CONSOLE_HEADER_H_
