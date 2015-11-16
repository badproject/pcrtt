#define __CL_ENABLE_EXCEPTIONS
#if defined(__APPLE__) || defined(__MACOSX)
#include <OpenCL/cl.hpp>
#else
//#include <CL/cl.hpp>
#include "cl.hpp"
#endif
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include <pcrtt.h>
#include <shared/shared.h>

// take care if you have libdvbcsa installed
#include <dvbcsa/dvbcsa.h>
#include <dvbcsa/dvbcsa_pv.h>

#include <opencl/include.h>
#include <shared/cpu.h>

#include <opencl/csa_rainbow_a.h>

#define BIN_FILE	"csa_rainbow_a.bin"

namespace PCRTT {
namespace OpenCL {
	
static std::vector<cl::Platform> all_platforms;
static cl::Platform default_platform;
static std::vector<cl::Device> all_devices;
static cl::Device default_device;
static cl::Context context;
static cl::Program program;
static cl::CommandQueue queue;

class Fifo_t
{
public:
	Fifo_t() : buffers(), ui_var(), ul_var(), kernel() { }
	~Fifo_t() { destroy_buffer(); }

	inline void destroy_buffer()
	{
		buffers.clear();
	}

	Fifo_t& add_read_buffer(
		int nb_buffer, int size, int read_only = 0
	)
	{
		for(int i=0; i < nb_buffer; ++i)
		{
			cl::Buffer b( context, read_only ? CL_MEM_READ_ONLY : 
				CL_MEM_READ_WRITE, size );
			buffers.push_back(b);
		}
		return (*this);
	}
	
	inline Fifo_t& add_write_buffer(
		int nb_buffer, int size, int write_only = 0
	)
	{
		for(int i=0; i < nb_buffer; ++i)
		{
			cl::Buffer b( context, write_only ? CL_MEM_WRITE_ONLY : 
				CL_MEM_READ_WRITE, size );
			buffers.push_back(b);
		}
		return (*this);
	}

	inline Fifo_t& add_ui_var( uint def_value = 0 )
	{
		uint v = def_value;
		ui_var.push_back(v);
		return (*this);
	}

	inline Fifo_t& add_ul_var( ulong def_value = 0 )
	{
		ulong v = def_value;
		ul_var.push_back(v);
		return (*this);
	}

	inline Fifo_t& set_ui_var( uint id, uint value )
	{
		ui_var[id] = value;
		return (*this);
	}

	inline Fifo_t& set_ul_var( uint id, ulong value )
	{
		ul_var[id] = value;
		return (*this);
	}

	inline Fifo_t& init_kernel(const char* name)
	{
		kernel = cl::Kernel( program, name );
		return (*this);
	}

	inline Fifo_t& set_kernel()
	{
		int i;
		uint id=0;
		/* uint var */
		for( i=0; i < ui_var.size(); ++i)
		{
			kernel.setArg( id, sizeof(uint), (void *)&ui_var[i] ); id++;
		}
		/* ulong var */
		for(i=0; i < ul_var.size(); ++i)
		{
			kernel.setArg( id, sizeof(ulong), (void *)&ul_var[i] ); id++;
		}
		/* buffer */
		for( i=0; i < buffers.size(); ++i)
		{
			//kernel.setArg( id, sizeof(cl_mem), &buffers[i] ); id++;
			kernel.setArg( id, buffers[i] ); id++;
		}

		return (*this);
    }

	inline void debug(int id)
	{
		cl_mem_flags flags;
		buffers[id].getInfo( CL_MEM_FLAGS, &flags );
		TRACE_DEBUG( "flags: 0x%X - valid: %d", flags, (flags & CL_MEM_READ_ONLY ) ? 1 : 0 );

		if ( flags & CL_MEM_READ_ONLY ) TRACE_DEBUG( "flags: has CL_MEM_READ_ONLY");
		if ( flags & CL_MEM_WRITE_ONLY ) TRACE_DEBUG( "flags: has CL_MEM_WRITE_ONLY");
		if ( flags & CL_MEM_READ_WRITE  ) TRACE_DEBUG( "flags: has CL_MEM_READ_WRITE");
		if ( flags & CL_MEM_USE_HOST_PTR ) TRACE_DEBUG( "flags: has CL_MEM_USE_HOST_PTR");
		if ( flags & CL_MEM_ALLOC_HOST_PTR ) TRACE_DEBUG( "flags: has CL_MEM_ALLOC_HOST_PTR");
		if ( flags & CL_MEM_COPY_HOST_PTR ) TRACE_DEBUG( "flags: has CL_MEM_COPY_HOST_PTR");
	}

	inline Fifo_t& write( int id, const uchar *data, const uint& size )
	{
		if ( id >= 0 && id < buffers.size() )
		{
			queue.enqueueWriteBuffer( buffers[id], CL_TRUE, 0, size, data );
		}
		queue.finish();
		return (*this);
	}

	inline Fifo_t& read(int id, uchar* data, const uint& size)
	{
		queue.enqueueReadBuffer( buffers[id], CL_TRUE, 0, size, data );
		return (*this);
	}

	inline Fifo_t& start(int nb_worker)
	{
		cl::Event event;

		cl_int err = queue.enqueueNDRangeKernel(
			kernel,
			cl::NullRange,				// offset
			cl::NDRange( nb_worker ),	// global
			cl::NullRange,
			NULL,
			&event
		);

		event.wait();

		queue.finish();

		return (*this);
	}

	/** vars */
	std::vector<cl::Buffer> buffers;
	std::vector<uint> ui_var;
	std::vector<ulong> ul_var;
	
