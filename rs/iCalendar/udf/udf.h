/*
	Reserva de Salas
	Copyright 2007 Gianfranco Rossi.

	Este programa é software livre; você pode redistribuí-lo e/ou
	modificá-lo sob os termos da Licença Pública Geral GNU, conforme
	publicada pela Free Software Foundation; tanto a versão 2 da
	Licença.

	Este programa é distribuído na expectativa de ser útil, mas SEM
	QUALQUER GARANTIA; sem mesmo a garantia implícita de
	COMERCIALIZAÇÃO ou de ADEQUAÇÃO A QUALQUER PROPÓSITO EM
	PARTICULAR. Consulte a Licença Pública Geral GNU para obter mais
	detalhes.

	Você deve ter recebido uma cópia da Licença Pública Geral GNU
	junto com este programa; se não, escreva para a Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
	02111-1307, USA.
 */

#ifndef __INCLUDE_UDF_H
#define __INCLUDE_UDF_H

#include <ptypes.h>
#include <pstreams.h>
#include <ptime.h>

#include <ibase.h>
#include <ib_util.h>

#include <stdlib.h>
#include <time.h>

#ifdef MT
	#define STATIC_MT 
	#define RWLOCK_READ scoperead __readlock( m_rwlock )
	#define RWLOCK_WRITE scopewrite __writelock( m_rwlock )
#else
	#define STATIC_MT static
	#define RWLOCK_READ 
	#define RWLOCK_WRITE 
#endif

USING_PTYPES;

#include "utils.h"

#endif // __INCLUDE_UDF_H
