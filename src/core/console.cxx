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

#if defined(__CONSOLE__)

#include "console.h"

/** chain dir option */
static PCRTT::String chain_dir;
static int calc_chain = 0;
static uint worker_type = 

// todo: add a combo box
#if defined(__CUDA__)
 CUDA_WORKER;
#elif defined(__OPENCL__)
 OPENCL_WORKER;
#elif defined(__VULKAN__)
 VULKAN_WORKER;
#endif // defined(__CUDA__)

// todo: add sig handler to manage correct start/resume/stop or use ncurses.

static void help(const char* cmd)
{
	PCRTT::String str;
	std::cout <<
		"[ PCRTT Console ] "<<cmd<<" - (c) NEngine 2015\n\n"	<<
		"Usage: "<<cmd<<" [options]\n"							<<
		"Options:\n"											<<
		"  -? | --help                 Display this help\n";

	::exit(0);
}

static int process_cmd(int argc, char* argv[])
{
	int i = 0;

	#define CHK_OPTIONS(a,act) if (cmd == a) \
		{ act; continue; }
	#define CHK_OPTIONS1(a,act) if (cmd == a) \
		{ arg = argv[i+1]; arg0=arg; act; ++i; continue; }
	#define CHK_OPTIONS2(a,act) if (cmd == a) \
		{if ( i+2 < argc ) { arg0 = argv[i+1]; arg1 = argv[i+2]; act; i+=2;} \
			continue; }

	for ( i=0; i < argc; ++i )
	{
		PCRTT::String cmd(argv[i]), option, arg, arg0, arg1;

		if ( !cmd.size() && (cmd[0] != '-') )
			continue;

		//::printf("parsing: '%s'\n", argv[i] );

		if ( cmd == "--help" ) {help(argv[0]); }
		else if ( cmd == "-?" ) {help(argv[0]);}

		CHK_OPTIONS1("--chain_dir", chain_dir = arg; printf("chain dir: %s\n", chain_dir.c_str() ); );
		CHK_OPTIONS("--calc_chain", calc_chain = 1; printf("calc chain...\n" ); );

		// option
		//CHK_OPTIONS("-v", _debug = 1; REPORT("option -v found\n") );
		//CHK_OPTIONS("--verbose", _debug = 1; REPORT("option -v found\n") );
	}

	return 1;
}
#if defined(__NCURSES__)
#define WIDTH 30
#define HEIGHT 10 

static WINDOW *_win;
int startx = 0;
int starty = 0;

char *choices[] = { 	"Choice 1",
			"Choice 2",
			"Choice 3",
			"Choice 4",
			"Exit",
		  };

int n_choices = sizeof(choices) / sizeof(char *);
#endif // __NCURSES__

static int init_ncurses()
{
#if defined(__NCURSES__)
	// makes the screen
	initscr();
	clear();
	noecho();
	cbreak();	//Line buffering disabled. pass on everything

	startx = (80 - WIDTH) / 2;
	starty = (24 - HEIGHT) / 2;

	attron(A_REVERSE);
	mvprintw(23, 1, "Click on Exit to quit (Works best in a virtual console)");
	refresh();
	attroff(A_REVERSE);

	_win = newwin( HEIGHT, WIDTH, starty, startx );
#endif // __NCURSES__
	return 1;
}

#if defined(__NCURSES__)
void print_menu(WINDOW *menu_win, int highlight)
{
	int x, y, i;	

	x = 2;
	y = 2;
	box(menu_win, 0, 0);
	for(i = 0; i < n_choices; ++i)
	{	if(highlight == i + 1)
		{	wattron(menu_win, A_REVERSE); 
			mvwprintw(menu_win, y, x, "%s", choices[i]);
			wattroff(menu_win, A_REVERSE);
		}
		else
			mvwprintw(menu_win, y, x, "%s", choices[i]);
		++y;
	}
	wrefresh(menu_win);
}

/* Report the choice according to mouse position */
void report_choice(int mouse_x, int mouse_y, int *p_choice)
{	int i,j, choice;

	i = startx + 2;
	j = starty + 3;
	
	for(choice = 0; choice < n_choices; ++choice)
		if(mouse_y == j + choice && mouse_x >= i && mouse_x <= i + 
			strlen(choices[choice]))
		{	if(choice == n_choices - 1)
				*p_choice = -1;		
			else
				*p_choice = choice + 1;	
			break;
		}
}
#endif // #if defined(__NCURSES__)

