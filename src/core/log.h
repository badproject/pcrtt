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
#if !defined( PCRTT_SRC_LOG_HEADER_H_ )
#define PCRTT_SRC_LOG_HEADER_H_

/** very simple log system, not thread safe */
#define LOG_FATAL	0
#define LOG_ERR		1
#define LOG_WARNING	2
#define LOG_INFO	3
#define LOG_DEBUG	4


#define TRACE_ERROR(...)   trace_log(LOG_ERR,__FILE__,__LINE__,__VA_ARGS__)
#define TRACE_WARNING(...) trace_log(LOG_WARNING,__FILE__,__LINE__,__VA_ARGS__)
#define TRACE_FATAL(...)   trace_log(LOG_FATAL,__FILE__,__LINE__,__VA_ARGS__)
#define TRACE_DEBUG(...)   trace_log(LOG_DEBUG,__FILE__,__LINE__,__VA_ARGS__)
#define TRACE_INFO(...)    trace_log(LOG_INFO,__FILE__,__LINE__,__VA_ARGS__)

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void trace_log(const int lvl, const char* file, const int line,
	const char* buffer, ...);

#define applog(...) applog_(__FILE__,__LINE__,__VA_ARGS__)


void applog_(const char* file, const int line,const int lvl,
	const char* buffer, ...);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // PCRTT_SRC_LOG_HEADER_H_
