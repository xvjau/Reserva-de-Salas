#include "CNotification.h"
#include <QtCore/QTimer>

CNotification::CNotification():
	m_ignoreCount(0)
{
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
	
	emit FBEvent(type, _count);
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
