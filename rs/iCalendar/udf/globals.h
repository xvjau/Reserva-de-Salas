/***************************************************************************
 *   Copyright (C) 2007 by Gianni Rossi   *
 *   gianni.rossi@gmail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef GLOBALS_H
#define GLOBALS_H


#ifdef MT
#define STATIC_MT 
#define RWLOCK_READ scoperead __readlock( m_rwlock )
#define RWLOCK_WRITE scopewrite __writelock( m_rwlock )
#else
#define STATIC_MT static
#define RWLOCK_READ 
#define RWLOCK_WRITE 
#endif

#include <vmime/vmime.hpp>

#ifdef DEBUG
#include <iostream>
#include <fstream>

extern std::ofstream logFile;

std::ostream& operator<<(std::ostream& os, const vmime::exception& e);
#endif


extern vmime::charset g_charset;

#endif // GLOBALS_H