	// private
	cl::Kernel kernel;
};

static Fifo_t csa_rainbow_1;

//  Retreive program binary for all of the devices attached to the
//  program an and store the one for the device passed in
static int save_program_binary(cl::Program& program,
	cl::Device& device, const char* fileName)
{
#if 0 // finish me
	cl_uint numDevices = 0;
	cl_int err;

	// 1 - Query for number of devices attached to program
	err = program.getInfo( CL_PROGRAM_NUM_DEVICES, &numDevices );

	CHK_ERROR(err)
	// 2 - Get all of the Device IDs
	cl_device_id *devices = new cl_device_id[numDevices];
	err = clGetProgramInfo(program, CL_PROGRAM_DEVICES,
							  sizeof(cl_device_id) * numDevices,
							  devices, NULL);
   CHK_ERROR(err)

	// 3 - Determine the size of each program binary
	size_t *programBinarySizes = new size_t [numDevices];
	err = clGetProgramInfo(program, CL_PROGRAM_BINARY_SIZES,
							  sizeof(size_t) * numDevices,
							  programBinarySizes, NULL);
	 CHK_ERROR(err)

	uchar **programBinaries = new uchar*[numDevices];
	for (cl_uint i = 0; i < numDevices; i++)
	{
		programBinaries[i] = new uchar[programBinarySizes[i]];
	}

	// 4 - Get all of the program binaries
	err = clGetProgramInfo(program, CL_PROGRAM_BINARIES, sizeof(uchar*) * numDevices,
							  programBinaries, NULL);
	CHK_ERROR(err)

	// 5 - Finally store the binaries for the device requested out to disk for future reading.
	for (cl_uint i = 0; i < numDevices; i++)
	{
		// Store the binary just for the device requested.  In a scenario where
		// multiple devices were being used you would save all of the binaries out here.
		if (devices[i] == device)
		{
			FILE *fp = fopen(fileName, "wb");
			fwrite(programBinaries[i], 1, programBinarySizes[i], fp);
			fclose(fp);
			break;
		}
	}

	// Cleanup
	delete [] devices;
	delete [] programBinarySizes;
	for (cl_uint i = 0; i < numDevices; i++)
	{
		delete [] programBinaries[i];
	}
	delete [] programBinaries;
#endif // 
	return 1;
}

static int load_source_code()
{
	try
	{
		cl::Program::Sources sources;

		sources.push_back({kernel_code_src,kernel_code_size});

		program = cl::Program(context,sources);

		if ( program.build({default_device}) != CL_SUCCESS )
		{
			std::cout<<" Error building: " << 
				program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device) << "\n";
			return 0;
		}

		save_program_binary( program, default_device, BIN_FILE );
	}
	catch (cl::Error err)
	{
		std::cerr 
			<< "ERROR: " << err.what() << "(" << err.err() << ")" << std::endl;
		
		std::cout<<" Error building: " << 
			program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device) << "\n";
	}

	return 1;
}

static int load_binary_code()
{
	 int sz;
	cl::Program::Binaries sources;
	void* data = file_contents( BIN_FILE, &sz );

	sources.push_back({data,sz});

	program = cl::Program( context, all_devices, sources );

	if ( program.build({default_device}) != CL_SUCCESS )
	{
		std::cout<<" Error building: " << 
			program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device) << "\n";
		return 0;
	}
   
	return 0;
}

static int load_code()
{
	if ( !file_exists(BIN_FILE) )
	{
		return load_source_code();
	}
	return load_binary_code();
}

static int init_opencl()
{
	// get all platforms (drivers)
	cl::Platform::get(&all_platforms);

	if ( !all_platforms.size() )
	{
		std::cout<<" No platforms found. Check OpenCL installation!\n";
		return 0;
	}

	default_platform = all_platforms[0];

	std::cout << "Using platform: " << 
		default_platform.getInfo<CL_PLATFORM_NAME>() <<"\n";

	// get default device of the default platform
	default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);

	if ( !all_devices.size() )
	{
		std::cout << "No devices found. Check OpenCL installation!\n" ;
		return 0;
	}

	default_device = all_devices[0];

	std::cout<< "Using device: " << 
		default_device.getInfo<CL_DEVICE_NAME>() << "\n";

	context = cl::Context({default_device});

	if(!load_code())
	{
		std::cout << "Can't load code!\n" ;
		return 0;
	}

	/** init queue */
	queue = cl::CommandQueue( context, default_device );

	/** prepare our kernel fifo */
	const uint size = BLOCKS_RB * THREADS_RB * 6;
	csa_rainbow_1.
		add_ul_var(0).
		add_read_buffer( 1, size, 1 ). // 0
		add_write_buffer( 1, size, 1 ). // 1
		init_kernel("k_csa_rainbow_a");

	return 1;
}

/** global init */
int init()
{
	return init_opencl();
}

int destroy()
{
	return 1;
}

// ----------------------------------------------------------------------------
int CsaRainbowDoXRounds(uchar *Cw6Array_h, ulong *CurRoundNrArray_h,
	ulong *RemRoundsArray_h)
{
	return 0;
}

int CsaRainbowFindA(uchar *Cw6EndArray_h, ulong *RoundNrArray_h,
	uchar *CryptArray_h)
{
	return 0;
}

/** function OpenCL::CsaRainbow1 */

int CsaRainbow1(uchar *output, const uchar *input, ulong nb)
{
	const uint size = BLOCKS_RB * THREADS_RB;

	csa_rainbow_1.
		write( 0, input, (size*6) ).	// write to buffer offset 0
		set_ul_var( 0, nb ).			// ulong var offset 0
		set_kernel().					// set all our args to kernel
		start( size ).					// function wait to finish queue job
		read( 1, output, (size*6) );	// read buffer offset 1

	return 1;
}

} // end of namespace OpenCL
} // end of namespace PCRTT
