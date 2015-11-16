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

static config_t cfg;
static config_t * cfgp = 0;
static char config_file[256] = {0};

config_t* load_config()
{
	char path[256] = {0};
	//const config_setting_t *db;
	//const char *base = NULL;
	//int count;
	//char str[256];
	char* home;
	// .config

	if ( 0 != cfgp )
		return cfgp;

	home = getenv("HOME");

	cfgp = &cfg;

	config_init(cfgp);

	snprintf( path,sizeof(path), "%s/.config/pcrtt", home );
	snprintf( config_file,sizeof(config_file), "%s/pcrtt.conf", path );

	DIR* dr = opendir(path);

	if ( NULL != dr )
		closedir(dr);
	else
		mkdir( path, 0755 );

	if(!file_exists(config_file))
		return cfgp;

	if ( !config_read_file(cfgp, config_file) )
	{
		fprintf(stderr, "%s:%d - %s\n",
			config_file,
			config_error_line(cfgp),
			config_error_text(cfgp)
		);

		config_destroy(cfgp);

		return 0;
	}

	/** load all variables */

	/*db = config_lookup(cfgp, "database");
	count = config_setting_length(db);

	if ( count != 3 )
	{
		fprintf( stderr, "invalid database variable in config file.\n" );
		return 0;
	}*/

	//config_lookup_int(cfgp, "debug", &_config.debug);
	//config_lookup_int(cfgp, "trace", &_config.trace);

	//config_lookup_string(cfgp, "version", (const char**)&base);
	//_config.version = NE5::String( base );

	//config_lookup_string(cfgp, "hostname", (const char**)&base);
	//_config.hostname = String( base );

	//config_lookup_string(cfgp, "user_list_path", (const char**)&base);
	//_config.user_list_path = String( base );

	//config_lookup_string(cfgp, "ftp_path", (const char**)&base);
	//_config. ftp_path  = String( base );

	//config_lookup_string(cfgp, "backup_path", (const char**)&base);
	//_config. backup_path  = String( base );

	//config_lookup_string(cfgp, "urltmp", (const char**)&base);
	//_config. urltmp  = String( base );

	//config_lookup_int(cfgp, "port", &_config.port);
	//config_lookup_int(cfgp, "irc_port", &_config.irc_port);

	//_config.db_user = config_setting_get_string_elem(db, 0);
	//_config.db_passwd = config_setting_get_string_elem(db, 1);
	//_config.db_name = config_setting_get_string_elem(db, 2);
	//config_destroy(cfgp);

	return cfgp;
}

config_t* getConfig()
{ return cfgp; }

void write_config()
{
	//config_setting_t* s = config_setting_add(NULL,name,CONFIG_TYPE_STRING);
	//config_setting_set_string(s,value);
	if(!cfgp)
	{
		fprintf(stderr,"config must be loaded first.");
		return;
	}

	config_write_file( cfgp, config_file );
}

void close_cfg()
{
	config_destroy(cfgp);
}

const char* getExePath(void)
{
	static char buf[128]= { 0 };

	if ( buf[0] != 0 )
		return buf;

	char* current = get_current_dir_name();

	strncpy(buf,current,sizeof(buf));

	return buf;
}

long long get_time(void)
{
	struct timeval tval;
	(void)gettimeofday( &tval, NULL );
	return (long long)tval.tv_sec * 1000000 + tval.tv_usec;
}

double TestSpeed(int start)
{
	static long double /*realt, realt2,*/ t, t2;

	switch( start )
	{
		// start
		case 1:
			//realt = (double)(panGetTime());
			//t = fmodf(realt, 30.0f);
			t = (long double)(get_time());
		break;

		// stop
		case 0:
			//realt2 = (double)(panGetTime());
			//t2 = fmodf(realt2, 30.0f);
			t2 = (long double)(get_time());
		break;
	}

	//return (double)t2-t;
	return (double)( (t2-t) * 0.000001 );
}

#if defined(UNIT_TEST_TEST_SPEED)
int main(int argc, char* argv[])
{
	double total=0.0;
	for(int i = 1; i < 6; ++i)
	{
		double t = TestSpeed(1);
		usleep( 1000 * (i*1000) );
		t = TestSpeed(0);
		total += t;
		printf("pass %d (sec) elapsed: %f msec\n", i, t );
	}
	total -= 15.0;

	printf("all done : test %s (precision: %f)\n",
		(total>0.0&&total<0.1)?"passed":"failed", total/5 );
	return 0;
}
#endif // UNIT_TEST_TEST_SPEED

ulong get_size(FILE* fp)
{
	ulong size = 0;

	if( !fp )
		return size;

	// read the size
	fseek( fp, 0, SEEK_END );
	size = (ulong)ftell( fp );
	fseek( fp, 0, SEEK_SET );

	return size;
}

