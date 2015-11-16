#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<assert.h>
#include<signal.h>
#include<execinfo.h>
#include<fcntl.h>
#include "CrashInfo.h"
#define SYMBOL_SIZE 100 
void HandleSIGSEGV(int signal_number);
typedef void (*sighandler_t)(int);
/*DESC: set's up the signal handlers for SIGSEGV, 
 *catches the signals and dumps the backtrace
 *
 **/
void SetUpCrashRecovery(){
	sighandler_t prev_sig_handler;

	prev_sig_handler = signal(SIGSEGV, HandleSIGSEGV); 
	assert(prev_sig_handler != SIG_ERR);
	prev_sig_handler = signal(SIGABRT, HandleSIGSEGV);
	assert(prev_sig_handler != SIG_ERR);
	prev_sig_handler = signal(SIGINT, HandleSIGSEGV);
	assert(prev_sig_handler != SIG_ERR);
}
/*DESC: Handles SIGSEGV, SGINT, SIGABRT */
void HandleSIGSEGV(int signal_number){
	void *symbols[SYMBOL_SIZE];
	int symbol_count;
	FILE *log_fptr = fopen("sigsegv.log", "w");

	if(signal_number == SIGSEGV){
		fprintf(stdout, "%-80s\n", "[SIG-SEGV: received]");
	}else if(signal_number == SIGINT){
		fprintf(stdout, "%-80s\n", "[SIG-INT: program interrupted]");
	}else{
		fprintf(stdout, "%-80s\n", "[SIG-ABT: program aborted]");
	}

	fprintf(stdout, "%-80s\n", "[CREATING BACK-TRACE IN FILE sigsegv.log]");

	symbol_count = backtrace(symbols, SYMBOL_SIZE);
	fprintf(stdout, "%-60s%-20d\n", "[SYMBOLS ON STACK] = ", symbol_count);

	if(log_fptr){
		backtrace_symbols_fd(symbols, SYMBOL_SIZE, fileno(log_fptr));
		fclose(log_fptr);
		fprintf(stdout, "%-80s\n", "[PLEASE SEND THIS LOG FILE TO vamsik@engr.uconn.edu]");
	}else{
		fprintf(stdout, "%-80s\n", "[UNABLE TO OPEN A LOG FILE, I'M HELPLESS...]");
	}
	fflush(stdout);
	exit(1);
}
