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
#include "../include.h"

#include "gui.h"

/** function to manage callback in fltk world */

#if defined(__FLTK__)

#include <FL/fl_ask.H> 
#include <FL/Fl_File_Chooser.H> 
#include <FL/Fl_Text_Buffer.H>

#if defined(__FLTK_TAB__)
#include "gui_fltk_tab.h"
#else
#include "gui_fltk_default.h"
#endif // __FLTK_TAB__

static Fl_Text_Buffer *output_buffer = 0;
static String output_buffer_str;
ushort gui_verbose = 0;

void destroy_gui()
{
	
}

int gui_create_fltk(int argc, char* argv[])
{
	const String title(getTitle());
	Fl_Double_Window* win;
	win = create_fltk_gui();
	
	if (!win)
	{
		TRACE_FATAL("Can't create fltk window.");
		exit(0);
		return 0;
	}
	/** attach a buffer to text display */
	output_buffer = new Fl_Text_Buffer();
	output_console->buffer(output_buffer); 
	output_buffer->text( title );
	output_buffer_str.clear();

#if defined(__FLTK_TAB__)
	about_label->copy_label( title );
	about_logo->copy_label("");
#endif // __FLTK_TAB__

	win->show();

	return(Fl::run());
}

void debug_gui(const char* fmt,...)
{
	char str[256]={0};
	va_list argList;

	va_start( argList, fmt );
	vsnprintf( str, sizeof(str), fmt, argList );
	va_end( argList );

	if ( gui_verbose )
		::printf( str );
	// todo: verbose to gui
}

void do_open_chain_dir()
{
	char* r = fl_dir_chooser("Select Chain Dir","",0);
	if ( r )
		chainDir_Filename->static_value(r,strlen(r));
}

void do_calc_chain_start_btn()
{
	
}

void do_calc_chain_pause_btn()
{
	
}

void do_calc_chain_stop_btn()
{
	
}

void do_open_chain_file()
{
	const char* pattern = "Chain Files (*.{rb})";
	char* r = fl_file_chooser("Select chain file",pattern,"",0);
	if ( r )
		chainFile_Filename->static_value(r,strlen(r));
}

void do_open_rainbow_table_rbt_dir()
{
	char* r = fl_dir_chooser("Select RBT Dir","",0);
	if ( r )
		rbtDir_Filename->static_value(r,strlen(r));
}

void do_add_rainbow_table_merge_btn()
{
	
}

void do_add_rainbow_table_stop_btn()
{
	
}

void do_open_ts_file()
{
	const char* pattern = "TS Files (*.{ts})";
	char* r = fl_file_chooser("Select TS file",pattern,"",0);
	if ( r )
		searchCrypt8_Filename->static_value(r,strlen(r));
}

void do_calc_search_crypt8_start_btn()
{
	
}

void do_calc_search_crypt8_stop_btn()
{
	
}

void do_open_search_cw_rbt_dir()
{
	char* r = fl_dir_chooser("Select RBT Dir","",0);
	if ( r )
		searchCW_RBTDir->static_value(r,strlen(r));
}

void do_calc_search_cw_start_btn()
{
	
}

void do_calc_search_cw_stop_btn()
{
	
}


static void enable_all_button()
{
	// calc chain
	//ENABLE_BUTTON( calcChain_chainDir_Open )
	ENABLE_BUTTON( calcChain_Start )
	ENABLE_BUTTON( calcChain_Pause )
	ENABLE_BUTTON( calcChain_Stop )
	// add to rainbow table
	//ENABLE_BUTTON( addRainbowTable_chainFile_Open )
	//ENABLE_BUTTON( addRainbowTable_rbtDir_Open )
	ENABLE_BUTTON( addRainbowTable_Merge )
	ENABLE_BUTTON( addRainbowTable_Stop )
	// search Crypt8
	//ENABLE_BUTTON( searchCrypt8_rbtDir_Open )
	ENABLE_BUTTON( searchCrypt8_Start )
	ENABLE_BUTTON( searchCrypt8_Stop )
	// search CW
	//ENABLE_BUTTON( searchCW_Open )
	ENABLE_BUTTON( searchCW_Start )
	ENABLE_BUTTON( searchCW_Stop )
}

static void disable_all_button()
{
	// calc chain
	//DISABLE_BUTTON( calcChain_chainDir_Open )
	DISABLE_BUTTON( calcChain_Start )
	DISABLE_BUTTON( calcChain_Pause )
	DISABLE_BUTTON( calcChain_Stop )
	// add to rainbow table
	//DISABLE_BUTTON( addRainbowTable_chainFile_Open )
	//DISABLE_BUTTON( addRainbowTable_rbtDir_Open )
	DISABLE_BUTTON( addRainbowTable_Merge )
	DISABLE_BUTTON( addRainbowTable_Stop )
	// search Crypt8
	//DISABLE_BUTTON( searchCrypt8_rbtDir_Open )
	DISABLE_BUTTON( searchCrypt8_Start )
	DISABLE_BUTTON( searchCrypt8_Stop )
	// search CW
	//DISABLE_BUTTON( searchCW_Open )
	DISABLE_BUTTON( searchCW_Start )
	DISABLE_BUTTON( searchCW_Stop )
}

void SetButtonStateCalcChainStart()
{
	disable_all_button();
	ENABLE_BUTTON( calcChain_Pause )
	ENABLE_BUTTON( calcChain_Stop )
}

void SetButtonStateCalcChainPause()
{
	disable_all_button();
	ENABLE_BUTTON( calcChain_Start )
	ENABLE_BUTTON( calcChain_Stop )
}

void SetButtonStateCalcChainStop()
{
	disable_all_button();
	ENABLE_BUTTON( searchCW_Start )
	ENABLE_BUTTON( searchCrypt8_Start )
	ENABLE_BUTTON( addRainbowTable_Merge )
	ENABLE_BUTTON( calcChain_Start )
}

void SetButtonStateaddToRbtStart()
{
	disable_all_button();
	ENABLE_BUTTON( addRainbowTable_Stop )
}

void SetButtonStateAddToRbtStop()
{
	enable_all_button();
	DISABLE_BUTTON( calcChain_Pause )
	DISABLE_BUTTON( calcChain_Stop )
	DISABLE_BUTTON( addRainbowTable_Stop )
	DISABLE_BUTTON( searchCrypt8_Stop )
	DISABLE_BUTTON( searchCW_Stop )
}

#endif // __FLTK__
