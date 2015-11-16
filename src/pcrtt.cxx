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
#include <vector>
#include <random>
#include <algorithm>

#include <core/memalloc.h>
#include <core/sort.h>


#if defined(__CONSOLE__)
#include <core/console.h>
#endif // __CONSOLE__

static void at_exit__(void)
{
	destroy_gui();
	PCRTT::Worker::destroy();
	PCRTT::MemAlloc::freeMemoryManager();
}

static void process_cmd(int argc, char* argv[])
{
	/** todo : global option */
}

#define FILE_SRC "/tmp/abc3.bin"
#define FILE_DST "/tmp/abc.bin.sorted"
#define ADV_SIZE 12

struct u12
{
	uchar data[12];
};

struct cpuComp
{
	bool operator()(const u12& a, const u12& b)
	{
		return memcmp(a.data+6,b.data+6,6);
	}
};

int unit_test_sort_file()
{
#if 0
	return unit_test_ref();
#else
	const char* src = FILE_SRC;
	const char* dst = FILE_DST;

	FILE* fi, *fo;
	//std::vector<uchar> vuchar;
	std::vector<u12> buffer;
	uchar* data;
	ulong i, offset=0;
	const ulong size = PCRTT::File::Size(src);
	double elapsed;
	const ulong total = size / ADV_SIZE;

	if( (size % ADV_SIZE) != 0 )
	{
		TRACE_ERROR("Invalid size of file source %s / %d", src, size );
		return 0;
	}

	fi = fopen(src,"rb");

	if(!fi)
	{
		TRACE_ERROR("Can't open file source: %s", src );
		return 0;
	}

	fo = fopen(dst,"wb");

	if(!fo)
	{
		TRACE_ERROR("Can't open file dest: %s", dst );
		return 0;
	}

	buffer.resize(total);

	printf("buffer.size: %d\n", buffer.size() );
	data = (uchar*)buffer.data();

	TRACE_DEBUG("Number of chunk to process: %d", total );

	elapsed = TestSpeed(1);
	int len = fread( data, 1, size, fi );
	printf("len: %d size: %d\n", len, size );
	assert(len == size);
	fclose(fi);

	elapsed = TestSpeed(0);

	TRACE_DEBUG("Time for reading = %f msec.\n", elapsed);

	elapsed = TestSpeed(1);
	std::sort( buffer.begin(), buffer.end(), cpuComp() );
	elapsed = TestSpeed(0);

	TRACE_DEBUG( "Time for sorting = %f msec.\n", elapsed );

	elapsed = TestSpeed(1);
	fwrite( data, 1, size, fo );
	elapsed = TestSpeed(0);
	fclose(fo);

	TRACE_DEBUG("Time for writing = %f msec.\n", elapsed);

	for(i = 0; i < buffer.size(); ++i)
	{
		printf( "%d) %u %u %u %u %u %u\n", i,
			buffer[i].data[0+6], buffer[i].data[1+6], buffer[i].data[2+6],
			buffer[i].data[3+6], buffer[i].data[4+6], buffer[i].data[5+6] );
	}
#endif //
	return 1;
}


int main(int argc, char* argv[])
{
	//return PCRTT::unit_test_sort_file();
	//return unit_test_sort_file();

	atexit(at_exit__);

	PCRTT::MemAlloc::initMemoryManager( 5 * 1024 * 1024 );

	process_cmd(argc,argv);

	if (!PCRTT::Worker::init())
		exit(EXIT_FAILURE);

// first we search for a gui
#if defined(__FLTK__)
	return gui_create_fltk(argc,argv);
#elif defined(__GTK__)
	return gui_create_gtk(argc,argv);
#elif defined(__CONSOLE__)
	return gui_create_console(argc,argv);
#else
	return EXIT_FAILURE;
#endif // __FLTK__

	return EXIT_SUCCESS;
}