int gui_create_console(int argc, char* argv[])
{
	if ( !process_cmd(argc,argv) )
		help( argv[0] );

	if (!init_ncurses())
		help(argv[0]);
	int sz = chain_dir.size();

	/** look for calc chain dir */
	if ( calc_chain && !sz )
		help( argv[0] );
	else if ( calc_chain && sz )
	{
		//PCRTT::Worker::start( worker_type, CHAIN_START );

#if defined(__NCURSES__)
		int c, choice = 0;
		MEVENT event;

		/* Print the menu for the first time */
		print_menu(_win, 1);
		/* Get all the mouse events */
		
		mousemask(ALL_MOUSE_EVENTS, NULL);
		
		while(1)
		{	c = wgetch(_win);
			switch(c)
			{	case KEY_MOUSE:
				if(getmouse(&event) == OK)
				{	/* When the user clicks left mouse button */
					if(event.bstate & BUTTON1_PRESSED)
					{	report_choice(event.x + 1, event.y + 1, &choice);
						if(choice == -1) //Exit chosen
							goto end;
						mvprintw(22, 1, "Choice made is : %d String Chosen is"
							" \"%10s\"", choice, choices[choice - 1]);
						refresh(); 
					}
				}
				print_menu(_win, choice);
				break;
			}
		}
end:
	endwin();
	return 1;
#else
		for(;;)
		{
			usleep( 1000 * 20 );
		}
#endif // 

		return 1;
	}

	return 0;
}

void destroy_gui()
{
	// nop
}

void _debug_gui(const char* file, const int& line, const char* fmt,...)
{
	va_list argList;
	va_start( argList, fmt );
	vprintf( fmt, argList );
	va_end( argList );
}

void clear_output_gui()
{
	system("reset");
}

// ----------------------------------------------------------------------------

void SetButtonStateCalcChainStart()
{
	// nop
}

void SetButtonStateCalcChainPause()
{
	
}

void SetButtonStateCalcChainStop()
{
	
}

void SetButtonStateaddToRbtStart()
{
	
}

void SetButtonStateAddToRbtStop()
{
	
}

void SetButtonStateSearchCrypt8InTsStart()
{
	
}

void SetButtonStateSearchCrypt8InTsStop()
{
	
}

void SetButtonStateSearchCwStart()
{
	
}

void SetButtonStateSearchCwStop()
{
	
}

void UpdateStatistic(const Stats_t* stats, const int& type)
{
	// todo
}

/** string from edit */
PCRTT::String getCalcChainChainDir(void)
{
	PCRTT::String r(chain_dir); return r;
}

PCRTT::String getAddToRbtSrc(void)
{
	PCRTT::String r; return r;
}

PCRTT::String getAddToRbtRbtDir(void)
{
	PCRTT::String r; return r;
}

PCRTT::String getSearchCrypt8InTsSrc(void)
{
	PCRTT::String r; return r;
}

PCRTT::String getSearchCrypt8InTsPid(void)
{
	PCRTT::String r; return r;
}

PCRTT::String getSearchCrypt8InTsPayloadSize(void)
{
	PCRTT::String r; return r;
}

PCRTT::String getSearchCrypt8InTsFileLimit(void)
{
	PCRTT::String r; return r;
}

PCRTT::String getSearchCwRbtDir(void)
{
	PCRTT::String r; return r;
}

PCRTT::String getSearchCwCrypt8(void)
{
	PCRTT::String r; return r;
}

/** to edit */
void setSearchCwCw(const PCRTT::String& str)
{
	
}

void setCalcChainChainDir(const PCRTT::String& s)
{
	chain_dir = s;
}

void setAddToRbtSrc(const PCRTT::String&)
{
	
}

void setAddToRbtRbtDir(const PCRTT::String&)
{
	
}

void setSearchCrypt8InTsSrc(const PCRTT::String&)
{
	
}

void setSearchCrypt8InTsPid(const PCRTT::String&)
{
	
}

void setSearchCrypt8InTsPayloadSize(const PCRTT::String&)
{
	
}

void setSearchCrypt8InTsFileLimit(const PCRTT::String&)
{
	
}

void setSearchCwRbtDir(const PCRTT::String&)
{
	
}

void setSearchCwCrypt8(const PCRTT::String&)
{
	
}

#endif // __CONSOLE__
