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
#if !defined(PCRTT_SRC_PCRTT_HEADER_H_)
#define PCRTT_SRC_PCRTT_HEADER_H_

#if !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif // _GNU_SOURCE

#include "config.h"

/** C header */

#if defined(HAVE_STDIO_H)
#include <stdio.h>
#else
#error no stdio.h found
#endif // HAVE_STDIO_H

#if defined(HAVE_STDINT_H)
#include <stdint.h>
#else
#error no stdint.h found
#endif // HAVE_STDINT_H

#if defined(HAVE_STDLIB_H)
#include <stdlib.h>
#else
#error no stdlib.h found
#endif // HAVE_STDLIB_H

#if defined(HAVE_STDARG_H)
#include <stdarg.h>
#else
#error no stdarg.h found
#endif // HAVE_STDARG_H

#if defined(HAVE_STRING_H)
#include <string.h> // memcpy
#else
#error no string.h found
#endif // HAVE_STRING_H

#if defined(HAVE_ASSERT_H)
#include <assert.h>
#else
#error no assert.h found
#endif // HAVE_ASSERT_H

#if defined(HAVE_TIME_H)
#include <time.h>
#else
#error no time.h found
#endif // HAVE_TIME_H

#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#else
#error no unistd.h found
#endif // HAVE_UNISTD_H

#if defined(HAVE_PTHREAD_H)
#include <pthread.h>
#else
#error no pthread.h found
#endif // HAVE_PTHREAD_H

#if !defined(__WIN32__) && defined(HAVE_PTHREAD_H)
#	include <pthread.h>
#	include <semaphore.h>
#endif	// 

#if defined(HAVE_LIBCONFIG_H)
#include <libconfig.h>
#else
#error no libconfig.h found
#endif // HAVE_LIBCONFIG_H

#if defined(__NCURSES__)
#include <curses.h>
#endif // __NCURSES__

#include <unistd.h>
#include <sys/stat.h> // mkdir
#include <sys/types.h>
#include <dirent.h> // DIR
#include <sys/time.h> // timeval / gettimeofday,
#include <fcntl.h>
#include <errno.h>

/** C++ header */

#if defined(__cplusplus)
#	if defined(HAVE_IOSTREAM)
#		include <iostream>
#	endif // HAVE_IOSTREAM
#	if defined(HAVE_STRING)
#		include <string>
#	else
#		error no string found
#	endif // HAVE_STRING
#endif // __cplusplus

/** core header (C) */
#include "core/types.h"
#include "core/define.h"
#include "core/log.h"
#include "core/core.h"

/** core header (C++) */
#if defined(__cplusplus)
#include "core/vector.h"
#include "core/str.h"
#include "core/noncopyable.h"
#include "core/mutex.h"
#include "core/thread.h"
#include "core/file.h"
#include "core/rainbowtab.h"
#include "worker/include.h"
#endif // __cplusplus

/** cuda header */
#if defined(__CUDA__)
#include "cuda/include.h"
#endif // __CUDA__

/** opencl header */
#if defined(__OPENCL__)
#include "opencl/include.h"
#endif // __OPENCL__

/** vulkan header */
#if defined(__VULKAN__)
#include "vulkan/include.h"
#endif // __VULKAN__

/** FLTK header */
#if defined(__FLTK__) && defined(__cplusplus)
#include "fltk/gui.h"
#endif // __FLTK__

/** GTK header */
#if defined(__GTK__) && defined(__cplusplus)
#include "gtk/gui.h"
#endif // __GTK__

/** console header */
#if defined(__CONSOLE__)
#include "core/console.h"
#endif // __CONSOLE__

#endif // PCRTT_SRC_PCRTT_HEADER_H_
