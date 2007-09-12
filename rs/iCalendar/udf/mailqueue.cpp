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
#ifdef MT
 
#include "mailqueue.h"
#include "smtpsend.h"
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <list>

static boost::thread			*mailQueue = 0;
static boost::mutex				mailMutex;
static boost::condition			waitCond;

typedef std::list<ICalMessage*>	ICalMessageList;
static ICalMessageList			mailList;

static bool						terminateQueue = false;

#define SCOPE_LOCK boost::mutex::scoped_lock __MUTEX_LOCKER(mailMutex)

void mailQueueLoop()
{
	waitCond.notify_one();
	
	while (!terminateQueue)
	{
		{
			SCOPE_LOCK;
			
			waitCond.wait(__MUTEX_LOCKER);
			
			ICalMessageList::const_iterator it;
			for(it = mailList.begin(); it != mailList.end(); ++it)
			{
				SMTPSend sender(&g_config);
				sender.send( *it );
				delete *it;
			}
			mailList.clear();
		}
	}
}

void enqueueMail( ICalMessage *message )
{
	SCOPE_LOCK;
	
	if ( !mailQueue )
	{
		mailQueue = new boost::thread(mailQueueLoop);
		waitCond.wait(__MUTEX_LOCKER);  // Wait for the thread to initialize
	}
	
	mailList.push_back( message );
	waitCond.notify_one();
}
#endif // MT
