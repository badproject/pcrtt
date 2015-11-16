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
#if !defined(PCRTT_SRC_DEFINE_HEADER_H_)
#define PCRTT_SRC_DEFINE_HEADER_H_

#define M_MIN(a,b)  (((a) < (b)) ? (a) : (b))
#define M_MAX(a,b)  (((a) > (b)) ? (a) : (b))

#define KILLOBJECT(o) if ( o ) { delete o; o = 0; }
#define KILLARRAY(o) if ( o ) { delete [] o; o = 0; }

#define EMPTY_STRUCT	memset(this, 0x00, sizeof(*this) );

#define p_inline	__inline

#endif // PCRTT_SRC_DEFINE_HEADER_H_
