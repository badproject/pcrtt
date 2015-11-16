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
#if !defined( PCRTT_SRC_CORE_CORE_HEADER_H_ )
#define PCRTT_SRC_CORE_CORE_HEADER_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

config_t* load_config();
config_t* getConfig();
void write_config();
void close_cfg();

const char* getExePath(void);

long long get_time(void);
double TestSpeed(int start);

/** return 1 if file exist, else 0 */
int file_exists(const char* filename);

void* file_contents(const char *filename, int *length);

ulong get_size(FILE* fp);

ulong file_size(const char* filename);

/**
 * get os dir separator
 * return os specific character separator
 */
char dirsep();

/**
 * check if the char is a directory separator
 * return 1 for a valid dir separator, else 0
 */
int isdirsep(char c);

/**
 * check if the string is a valid directory
 * return 1 for a valid directory, else 0
 */
int isdirectory(const char* n);

/** return the current OS name */
const char*	getOsName(void);

/** internal function to return window title */
const char* getTitle(void);

const char* getAboutString(void);

const char* getBuildString(void);

#if !defined(HAVE_NANOSLEEP)
int nanosleep(const struct timespec *requested_time,
	struct timespec *remaining_time);
#endif // HAVE_NANOSLEEP

#if !defined(HAVE_SLEEP)
uint sleep(uint seconds);
#endif // HAVE_SLEEP

#if !defined(HAVE_USLEEP)
int usleep(useconds);
#endif // HAVE_USLEEP

#ifdef __cplusplus
}
#endif // __cplusplus

#endif	// PCRTT_SRC_CORE_CORE_HEADER_H_
