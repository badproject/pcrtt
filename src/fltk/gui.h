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
#if !defined(PCRTT_SRC_FLTK_GUI_INCLUDE_H_)
#define PCRTT_SRC_FLTK_GUI_INCLUDE_H_

#if defined(__FLTK__)

#include <FL/Fl.H>

extern ushort gui_verbose;

int gui_create_fltk(int argc, char* argv[]);
void destroy_gui();
void debug_gui(const char* fmt,...);

/** for callback */
void do_open_chain_dir();
void do_calc_chain_start_btn();
void do_calc_chain_pause_btn();
void do_calc_chain_stop_btn();
void do_open_chain_file();
void do_open_rainbow_table_rbt_dir();
void do_add_rainbow_table_merge_btn();
void do_add_rainbow_table_stop_btn();
void do_open_ts_file();
void do_calc_search_crypt8_start_btn();
void do_calc_search_crypt8_stop_btn();
void do_open_search_cw_rbt_dir();
void do_calc_search_cw_start_btn();
void do_calc_search_cw_stop_btn();

// must be thread safe
void SetButtonStateCalcChainStart();
void SetButtonStateCalcChainPause();
void SetButtonStateCalcChainStop();
void SetButtonStateaddToRbtStart();
void SetButtonStateAddToRbtStop();

void UpdateStatistic(const Stats_t* stats, const int& type);

/** string from edit */
String getCalcChainChainDir(void);
String getAddToRbtSrc(void);
String getAddToRbtRbtDir(void);
String getSearchCrypt8InTsSrc(void);
String getSearchCrypt8InTsPid(void);
String getSearchCrypt8InTsPayloadSize(void);
String getSearchCrypt8InTsFileLimit(void);
String getSearchCwRbtDir(void);
String getSearchCwCrypt8(void);

#endif // __FLTK__

#endif // CSA_COLIBRI_SRC_FLTK_GUI_INCLUDE_H_
