/*
	Reserva de Salas
	Copyright 2006 Gianfranco Rossi.

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

#include "CNotification.h"
#include <QtCore/QTimer>

CNotification::CNotification( QObject * _parent ):
	QObject( _parent ),
	m_ignoreCount(0)
{
	// Async Events got deprecated in IBPP
	#ifndef __ASYNC_EVENTS
	m_timer.setInterval(500);
	if (! connect(&m_timer, SIGNAL(timeout()), this, SLOT(onTimer())))
		throw -1;
		
	m_timer.start();
	#endif
}

void CNotification::ibppEventHandler(IBPP::Events _events,
	const std::string& _eventName, int _count)
{
	#ifdef __ASYNC_EVENTS
	{ //  <-Scope delimiter for MutexLocker
		QMutexLocker locker(&m_mutex);
	#endif
		if (m_ignoreCount)
		{
			--m_ignoreCount;
			return;
		}
	#ifdef __ASYNC_EVENTS
	} //  <-End scope delimiter for MutexLocker
	#endif
	
	FBEventType type = FBEUnknown;
	
	if (_eventName == "reserva_ins")
		type = FBEInsert;
	else if (_eventName == "reserva_upd")
		type = FBEUpdate;
	else if (_eventName == "reserva_del")
		type = FBEDelete;
	
	emit FBEvent( type );
}

void CNotification::incIgnoreCount()
{
	#ifdef __ASYNC_EVENTS
	QMutexLocker locker(&m_mutex);
	#endif
	++m_ignoreCount;
}


void CNotification::onTimer()
{
	#ifndef __ASYNC_EVENTS
	m_event->Dispatch();
	#endif
}