ulong file_size(const char* filename)
{
	ulong size = 0;
#if defined(HAVE_OPEN) && defined(HAVE_FSTAT) && defined(HAVE_FDOPEN) && \
	defined(HAVE_FSEEKO)

	FILE* fp;
	int fd;
	off_t file_size;
	char *buffer;
	struct stat st;

	fd = open(filename, O_RDONLY);

	if (fd == -1)
	{
		TRACE_ERROR( "Can't open file: %s error: %s\n", filename,
			strerror(errno) );
		return(0);
	}

	fp = fdopen(fd, "r");

	if (fp == NULL)
	{
		TRACE_ERROR( "Can't open file: %s error: %s\n", filename,
			strerror(errno) );
		return(0);
	}

	/* Ensure that the file is a regular file */
	if ((fstat(fd, &st) != 0) || (!S_ISREG(st.st_mode)))
	{
		TRACE_ERROR( "Can't open file: %s error: %s\n", filename,
			strerror(errno) );
		if (fp) fclose(fp);
		return(0);
	}

	if (fseeko(fp, 0 , SEEK_END) != 0)
	{
		TRACE_ERROR( "Can't open file: %s error: %s\n", filename,
			strerror(errno) );
		if (fp) fclose(fp);
		return(0);
	}
	   
	file_size = ftello(fp);

	if (file_size == -1)
	{
		TRACE_ERROR( "Can't open file: %s error: %s\n", filename,
			strerror(errno) );
		if (fp) fclose(fp);
		return(0);
	}

	if (fp) fclose(fp);

	size = (ulong)file_size;

#elif defined(HAVE_OPEN) && defined(HAVE_FSTAT)
	off_t file_size;
	struct stat stbuf;
	int fd;

	fd = open(filename, O_RDONLY);

	if (fd == -1)
	{
		TRACE_ERROR( "Can't open file: %s error: %s\n", filename,
			strerror(errno) );
		return(0);
	}
  
	if ((fstat(fd, &stbuf) != 0) || (!S_ISREG(stbuf.st_mode)))
	{
		TRACE_ERROR( "Can't open file: %s error: %s\n", filename,
			strerror(errno) );
		if(fd) close(fd);
		return(0);
	}

 	if(fd) close(fd);

	file_size = stbuf.st_size;

	size = (ulong)file_size;
#elif defined(HAVE_FOPEN) && defined(HAVE_FCLOSE) && defined(HAVE_FSEEK)
	FILE *fp = 0;
	if( (fp = fopen( filename, "rb" )) == 0)
		return 0;

	// read the size
	fseek( fp, 0, SEEK_END );
	size = (ulong)ftell( fp );
	fseek( fp, 0, SEEK_SET );
	fclose(fp);
#endif // HAVE_OPEN

	return size;
}

int file_exists(const char* filename)
{
#if defined(HAVE_FOPEN) && defined(HAVE_FCLOSE)
	FILE* fp = (FILE*)fopen( filename, "rb" );
	if(fp)
	{
		fclose(fp);
		return 1;
	}
#endif // HAVE_FOPEN
	return 0;
}

void* file_contents(const char *filename, int *length)
{
	int sz = 0; 
	FILE *fp;
	void *buffer;

	sz = file_size(filename);

	if (!sz)
		return 0;

	fp = fopen(filename, "r");

	if ( !fp )
	{
		TRACE_ERROR("Unable to open %s for reading\n", filename);
		return 0;
	}

	buffer = malloc( sz + 1 );
	sz = fread(buffer, 1, sz, fp);
	fclose(fp);

	if (length)
		*length = sz;

    return buffer;
}

char dirsep()
{
#if defined(__WIN32__) && !defined(__CYGWIN__) && !defined(__MINGW__)
	return '\\';
#else
	return '/';
#endif
}

int isdirsep(char c) { return ( c == dirsep() ); }

int isdirectory(const char* n)
{
	// Do a quick optimization for filenames with a trailing slash...
	if (*n && isdirsep(n[strlen(n) - 1]))
		return 1;

#if defined(HAVE_OPENDIR) && defined(HAVE_CLOSEDIR)
		DIR* dr = opendir(n);

		if ( NULL != dr )
		{
			closedir(dr);
			return 1;
		}
#else
	#error TODO: WRITE CODE TO REPLACE OPENDIR CLOSEDIR
#endif // HAVE_OPENDIR
	return 0;
}

const char*	getOsName(void)
{
#if defined(__WIN32__)
	return "Windows";
#elif defined(__LINUX__)
	return "Linux";
#elif (defined(__APPLE__) && defined(__COCOA__))
	return "MacOSX (Using Cocoa)";
#elif defined(__APPLE__) 
	return "MacOSX";
#elif defined(__IPHONE__) || defined(__IPOD__)
	return "iOS";
#elif defined(__IRIX__)
	return "Irix";
#endif
	// todo: other os
	return "unknown os";
}

