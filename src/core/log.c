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

#if defined(__DEBUG__)
static const char filename[] = "/tmp/pcrtt.log";

static inline FILE* olog()
{
	FILE* fp = fopen(filename, "a+");
	return fp;
}

#endif // __DEBUG__

static const char* log_level[] = {
	"FATAL",	// 0
	"ERROR",	// 1
	"WARNING",	// 2
	"INFO",		// 3
	"DEBUG",	// 4
};

static const char* get_lvl_code(const int lvl)
{
	if ( lvl < 0 || lvl > 4 )
		return log_level[1/* error */];

	return log_level[lvl];
}

#if defined(__DEBUG__)
const char* getSrcOnly(const char* filename)
{
	const char * file = strrchr(filename, '/');
	if (file != 0)
		file++;
	else
	{
		file = strrchr(filename, '\\');
		if (file != 0)
			file++;
		else
			file = filename;
	}
	return file;
}
#endif // __DEBUG__

void trace_log(const int lvl, const char* file, const int line,
	const char* buffer,...)
{
	va_list argList;
#if defined(__DEBUG__)
	char* str=0;
	FILE* fp;

	if(!buffer)
		return;

	int sz = 8192;
	str = (char*)malloc( sz );

	va_start(argList, buffer);
	vsnprintf(str, sz-1, buffer, argList);
	va_end(argList);

	const char* f = getSrcOnly(file);

	const char* level_str = get_lvl_code(lvl);

	printf("%s %s @ %d : %s\n", level_str, f, line, str );

	fp = olog();

	if(!olog)
	{
		free(str);
		return;
	}

	fprintf( fp, "%s %s @ %d : %s\n", level_str, f, line, str );
	fclose(fp);
	free(str);
#else
	const char* level_str = get_lvl_code(lvl);
	printf("%s ", level_str );
	
	va_start(argList, buffer);
	vprintf(buffer, argList);
	va_end(argList);
#endif // __DEBUG__
}

void applog_(const char* file, const int line,const int lvl,
	const char* buffer, ...)
{
	va_list argList;
	if(!buffer) return;

	const char* level_str = get_lvl_code(lvl);
	printf("%s ", level_str );

	va_start(argList, buffer);
	vprintf(buffer, argList);
	va_end(argList);
}
