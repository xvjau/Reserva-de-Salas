#ifndef __INCLUDE_CNOTIFICATION_H
#define __INCLUDE_CNOTIFICATION_H

#include <QtCore/QObject>
#include "ibpp.h"
#include <string>

#ifdef __ASYNC_EVENTS
#include <QtCore/QMutex>
#else
#include <QtCore/QTimer>
#endif

class CNotification: public QObject, public IBPP::EventInterface
{
	Q_OBJECT

	public:
        CNotification();
        
        enum FBEventType {FBEUnknown, FBEInsert, FBEUpdate, FBEDelete};
	    void ibppEventHandler(IBPP::Events _events, const std::string& _eventName, int _count);
        
		void incIgnoreCount();
		
	private:
        #ifdef __ASYNC_EVENTS
        QMutex		m_mutex;
		#else
		QTimer      m_timer;
		IBPP::Events m_event;
		#endif
		int 		m_ignoreCount;

    #ifndef __ASYNC_EVENTS
    public:
        void setEvents(IBPP::Events _event) {m_event = _event;};
    #endif
	public slots:
		void onTimer();
	
	signals:
		void FBEvent(int event, int count);
};

#endif // __INCLUDE_CNOTIFICATION_H
