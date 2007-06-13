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
 
#include "mailqueue.h"
#include "smtp.h"

#define MAX_THREAD 3

typedef enum { msgQuit, msgMail } msgType;

struct mailMessage: public message
{
	public:
		mailMessage( msgType id ):
			message( id ) {};
		
		string to;
		string subject;
		string body;
		string eventType;
		string eventText;
};

class mailThread;

static jobqueue 	mailQueue;
static mutex 		lockThreadList;
static tpodlist<mailThread*> threadList;

class mailThread: public thread
{
	private:
		mailThread():
				thread( true ) // autofree
		{
		};
		
		~mailThread()
		{
			scopelock lock( lockThreadList );
			
			for( int i = 0; i < threadList.get_count(); ++i )
			{
				if ( threadList[ i ] == this )
				{
					threadList.del( i );
					break;
				}
			}
		}

	public:		
		/*
		 Check if the MAX_THREADs limit is reached.  If so, let a thread handle the request, if not
		 create a new thread.
		*/	
		static void checkThreads()
		{
			scopelock lock( lockThreadList );
		
			if ( threadList.get_count() < MAX_THREAD )
			{
				mailThread * th = new mailThread;
				threadList.add( th );
				
				th->start();
			}
		}
		
	protected:		
		virtual void execute()
		{
			mailMessage * message;
			bool quit = false;
			
			while ( ! quit )
			{
				message = static_cast<mailMessage*>( mailQueue.getmessage( 200 ) );
				
				if ( message )
				{
					switch( message->id )
					{
						case msgQuit: 
						{
							//m_quit = true;
							break;
						}
							
						case msgMail:
						{
							string result = sendMail ( message->to, message->subject, message->body, message->eventType, message->eventText );
							
							logfile f("/tmp/rs.log");

							f.set_bufsize(1024);             // the default value in this version is 8192

							try 
							{
								f.open();
								f.put( isoDate( now() ) + ": " + result + "\n");
								f.close();
							}
							catch (estream* e) 
							{
								perr.putf("File error: %s\n", e->get_message());
								delete e;
							}
							
							break;
						}
					}
				}
				else
					quit = true;
			};
		}
};

int enqueueMail( string to, string subject, string messageBody, string eventType, string eventText )
{
	mailMessage * message = new mailMessage( msgMail );
	
	message->to				= to;
	message->subject		= subject;
	message->body			= messageBody;
	message->eventType 		= eventType;
	message->eventText 		= eventText;
	
	mailQueue.post( message );
	
	mailThread::checkThreads();
}