const char* getTitle(void)
{
	static char buffer[256]={0};
	static int _init = 0;

	if ( _init)
		return buffer;

	_init = 1;

	const char* os_name = getOsName();

	snprintf(buffer, sizeof(buffer),
		"Posix-Rainbow-Table-Tool v%s - %s %s (%s)", 
		PACKAGE_VERSION,os_name, 
#if defined(__x86_64__)
		"x86_64",
#elif defined(__i686__)
		"i686",
#elif defined(__i586__)
		"i586",
#elif defined(__i386__)
		"i386",
#elif defined(__ia64__)
		"ia64",
#elif defined(__powerpc64__)
		"PowerPC64",
#elif defined(__PPC__)
		"PowerPC",
#elif defined(__ARM__)
		"ARM",
#else // todo: other cpu
		"",
#endif
#if defined(_DEBUG)
		"debug"
#else
		"release"
#endif	// _DEBUG
	);
	return buffer;
}

const char* getAboutString(void)
{
	static char buffer[256]={0};
	static int _init = 0;

	if ( _init)
		return buffer;

	_init = 1;

	const char* os_name = getOsName();

	snprintf(buffer, sizeof(buffer),
		"Posix-Rainbow-Table-Tool v%s - %s %s\n"
		"%s version\n"
		"Build date: %s - %s"
		#if defined(__GNUC__)
		"\nGCC version %d.%d.%d"
		#endif
		, 
		PACKAGE_VERSION,os_name, 
#if defined(__x86_64__)
		"x86_64",
#elif defined(__i686__)
		"i686",
#elif defined(__i586__)
		"i586",
#elif defined(__i386__)
		"i386",
#elif defined(__ia64__)
		"ia64",
#elif defined(__powerpc64__)
		"PowerPC64",
#elif defined(__PPC__)
		"PowerPC",
#elif defined(__ARM__)
		"ARM",
#else // todo: other cpu
		"",
#endif
#if defined(_DEBUG)
		"debug",
#else
		"release",
#endif	// _DEBUG
		__DATE__,__TIME__
		#if defined(__GNUC__)
		,__GNUC__,__GNUC_MINOR__,__GNUC_PATCHLEVEL__
		#endif
	);
	return buffer;
}

const char* getBuildString(void)
{
	static char buffer[256]={0};
	static int _init = 0;

	if ( _init)
		return buffer;

	_init = 1;

	snprintf(buffer, sizeof(buffer),
#if defined(__GNUC__)
		"\nGCC version %d.%d.%d\n"
		"Build cmd: %s",
#else
		"",
#endif
#if defined(__GNUC__)
		__GNUC__,__GNUC_MINOR__,__GNUC_PATCHLEVEL__,
		__BUILD_FLAGS__
#endif
	);
	return buffer;
}

#if !defined(HAVE_NANOSLEEP)
int nanosleep(const struct timespec *requested_time,
	struct timespec *remaining_time)
{
	kern_return_t ret;
	mach_timespec_t remain;
	mach_timespec_t current;
	
	if ((requested_time == NULL) || (requested_time->tv_sec < 0) ||
		(requested_time->tv_nsec > NSEC_PER_SEC)) {
		errno = EINVAL;
		return -1;
	}

	ret = clock_get_time(clock_port, &current);
	if (ret != KERN_SUCCESS)
	{
		fprintf(stderr,"clock_get_time() failed: %s\n",mach_error_string(ret));
		return -1;
	}
	/* This depends on the layout of a mach_timespec_t
	 * and timespec_t being equivalent
	 */
	ret = clock_sleep_trap(clock_port, TIME_RELATIVE, requested_time->tv_sec,
			requested_time->tv_nsec, &remain);
	if (ret != KERN_SUCCESS)
	{
		if (ret == KERN_ABORTED)
		{
			errno = EINTR;
			if (remaining_time != NULL)
			{
				ret = clock_get_time(clock_port, &remain);
				if (ret != KERN_SUCCESS)
				{
					fprintf(stderr, "clock_get_time() failed: %s\n",
						mach_error_string(ret));
					return -1;
				}
				ADD_MACH_TIMESPEC(&current, requested_time);
				SUB_MACH_TIMESPEC(&current, &remain);
				remaining_time->tv_sec = current.tv_sec;
				remaining_time->tv_nsec = current.tv_nsec;
			}
		}
		else
		{
			errno = EINVAL;
		}
		return -1;
	}

	return 0;
}

#endif // TIME_H

#if !defined(HAVE_SLEEP)

uint sleep(uint seconds)
{
    struct timespec req, rem;

    if (seconds == 0)
    {
        return 0;
    }
    req.tv_sec = seconds;
    req.tv_nsec = 0;

    /* It's not clear from the spec whether the remainder will be 0
    ** if we weren't interrupted
    */
    if (nanosleep(&req, &rem) == -1)
    {
        return (uint)rem.tv_sec;
    }
    return 0;
}

#endif // HAVE_SLEEP

#if !defined(HAVE_USLEEP)

int usleep(useconds)
{
	struct timespec time_to_sleep;

	time_to_sleep.tv_nsec = (useconds % 1000000) * 1000;
	time_to_sleep.tv_sec = useconds / 1000000;
	return (nanosleep(&time_to_sleep, NULL));
}

#endif // HAVE_USLEEP
