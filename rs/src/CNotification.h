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
        CNotification( QObject * _parent = 0 );
        
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
