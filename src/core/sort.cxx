/**
 *
 * You could use this program under the terms of GPL v3, for more details see:
 * 
 * http://www.gnu.org/copyleft/gpl.html
 * 
 * Copyright 2012 Sol from www.solarianprogrammer.com
 * 
 * ref: https://github.com/sol-prog/Sort_data_parallel/
 * 
 */
#include <pcrtt.h>

#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <thread>
#include <ctime>

#include <core/sort.h>

namespace PCRTT {

/** 
 * Split "mem" into "parts", e.g. if mem = 10 and parts = 4 you will
 * have: 0,2,4,6,10 
 * if possible the function will split mem into equal chuncks, if not 
 * the last chunck will be slightly larger
 */
static std::vector<uint> bounds(uint parts, uint mem)
{
	std::vector<uint>bnd(parts + 1);
	uint delta = mem / parts;
	uint reminder = mem % parts;
	uint N1 = 0, N2 = 0;
	bnd[0] = N1;
	for (uint i = 0; i < parts; ++i)
	{
		N2 = N1 + delta;
		if (i == parts - 1)
			N2 += reminder;
		bnd[i + 1] = N2;
		N1 = N2;
	}
	return bnd;
}

static compare_fnc compare=0;
static ushort _id = 0;

static void test_sort_cmp(Chunk_v &V, uint left, uint right)
{
	std::sort( std::begin(V) + left, std::begin(V) + right, compare );
}

static void test_sort(Chunk_v &V, uint left, uint right)
{
	std::sort( std::begin(V) + left, std::begin(V) + right, Chunk_t() );
}

// Merge V[n0:n1] with V[n2:n3]. The result is put back to V[n0:n3]
static void par_merge(Chunk_v &V,uint n0,uint n1,uint n2,uint n3)
{
	std::inplace_merge(std::begin(V) + n0, std::begin(V) + n1, 
		std::begin(V) + n3);
}

double sort(Chunk_v &V, uint parts, uint mem, compare_fnc cmp)
{
	time_t begin, end;
	double time_elapsed=0.0;

	/*if ( cmp )
		std::sort(std::begin(V), std::end(V), cmp );
	else
		std::sort(std::begin(V), std::end(V) );

	return time_elapsed;*/

	//Split the data in "parts" pieces and sort each piece in a separate thread
	std::vector<uint> bnd = bounds( parts, mem );
	std::vector<std::thread> thr;

	compare = cmp;

	begin = std::clock();

	// Launch "parts" threads
	for(uint i = 0; i < parts; ++i)
	{
		if ( cmp )
		{
			thr.push_back(std::thread(test_sort_cmp, std::ref(V), 
				bnd[i], bnd[i + 1]));
		}
		else
		{
			thr.push_back(std::thread(test_sort, std::ref(V), 
				bnd[i], bnd[i + 1]));
		}
	}

	for(auto &t : thr)
	{
		t.join();
	}

	//Merge data
	while( parts >= 2 )
	{
		std::vector<uint> limits;
		std::vector<std::thread> th;

		for(uint i = 0; i < parts - 1; i += 2)
		{
			th.push_back(std::thread(par_merge, std::ref(V), 
				bnd[i], bnd[i + 1], bnd[i + 1], bnd[i + 2]));
			
			uint naux = limits.size();
			if(naux > 0)
			{
				if(limits[naux - 1] != bnd[i])
				{
					limits.push_back(bnd[i]);
				}
				limits.push_back(bnd[i + 2]);
			}
			else
			{
				limits.push_back(bnd[i]);
				limits.push_back(bnd[i + 2]);
			} 
		}

		for(auto &t : th)
		{
			t.join();
		}

		parts /= 2;
		bnd = limits;
	}

	end = std::clock();
	time_elapsed = double(end-begin)/CLOCKS_PER_SEC;

	return time_elapsed;
}

// Use only std::sort for parts = 1
/*double run_tests_std_sort(Chunk_v &V, uint parts, uint mem)
{
	std::sort(std::begin(V), std::end(V));
}*/

//-----------------------------------------------------------------------------

#if 0 // cuda

#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <boost/chrono.hpp>

#include <thrust/device_vector.h>
#include <thrust/host_vector.h>
#include <thrust/sort.h>

//Fill a vector with random numbers in the range [lower, upper]
void rnd_fill(thrust::host_vector<double> &V, const double lower, const double upper, const unsigned int seed) {

	//Create a unique seed for the random number generator
	srand(time(NULL));
	
	uint elem = V.size();
	for( uint i = 0; i < elem; ++i){
		V[i] = (double) rand() / (double) RAND_MAX;
	}
}

int main() {
	thrust::host_vector<double> V;
	thrust::device_vector<double> d_V;
	
	//use the system time to create a random seed
	unsigned int seed = (unsigned int) time(NULL);
	
	uint step = 10;
	uint mem = 10000000;

	for(uint i = 16; i <= mem; i = 2 * step, step *= 1.1) {
		//Fill V with random numbers in the range [0,1]:
		V.resize(i);
		rnd_fill(V, 0.0, 1.0, seed);
		
		boost::chrono::steady_clock::time_point start_cpu = boost::chrono::steady_clock::now();
		d_V = V; // Transfer data to the GPU
		boost::chrono::steady_clock::time_point end_cpu = boost::chrono::steady_clock::now();
		double dt1 = boost::chrono::duration <double, boost::milli> (end_cpu - start_cpu).count();

		cudaEvent_t start, stop;
		cudaEventCreate(&start);
		cudaEventCreate(&stop);

		//Start recording
		cudaEventRecord(start,0);
		
			thrust::stable_sort(d_V.begin(), d_V.end());
		
		//Stop recording
		cudaEventRecord(stop,0);
		cudaEventSynchronize(stop);
		float elapsedTime;
		cudaEventElapsedTime(&elapsedTime, start, stop);

		cudaEventDestroy(start);
		cudaEventDestroy(stop);
		
		start_cpu = boost::chrono::steady_clock::now();
		V = d_V; // Transfer data to the CPU
		end_cpu = boost::chrono::steady_clock::now();
		double dt2 = boost::chrono::duration <double, boost::milli> (end_cpu - start_cpu).count();
		

	//std::cout << i << "\t" << elapsedTime << "\t" << dt1 + dt2 << std::endl;
	std::cout << i << "\t" << elapsedTime + dt1 + dt2 << std::endl;
	}
	
	return 0;
}
#endif // 0 cuda

// ----------------------------------------------------------------------------

int sort_file(const char* src, const char* dst)
{
/*	FILE* fi, *fo;
	PCRTT::Chunk_v buffer;
	ulong i, size = File::Size(src);
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

	buffer.reserve(total);

	elapsed = TestSpeed(1);

	for(i = 0; i < total; ++i)
	{
		int len;
		Chunk_t t;

		t.size = ADV_SIZE;
		t.scmp = ADV_SIZE / 2;
		t.offset = ADV_SIZE / 2;
		t.data = new uchar[ADV_SIZE];

		len = fread( t.data, 1, ADV_SIZE, fi );

		assert(len == ADV_SIZE);

		buffer.push_back( t );
	}
	elapsed = TestSpeed(0);
	fclose(fi);

	TRACE_DEBUG("Time for reading = %f msec.\n", elapsed);

	elapsed = TestSpeed(1);
	sort( buffer, 8, 32 );
	elapsed = TestSpeed(0);

	TRACE_DEBUG( "Time for sorting = %f msec.\n", elapsed );

	elapsed = TestSpeed(1);
	for(i = 0; i < total; ++i)
	{
		int len = fwrite( buffer[i].data, 1, ADV_SIZE, fo );
		assert(len == ADV_SIZE);
		KILLARRAY(buffer[i].data);
	}
	elapsed = TestSpeed(0);
	fclose(fo);

	TRACE_DEBUG("Time for writing = %f msec.\n", elapsed);
*/
	return 1;
}


static bool compare2(const Chunk_t& a, const Chunk_t& b)
{
	const uchar* aa = a.data;
	const uchar* bb = b.data;
	++_id;
	for ( int i=0; i< 6; ++i )
	{
		if(aa[6+i] != bb[6+i])
		{
			return aa[6+i] - bb[6+i];
		}
	}
	return 0;
}

#define FILE_SRC "/tmp/abc3.bin"
#define FILE_DST "/tmp/abc.bin.sorted"
#define ADV_SIZE 12

static int ref_compare(const void * a, const void * b)
{
	// cmp EndCw
	int i;
	const uchar *aa = (uchar*)a;
	const uchar *bb = (uchar*)b;

	for ( i=0; i< 6; ++i )
	{
		if(aa[6+i] != bb[6+i])
		{
			return aa[6+i] - bb[6+i];
		}
	}

	return 0; // equal
}

static int unit_test_ref()
{
	const char* src = FILE_SRC;
	const char* dst = FILE_DST;

	String UnsortedSrcFileName(src);
	String SortedDstFileName(dst);

	// read unsorted file -> write to sorted file
	File SrcFile, DstFile;
	uchar *Data;
	uint64_t SrcLen;
	double elapsed;

	if ( !SrcFile.Open( UnsortedSrcFileName.c_str(), "rb") )
		return 0;

	SrcLen = SrcFile.GetSize();

	if((SrcLen % ADV_SIZE) != 0)
		return 0;

	if(!DstFile.Open(SortedDstFileName, "wb"))
		return 0;

	Data = new uchar[SrcLen];	assert( Data );

	elapsed = TestSpeed(1);
	SrcFile.Read(Data, SrcLen);
	elapsed = TestSpeed(0);

	TRACE_DEBUG("Time for reading = %f msec.\n", elapsed);

	elapsed = TestSpeed(1);
	::qsort( (void*)Data, SrcLen/ADV_SIZE, ADV_SIZE, ref_compare );
	elapsed = TestSpeed(0);

	TRACE_DEBUG( "Time for sorting = %f msec.\n", elapsed );

	elapsed = TestSpeed(1);
	DstFile.Write(Data, SrcLen);
	elapsed = TestSpeed(0);

	TRACE_DEBUG("Time for writing = %f msec.\n", elapsed );

	// free buffer
	KILLARRAY( Data );
}

int unit_test_sort_file()
{
#if 0
	return unit_test_ref();
#else
	const char* src = FILE_SRC;
	const char* dst = FILE_DST;

	FILE* fi, *fo;
	PCRTT::Chunk_v buffer;
	std::vector<uchar> vuchar;
	uchar* data;
	ulong i, offset=0;
	const ulong size = File::Size(src);
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

	vuchar.resize(size);
	printf("vuchar.size: %d\n", vuchar.size() );
	data = vuchar.data();
	buffer.reserve( total );

	TRACE_DEBUG("Number of chunk to process: %d", total );

	elapsed = TestSpeed(1);
	int len = fread( data, 1, size, fi );
	printf("len: %d size: %d\n", len, size );
	assert(len == size);
	fclose(fi);

	for(i = 0; i < total; ++i)
	{
		Chunk_t t;

		t.size = ADV_SIZE;
		t.scmp = ADV_SIZE / 2;
		t.offset = ADV_SIZE / 2;
		t.data = &data[offset]; // new uchar[ADV_SIZE];
		offset += ADV_SIZE;

		//len = fread( t.data, 1, ADV_SIZE, fi );
		//assert(len == ADV_SIZE);

		buffer.push_back( t );
	}
	elapsed = TestSpeed(0);

	TRACE_DEBUG("Time for reading = %f msec.\n", elapsed);

	elapsed = TestSpeed(1);
	double st = sort( buffer, 8, total );
	elapsed = TestSpeed(0);

	TRACE_DEBUG( "Time for sorting = %f msec (st: %f).\n", elapsed,st );

	elapsed = TestSpeed(1);
	for(i = 0; i < total; ++i)
	{
		int len = fwrite( buffer[i].data, 1, ADV_SIZE, fo );
		assert(len == ADV_SIZE);
		//KILLARRAY(buffer[i].data);
	}
	elapsed = TestSpeed(0);
	fclose(fo);

	TRACE_DEBUG("Time for writing = %f msec (id: %d).\n", elapsed, _id);
#endif //
	return 1;
}


} // end of namespace PCRTT
