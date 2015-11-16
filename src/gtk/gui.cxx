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
#include <gtk/gui.h>
#include <shared/include.h>

/** thank you gtk team for your super easy molly *!%$##! API */

#if defined(__GTK__)

using namespace PCRTT;

#include <gtk/gtk.h>
#include "../../data/logo.xpm"
#include "gtk_glade.h"

#if defined(__DEBUG__)
#define DO_DEBUG \
	printf ("__FUNCTION__ = %s\n", __FUNCTION__); \
	printf ("__PRETTY_FUNCTION__ = %s\n", __PRETTY_FUNCTION__);
#else
#define DO_DEBUG
#endif

#define GET_CFG(name,fnc) \
	{ \
		const char *str; \
		if ( config_lookup_string(cfg, name, &str) ) \
		{ \
			SET_ENTRY(fnc, str ); \
		} else { \
			::printf("No '%s' setting in configuration file.\n",name); \
		} \
	}

#define TOKENIZER_FNC(xx,yy)		TOKEN_NAME_FNC(xx,yy)
#define TO_STR(xx)					#xx

#define GLOBAL_ENTRY(name) static GtkEntry* ed_##name = 0; \
							static String str_##name;

#define GLOBAL_LABEL(name) static GtkLabel* lbl_##name = 0;
#define GLOBAL_BTN(name) static GtkButton* b_##name = 0;
#define GLOBAL_PB(name) static GtkProgressBar* pb_##name = 0;

#define DISABLE_BUTTON(name) \
	gtk_widget_set_sensitive( GTK_WIDGET(b_##name),false);
#define ENABLE_BUTTON(name) \
	gtk_widget_set_sensitive(GTK_WIDGET(b_##name),true);

#define REGISTER_BTN(name,cb) \
	b_##name = GTK_BUTTON(gtk_builder_get_object(builder,#name)); \
	g_signal_connect(b_##name,"clicked",G_CALLBACK(cb),NULL);

#define REGISTER_ENTRY(name) \
	ed_##name = GTK_ENTRY(gtk_builder_get_object(builder,#name));

#define REGISTER_LABEL(name) \
	lbl_##name = GTK_LABEL(gtk_builder_get_object(builder,#name));

#define REGISTER_PBAR(name) \
	pb_##name = GTK_PROGRESS_BAR(gtk_builder_get_object(builder,#name));

#define SET_ENTRY(name, txt) \
	str_##name = txt; \
	printf( TO_STR( ed_##name ) " %d\n", GTK_IS_ENTRY(ed_##name) ); \
	gtk_entry_set_text( GTK_ENTRY( ed_##name ), str_##name.c_str() );

#define GET_ENTRY(name) str_##name

#define SET_LABEL(name,txt) gtk_label_set_text( lbl_##name, txt );

#define OUTPUT_GUI_CONSOLE(txt) \
	if ( gui_is_init ) { \
		GtkTextIter end; \
		output_buffer_str << txt; \
		gtk_text_buffer_set_text(output_buffer,output_buffer_str.c_str(), \
			output_buffer_str.size()); \
		gtk_text_buffer_get_end_iter(output_buffer, &end); \
		gtk_text_view_scroll_to_iter(output_console,&end,0,FALSE,0,0); \
	}

/** set a value 1.0 - 0.0 */
#define SET_PROGRESS_BAR(name, value) \
	gtk_progress_bar_set_fraction(pb_##name,value);

// return a double value
#define GET_PROGRESS_BAR(name) gtk_progress_bar_get_fraction( b_##name );

ushort gui_verbose = 0;
static ushort gui_is_init = 0;
static GtkWindow* main_window = 0;
static Mutex _mutex;
static String output_buffer_str;
static String last_path;			// for selectFile()
static String chain_dir_path;		// do_open_chain_dir / 
static uint worker_type = 

// todo: add a combo box
#if defined(__CUDA__)
 CUDA_WORKER;
#elif defined(__OPENCL__)
 OPENCL_WORKER;
#elif defined(__VULKAN__)
 VULKAN_WORKER;
#endif // defined(__CUDA__)

//-- Calc Chain ---------------------------------------------------------------
// -- BTN
GLOBAL_BTN( calcChain_chainDir_Open )
GLOBAL_BTN( calcChain_Start )
GLOBAL_BTN( calcChain_Pause )
GLOBAL_BTN( calcChain_Stop )
// -- ENTRY
GLOBAL_ENTRY( calcChain_chainDir_Filename )
GLOBAL_LABEL( calcChain_speed_output )
GLOBAL_LABEL( calcChain_part_percent )
GLOBAL_LABEL( calcChain_file_percent )

// -- PROGRESS BAR
GLOBAL_PB( calcChain_ProgressPart )
GLOBAL_PB( calcChain_ProgressFile )

//-- Add Rainbow Table --------------------------------------------------------
// -- BTN
GLOBAL_BTN( addRainbowTable_chainFile_Open )
GLOBAL_BTN( addRainbowTable_rbtDir_Open )
GLOBAL_BTN( addRainbowTable_Merge )
GLOBAL_BTN( addRainbowTable_Stop )
// -- ENTRY
GLOBAL_ENTRY( addRainbowTable_chainFile_Filename )
GLOBAL_ENTRY( addRainbowTable_rbtDir_Filename )
GLOBAL_LABEL( addRainbowTable_percent )
// -- PROGRESS BAR
GLOBAL_PB( addRainbowTable_ProgressFile )

//-- Search Crypt8 ------------------------------------------------------------
// -- BTN
GLOBAL_BTN( searchCrypt8_rbtDir_Open )
GLOBAL_BTN( searchCrypt8_Start )
GLOBAL_BTN( searchCrypt8_Stop )
// -- ENTRY
GLOBAL_ENTRY( searchCrypt8_Filename )
GLOBAL_ENTRY( searchCrypt8_PID )
GLOBAL_ENTRY( searchCrypt8_PL_size )
GLOBAL_ENTRY( searchCrypt8_File_limit )
GLOBAL_LABEL( searchCrypt8_percent )
// -- PROGRESS BAR
GLOBAL_PB( searchCrypt8_ProgressFile )

//-- Search CW ----------------------------------------------------------------
// -- BTN
GLOBAL_BTN( searchCW_Open )
GLOBAL_BTN( searchCW_Start )
GLOBAL_BTN( searchCW_Stop )
// -- ENTRY
GLOBAL_ENTRY( searchCW_Crypt8 )
GLOBAL_ENTRY( searchCW_RBTDir )
GLOBAL_ENTRY( searchCW_CW )

// global entry
static GtkTextView* output_console = 0;
static GtkTextBuffer* output_buffer = 0;
static GtkTextTagTable* output_tab_table = 0;

// minimize warning message deprecated
static inline void g_lock()
{
	gdk_threads_enter();
}

static inline void g_unlock()
{
	gdk_threads_leave();
}

static void register_builder(GtkBuilder *builder)
{
	//-- Calc Chain
	// -- BTN
	REGISTER_BTN( calcChain_chainDir_Open, do_open_chain_dir )
	REGISTER_BTN( calcChain_Start, do_calc_chain_start_btn )
	REGISTER_BTN( calcChain_Pause, do_calc_chain_pause_btn )
	REGISTER_BTN( calcChain_Stop, do_calc_chain_stop_btn )
	// -- ENTRY
	REGISTER_ENTRY( calcChain_chainDir_Filename )
	// -- LABEL
	REGISTER_LABEL( calcChain_speed_output )
	REGISTER_LABEL( calcChain_part_percent )
	REGISTER_LABEL( calcChain_file_percent )
	// -- PROGRESS BAR
	REGISTER_PBAR( calcChain_ProgressPart )
	REGISTER_PBAR( calcChain_ProgressFile )
	//-- Add Rainbow Table
	// -- BTN
	REGISTER_BTN( addRainbowTable_chainFile_Open, do_open_chain_file )
	REGISTER_BTN( addRainbowTable_rbtDir_Open, do_open_rainbow_table_rbt_dir )
	REGISTER_BTN( addRainbowTable_Merge, do_add_rainbow_table_merge_btn )
	REGISTER_BTN( addRainbowTable_Stop, do_add_rainbow_table_stop_btn )
	// -- ENTRY
	REGISTER_ENTRY( addRainbowTable_chainFile_Filename )
	REGISTER_ENTRY( addRainbowTable_rbtDir_Filename )
	REGISTER_LABEL( addRainbowTable_percent )
	// -- PROGRESS BAR
	REGISTER_PBAR( addRainbowTable_ProgressFile )
	//-- Search Crypt8
	// -- BTN
	REGISTER_BTN( searchCrypt8_rbtDir_Open, do_open_ts_file )
	REGISTER_BTN( searchCrypt8_Start, do_calc_search_crypt8_start_btn )
	REGISTER_BTN( searchCrypt8_Stop, do_calc_search_crypt8_stop_btn )
	// -- ENTRY
	REGISTER_ENTRY( searchCrypt8_Filename )
	REGISTER_ENTRY( searchCrypt8_PID )
	REGISTER_ENTRY( searchCrypt8_PL_size )
	REGISTER_ENTRY( searchCrypt8_File_limit )
	REGISTER_LABEL( searchCrypt8_percent )
	// -- PROGRESS BAR
	REGISTER_PBAR( searchCrypt8_ProgressFile )
	//-- Search CW
	// -- BTN
	REGISTER_BTN( searchCW_Open, do_open_search_cw_rbt_dir )
	REGISTER_BTN( searchCW_Start, do_calc_search_cw_start_btn )
	REGISTER_BTN( searchCW_Stop, do_calc_search_cw_stop_btn )
	// -- ENTRY
	REGISTER_ENTRY( searchCW_Crypt8 )
	REGISTER_ENTRY( searchCW_RBTDir )
	REGISTER_ENTRY( searchCW_CW )

	output_console = GTK_TEXT_VIEW(gtk_builder_get_object(builder,
						"output_console"));

	output_tab_table = gtk_text_tag_table_new();

	output_buffer = gtk_text_buffer_new( output_tab_table );

	gtk_text_view_set_buffer(output_console,output_buffer);
}

void destroy_gui()
{
	// nop
}

void clear_output_gui()
{
	output_buffer_str.clear();
}

void _debug_gui(const char* file, const int& line, const char* fmt,...)
{
	int sz = 8192;
	va_list argList;

	char* str = new char[sz];
	str[0]=0;

	va_start( argList, fmt );
	vsnprintf( str, sz, fmt, argList );
	va_end( argList );

#if defined(__DEBUG__)
	::printf( "%s:%d - %s\n", String::basename(file).c_str(), line, str );
#endif

	g_lock();
	OUTPUT_GUI_CONSOLE( str )
	g_unlock();

	KILLARRAY(str);
}

int gui_create_gtk(int argc, char* argv[])
{
	const String title(getTitle());

	GtkBuilder *builder;
	GtkWidget  *window;
	GtkImage *	logo;
	GtkLabel* about_label;
	GdkPixbuf *pixbuf;
	GError	 *error = NULL;

#if GTK_CHECK_VERSION(2, 14, 0)
	/* Secure glib */
	if( ! g_thread_supported() )
		g_thread_init( NULL );

	/* Secure gtk */
	gdk_threads_init();

	/* Obtain gtk's global lock */
	g_lock();
#endif // GTK_CHECK_VERSION <= 2.14.0

	// Init GTK+
	gtk_init(&argc,&argv);

	const gchar *buffer = gtk_glade_src;
	gsize length = gtk_glade_size;

	// Create new GtkBuilder object
	builder = gtk_builder_new();

	// load ui
	//if( ! gtk_builder_add_from_file( builder, "gtk_tab.glade", &error ) )
	if( !gtk_builder_add_from_string(builder,buffer,length,&error) )
	{
		g_warning( "%s", error->message );
		g_free( error );
		return( 1 );
	}

	/* Get main window pointer from UI */
	window = GTK_WIDGET( gtk_builder_get_object( builder, "main_window" ) );

	main_window = GTK_WINDOW(window);

	// register signal
	g_signal_connect(window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

	register_builder(builder);

	// set title
	gtk_window_set_title (GTK_WINDOW(window), title);
	// to the output console too
	gui_is_init = 1; // sufficient for the output console
	OUTPUT_GUI_CONSOLE( title )

	// create logo
	pixbuf = gdk_pixbuf_new_from_xpm_data( (const char**)logo_xpm );
	logo = GTK_IMAGE( gtk_builder_get_object( builder, "about_logo" ) );
	gtk_image_set_from_pixbuf(logo,pixbuf);

	// set label to about
	about_label = GTK_LABEL( gtk_builder_get_object( builder, "about_label" ));
	gtk_label_set_text( about_label, getAboutString() );

	// load config here, without thread lock
	config_t* cfg = load_config();
	if (!cfg)
	{
		printf("Invalid config file.\n");
		return -1;
	}

	GET_CFG("calc_chain_chain_dir", calcChain_chainDir_Filename );
	GET_CFG("add_to_rbt_src", addRainbowTable_chainFile_Filename );
	GET_CFG("add_to_rbt_rbt_dir", addRainbowTable_rbtDir_Filename );
	GET_CFG("search_crypt8_in_ts_src", searchCrypt8_Filename );
	GET_CFG("search_crypt8_in_ts_pid", searchCrypt8_PID );
	GET_CFG("search_crypt8_in_ts_payload_size",searchCrypt8_PL_size);
	GET_CFG("search_crypt8_in_ts_file_size", searchCrypt8_File_limit);
	GET_CFG("search_cw_rbt_dir", searchCW_RBTDir );
	GET_CFG("search_cw_crypt", searchCW_Crypt8 );

	// Connect signals
	gtk_builder_connect_signals( builder, NULL );

	// Destroy builder, since we don't need it anymore
	g_object_unref( G_OBJECT( builder ) );

	// Show window. All other widgets are automatically shown by GtkBuilder
	gtk_widget_show( window );

	/* Start main loop */
	gtk_main();

#if GTK_CHECK_VERSION(2, 14, 0)
	/* Release gtk's global lock */
	g_unlock();
#endif // GTK_CHECK_VERSION <= 2.14.0

//	PCRTT::Shared::SaveLoadSettings(0); // load
	::printf("gtkinit released\n");
	return( 0 );
}

static String selectDirectory(const char *title,const char* extension)
{
	String str;
	GtkWidget *dialog;
	gboolean ret = 0;
	gint res;
	GtkFileFilter *filter;
	GtkFileChooser *chooser;

	dialog = gtk_file_chooser_dialog_new("Open Directory",
				main_window,
				GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
				"_Cancel", GTK_RESPONSE_CANCEL,
				"_OK", GTK_RESPONSE_ACCEPT,
				NULL);

	gtk_window_set_modal(GTK_WINDOW(dialog), true );

	// when multi selection
	// gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER( dialog ), 0 );
	// ? g_object_set(G_OBJECT(dialog),"show-hidden",TRUE,NULL);

	if ( last_path.size() )
	{
		ret = gtk_file_chooser_set_current_folder(
			GTK_FILE_CHOOSER(dialog), last_path.c_str() );
	}

	if( !ret )
		ret = gtk_file_chooser_set_current_folder(
			GTK_FILE_CHOOSER(dialog), "." );

	int j=0;
	filter = gtk_file_filter_new ();

	chooser = GTK_FILE_CHOOSER (dialog);

	char **globs = g_strsplit(extension," ",0);

	while( globs[j]!=NULL ) {
		gtk_file_filter_add_pattern( filter, globs[j] );
		j++;
	}

	g_strfreev( globs );

	gtk_file_chooser_set_filter( chooser, filter );
	//gtk_file_chooser_add_filter (chooser, filter);

	res = gtk_dialog_run( GTK_DIALOG(dialog) );

	if (res == GTK_RESPONSE_ACCEPT)
	{
		char *filename;
		filename = gtk_file_chooser_get_filename( chooser );
		str = String( filename );
		g_free( filename );
	}

	//gtk_file_chooser_remove_filter( chooser, filter );
	gtk_widget_destroy( dialog );

	if ( str.size() )
		last_path = String::getPath( str );

	return str;
	(void)title;
}

static String selectFile(const char *title,const char* extension)
{
	String str;
	GtkWidget *dialog;
	gboolean ret = 0;
	gint res;
	GtkFileFilter *filter;
	GtkFileChooser *chooser;

	dialog = gtk_file_chooser_dialog_new(title,
				main_window,
				GTK_FILE_CHOOSER_ACTION_OPEN,
				"_Cancel", GTK_RESPONSE_CANCEL,
				"_OK", GTK_RESPONSE_ACCEPT,
				NULL);

	gtk_window_set_modal(GTK_WINDOW(dialog), true );

	// when multi selection
	// gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER( dialog ), 0 );
	// ? g_object_set(G_OBJECT(dialog),"show-hidden",TRUE,NULL);

	if ( last_path.size() )
	{
		ret = gtk_file_chooser_set_current_folder(
			GTK_FILE_CHOOSER(dialog), last_path.c_str() );
	}

	if( !ret )
		ret = gtk_file_chooser_set_current_folder(
			GTK_FILE_CHOOSER(dialog), "." );

	int j=0;
	filter = gtk_file_filter_new ();

	chooser = GTK_FILE_CHOOSER (dialog);

	char **globs = g_strsplit(extension," ",0);

	while( globs[j]!=NULL ) {
		gtk_file_filter_add_pattern( filter, globs[j] );
		j++;
	}

	g_strfreev( globs );

	gtk_file_chooser_set_filter( chooser, filter );
	//gtk_file_chooser_add_filter (chooser, filter);

	res = gtk_dialog_run( GTK_DIALOG(dialog) );

	if (res == GTK_RESPONSE_ACCEPT)
	{
		char *filename;
		filename = gtk_file_chooser_get_filename( chooser );
		str = String( filename );
		g_free( filename );
	}

	//gtk_file_chooser_remove_filter( chooser, filter );
	gtk_widget_destroy( dialog );

	if ( str.size() )
		last_path = String::getPath( str );

	return str;
	(void)title;
}

static String saveFile(const char* title, const char* extension)
{
	String str;
	GtkWidget *dialog;
	GtkFileFilter *filter;
	char* home = getenv("HOME");

	dialog = gtk_file_chooser_dialog_new ("Save File",
				main_window,
				GTK_FILE_CHOOSER_ACTION_SAVE,
				"_Cancel" /*GTK_STOCK_CANCEL*/, GTK_RESPONSE_CANCEL,
				"_Save" /*GTK_STOCK_SAVE*/, GTK_RESPONSE_ACCEPT,
				NULL);

	gtk_file_chooser_set_do_overwrite_confirmation(
		GTK_FILE_CHOOSER(dialog), TRUE);

	gtk_file_chooser_set_current_folder(
		GTK_FILE_CHOOSER (dialog), home);
	gtk_file_chooser_set_current_name(
		GTK_FILE_CHOOSER (dialog), "Untitled document");

	int j=0;
	filter = gtk_file_filter_new ();
	char **globs = g_strsplit(extension," ",0);
	while( globs[j]!=NULL ) {
		gtk_file_filter_add_pattern( filter, globs[j] );
		j++;
	}
	g_strfreev( globs );
	//gtk_file_filter_add_pattern (filter, extension);
	gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(dialog),filter);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	{
		char *filename;

		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		//save_to_file(filename);
		str = String(filename);
		g_free(filename);
	}

	gtk_widget_destroy(dialog);
	return str;
	(void)title;
}

/** global function used in thread / dialog */
uint get_worker_type()
{
	return worker_type;
}

const char* get_calc_chain_dir(void)
{
	if ( chain_dir_path.size() )
		return chain_dir_path.c_str();
	return 0;
}

// -- callback with gtk

void do_open_chain_dir(void)
{
	String path = selectDirectory("Select directory","*");

	if ( !path.size() )
		return;

	SET_ENTRY( calcChain_chainDir_Filename, path );

	chain_dir_path = path;
}

void do_calc_chain_start_btn(void)
{
	PCRTT::Worker::start( worker_type, CHAIN_START );
}

void do_calc_chain_pause_btn(void)
{
	DO_DEBUG
	PCRTT::Worker::start( worker_type, CHAIN_PAUSE );
}

void do_calc_chain_stop_btn(void)
{
	DO_DEBUG
	PCRTT::Worker::start( worker_type, CHAIN_STOP );
}

void do_open_chain_file(void)
{
	String file = selectFile("Select Chain File","*.rbc");

	if ( !file.size() )
		return;

	SET_ENTRY( addRainbowTable_chainFile_Filename, file );
}

void do_open_rainbow_table_rbt_dir(void)
{
	String path = selectDirectory("Select directory","*");

	if ( !path.size() )
		return;

	SET_ENTRY( addRainbowTable_rbtDir_Filename, path );
}

void do_add_rainbow_table_merge_btn(void)
{
	DO_DEBUG
	PCRTT::Worker::start( worker_type, RBT_START );
}

void do_add_rainbow_table_stop_btn(void)
{
	DO_DEBUG
	PCRTT::Worker::start( worker_type, RBT_STOP );
}

void do_open_ts_file(void)
{
	DO_DEBUG
	String file = selectFile("Select TS File","*.ts");

	if ( !file.size() )
		return;

	SET_ENTRY( searchCrypt8_Filename, file );
}

void do_calc_search_crypt8_start_btn(void)
{
	DO_DEBUG
	PCRTT::Worker::start( worker_type, C8_START );
}

void do_calc_search_crypt8_stop_btn(void)
{
	DO_DEBUG
	PCRTT::Worker::start( worker_type, C8_STOP );
}

void do_open_search_cw_rbt_dir(void)
{
	DO_DEBUG
	g_lock();
	g_unlock();
}

void do_calc_search_cw_start_btn(void)
{
	DO_DEBUG
	g_lock();
	g_unlock();
}

void do_calc_search_cw_stop_btn(void)
{
	DO_DEBUG
	g_lock();
	g_unlock();
}

// --

#define addRainbowTable_Start addRainbowTable_Merge

static void enable_all_button()
{
	DO_DEBUG

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
	DO_DEBUG

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
	DO_DEBUG
	g_lock();

	disable_all_button();
	ENABLE_BUTTON( calcChain_Pause )
	ENABLE_BUTTON( calcChain_Stop )
	DISABLE_BUTTON( calcChain_Start )
	g_unlock();
}

void SetButtonStateCalcChainPause()
{
	DO_DEBUG
	g_lock();

	disable_all_button();
	ENABLE_BUTTON( calcChain_Start )
	ENABLE_BUTTON( calcChain_Stop )
	g_unlock();
}

void SetButtonStateCalcChainStop()
{
	DO_DEBUG
	g_lock();

	disable_all_button();
	ENABLE_BUTTON( searchCW_Start )
	ENABLE_BUTTON( searchCrypt8_Start )
	ENABLE_BUTTON( addRainbowTable_Merge )
	ENABLE_BUTTON( calcChain_Start )
	g_unlock();
}

void SetButtonStateaddToRbtStart()
{
	DO_DEBUG
	g_lock();

	disable_all_button();
	ENABLE_BUTTON( addRainbowTable_Stop )
	g_unlock();
}

void SetButtonStateAddToRbtStop()
{
	DO_DEBUG
	g_lock();

	enable_all_button();
	DISABLE_BUTTON( calcChain_Pause )
	DISABLE_BUTTON( calcChain_Stop )
	DISABLE_BUTTON( addRainbowTable_Stop )
	DISABLE_BUTTON( searchCrypt8_Stop )
	DISABLE_BUTTON( searchCW_Stop )
	g_unlock();
}

void SetButtonStateSearchCwStart()
{
	DO_DEBUG
	g_lock();

	disable_all_button();
	ENABLE_BUTTON( searchCW_Stop )
	g_unlock();
}

void SetButtonStateSearchCwStop()
{
	DO_DEBUG
	g_lock();

	disable_all_button();
	ENABLE_BUTTON( searchCW_Start )
	ENABLE_BUTTON( searchCrypt8_Start )
	ENABLE_BUTTON( addRainbowTable_Merge )
	ENABLE_BUTTON( calcChain_Start )
	g_unlock();
}

void SetButtonStateSearchCrypt8InTsStart()
{
	DO_DEBUG
	g_lock();

	disable_all_button();
	ENABLE_BUTTON( searchCrypt8_Stop )
	g_unlock();
}

void SetButtonStateSearchCrypt8InTsStop()
{
	DO_DEBUG
	g_lock();

	disable_all_button();
	ENABLE_BUTTON( searchCW_Start )
	ENABLE_BUTTON( searchCrypt8_Start )
	ENABLE_BUTTON( addRainbowTable_Merge )
	ENABLE_BUTTON( calcChain_Start )
	g_unlock();
}

/*void SearchCw::Display_SearchCw_Cw(const char* str)
{
	SendToGui( ID_MSG1, DISPLAY_SEARCH_CW_CW, (LPARAM) pString );
}*/

void UpdateStatistic(const Stats_t* stats, const int& type)
{
	g_lock();

//	DO_DEBUG

	String str;
	double part_perc;
	double file_perc;

	switch(type)
	{
		// calc chain
		case Stats_t::CALC:
			part_perc = (double)stats->ProcCalcChainPart;
			file_perc = (double)stats->ProcCalcChainFile;

			SET_PROGRESS_BAR(calcChain_ProgressPart, (part_perc / 100.0))
			SET_PROGRESS_BAR(calcChain_ProgressFile, (file_perc / 100.0))

			if ( part_perc != 0 )
			{
				str = format("%.2f%%", part_perc );
			}

			SET_LABEL( calcChain_part_percent, str )
			str.clear();

			if ( stats->ProcCalcChainFile != 0 )
			{
				str = format("%.2f%%", file_perc);
			}

			SET_LABEL( calcChain_file_percent, str )
			str.clear();

			if(stats->kCW_per_sec != 0)
				str = format("%lu kCW/sec (%llu ms/kernel call)",
						stats->kCW_per_sec, stats->DeltaTime);

			SET_LABEL(calcChain_speed_output,str);
		break;

		// add to RBT
		case Stats_t::RBT:
			file_perc = (double)stats->ProcAddToRbtFile;
			SET_PROGRESS_BAR(addRainbowTable_ProgressFile,(file_perc / 100.0))

			if ( file_perc != 0 )
				str = format("%.2f%%", file_perc);

			SET_LABEL( addRainbowTable_percent, str )
		break;

		// search crypt8 in TS
		case Stats_t::CRYPT8:
			file_perc = (double)stats->ProcSearchCrypt8InTsFile;
			SET_PROGRESS_BAR(searchCrypt8_ProgressFile,	(file_perc/100.0) )

			if(file_perc != 0)
				str = format("%.2f%%", file_perc);

			SET_LABEL(searchCrypt8_percent,str)
		break;
	}
	g_unlock();
}

// --
String getCalcChainChainDir(void)
{
	DO_DEBUG
	g_lock();
	String r( GET_ENTRY( calcChain_chainDir_Filename ) );
	g_unlock();
	return r;
}

String getAddToRbtSrc(void)
{
	DO_DEBUG
	g_lock();
	String r( GET_ENTRY( addRainbowTable_chainFile_Filename ) );
	g_unlock();
	return r;
}

String getAddToRbtRbtDir(void)
{
	DO_DEBUG
	g_lock();
	String r( GET_ENTRY( addRainbowTable_rbtDir_Filename ) );
	g_unlock();
	return r;
}

String getSearchCrypt8InTsSrc(void)
{
	DO_DEBUG
	g_lock();
	String r( GET_ENTRY( searchCrypt8_Filename ) );
	g_unlock();
	return r;
}

String getSearchCrypt8InTsPid(void)
{
	DO_DEBUG
	g_lock();
	String r( GET_ENTRY( searchCrypt8_PID ) );
	g_unlock();
	return r;
}

String getSearchCrypt8InTsPayloadSize(void)
{
	DO_DEBUG
	g_lock();
	String r( GET_ENTRY( searchCrypt8_PL_size ) );
	g_unlock();
	return r;
}

String getSearchCrypt8InTsFileLimit(void)
{
	DO_DEBUG
	g_lock();
	String r( GET_ENTRY( searchCrypt8_File_limit ) );
	g_unlock();
	return r;
}

String getSearchCwRbtDir(void)
{
	DO_DEBUG
	g_lock();
	String r( GET_ENTRY( searchCW_RBTDir ) );
	g_unlock();
	return r;
}

// GLOBAL_ENTRY( searchCW_Crypt8 )
// GLOBAL_ENTRY( searchCW_CW )

String getSearchCwCrypt8(void)
{
	DO_DEBUG
	g_lock();
	String r( GET_ENTRY( searchCW_Crypt8 ) );
	g_unlock();
	return r;
}

// --

void setSearchCwCw(const String& str)
{
	DO_DEBUG
	g_lock();
	SET_ENTRY(searchCW_CW,str);
	g_unlock();
}

void setCalcChainChainDir(const String& str)
{
	DO_DEBUG

	g_lock();
	SET_ENTRY(calcChain_chainDir_Filename,str);
	g_unlock();
}

void setAddToRbtSrc(const String& str)
{
	DO_DEBUG
	g_lock();
	SET_ENTRY(addRainbowTable_chainFile_Filename,str);
	g_unlock();
}

void setAddToRbtRbtDir(const String& str)
{
	DO_DEBUG
	g_lock();
	SET_ENTRY(addRainbowTable_rbtDir_Filename,str);
	g_unlock();
}

void setSearchCrypt8InTsSrc(const String& str)
{
	DO_DEBUG
	g_lock();
	SET_ENTRY(searchCrypt8_Filename,str);
	g_unlock();
}

void setSearchCrypt8InTsPid(const String& str)
{
	DO_DEBUG
	g_lock();
	SET_ENTRY(searchCrypt8_PID,str);
	g_unlock();
}

void setSearchCrypt8InTsPayloadSize(const String& str)
{
	DO_DEBUG
	g_lock();
	SET_ENTRY(searchCrypt8_PL_size,str);
	g_unlock();
}

void setSearchCrypt8InTsFileLimit(const String& str)
{
	DO_DEBUG
	g_lock();
	SET_ENTRY(searchCrypt8_File_limit,str);
	g_unlock();
}

void setSearchCwRbtDir(const String& str)
{
	DO_DEBUG
	g_lock();
	SET_ENTRY(searchCW_RBTDir,str);
	g_unlock();
}

void setSearchCwCrypt8(const String& str)
{
	DO_DEBUG
	g_lock();
	SET_ENTRY(searchCW_Crypt8,str);
	g_unlock();
}

#endif // __GTK__
