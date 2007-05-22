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

/* 
   Example from http://tools.ietf.org/html/rfc2447

   From: sman@netscape.com
   To: stevesil@microsoft.com
   Subject: Phone Conference
   Mime-Version: 1.0
   Content-Type:text/calendar; method=REQUEST; charset=US-ASCII
   Content-Transfer-Encoding: 7bit

   BEGIN:VCALENDAR
   PRODID:-//ACME/DesktopCalendar//EN
   METHOD:REQUEST
   VERSION:2.0
   BEGIN:VEVENT
   ORGANIZER:mailto:sman@netscape.com
   ATTENDEE;ROLE=CHAIR;ATTSTAT=ACCEPTED:mailto:sman@netscape.com
   ATTENDEE;RSVP=YES:mailto:stevesil@microsoft.com
   DTSTAMP:19970611T190000Z
   DTSTART:19970701T210000Z
   DTEND:19970701T230000Z
   SUMMARY:Phone Conference
   DESCRIPTION:Please review the attached document.
   UID:calsvr.example.com-873970198738777
   ATTACH:ftp://ftp.bar.com/pub/docs/foo.doc
   STATUS:CONFIRMED
   END:VEVENT
   END:VCALENDAR
*/

#include <ptypes.h>
#include <pstreams.h>
#include <ptime.h>

#include <ibase.h>
#include <ib_util.h>

#include <stdlib.h>
#include <time.h>

#include <libesmtp.h>
#include <auth-client.h>

#ifdef MT
	#define STATIC_MT 
	#define RWLOCK_READ scoperead( m_rwlock )
	#define RWLOCK_WRITE scopewrite( m_rwlock )
#else
	#define STATIC_MT static
	#define RWLOCK_READ 
	#define RWLOCK_WRITE 
#endif

USING_PTYPES;

class smtpConfig_c 
{
	private:
#ifdef MT
		rwlock		m_rwlock;
#endif
		string		m_host;
		
	public:
		void setHost( string value )
		{
			RWLOCK_WRITE;
			m_host = value; 
		}
		
		string host() const
		{
			RWLOCK_READ;
			return m_host;
		}
		
		
};

static smtpConfig_c smtpConfig;

datetime IsctstoDateTime( ISC_TIMESTAMP * ts )
{
	STATIC_MT tm time;
	isc_decode_timestamp( ts, &time );
	
	return encodedate( time.tm_year + 1900, time.tm_mon, time.tm_mday )
			+ encodetime( time.tm_hour, time.tm_min, time.tm_sec );
}

char * stringToChar( string s )
{
	STATIC_MT char *result, *p;
	STATIC_MT const char *q;
	static const int MAX_LENGTH = 256;

#ifdef MT
	result = static_cast<char*>( ib_util_malloc( MAX_LENGTH ));
#else
	static char buffer[ MAX_LENGTH ];
	result = buffer;
#endif
	
	if ( length( s ) > MAX_LENGTH )
		setlength( s, MAX_LENGTH );
	
	p = result;
	q = s;
	
	while ( *q )
		*p++ = *q++;

	// Copy the terminating 0
	*p = *q;
	
	return result;
}

string strReplace( string str, string frm, string to )
{
	return str;
}

string intToString( int value, int digits = 0 )
{
	static const int len = 48;
	STATIC_MT char buffer[len];
	
	STATIC_MT char * p = buffer + len - 1;
	*p-- = 0;
	
	if ( digits > ( len - 1))
		digits = len - 1;
	
	if ( value )
	{
		while ( value && ( p != ( buffer - 1 )))
		{
			*p-- = '0' + ( value - (( value / 10 ) * 10));
			value /= 10;
			digits--;
		}
	}
	else
	{
		*p-- = '0';
		digits--;
	}
	
	while ( digits-- )
		*p-- = '0';
	
	string result = ++p;
	
	return result;
}

string isoDate( datetime dt )
{
	STATIC_MT int year, month, day, hours, mins, secs, msecs;
	
	decodedate( dt, year, month, day );
	decodetime( dt, hours, mins, secs, msecs );

	string result;
	
	result = intToString( year, 4 ) + intToString( month, 2 ) + intToString( day, 2 )
			+ 'T' + intToString( hours, 2 ) + intToString( mins, 2 ) + intToString( secs, 2 );
	
	return result;
}

string BlobToString( BLOBCALLBACK blob )
{
	STATIC_MT string result;
#ifdef MT
	result.clear();
#endif
	
	if ( blob && blob->blob_handle )
	{
		int len = blob->blob_max_segment + 1L;
		ISC_UCHAR * buffer = static_cast<ISC_UCHAR*>( malloc( len ) );
		
		ISC_USHORT readLength = 0;

		while ( (*blob->blob_get_segment) ( blob->blob_handle, buffer, len, &readLength ) )
		{
			buffer[ readLength ] = 0;
			
			// I know, the double cast looks ugly; but it works!
			result += static_cast<char*>( static_cast<void*>( buffer ));
		}

		delete buffer;
	}

	return result;
}

string sendMail( string data )
{
	smtp_session_t session;
	smtp_message_t message;
	smtp_recipient_t recipient;
	auth_context_t authctx;
	
	auth_client_init ();
	session = smtp_create_session ();
	message = smtp_add_message (session);
	
	
	smtp_starttls_enable (session, Starttls_ENABLED);
	smtp_starttls_enable (session, Starttls_REQUIRED);
	
	smtp_set_server( session, smtpConfig.host() );
}

extern "C" 
{
	
void set_stmp( )
{
}

extern char * icalendar( char * uid, char * from, char * to, char * subject, BLOBCALLBACK description, 
		char * location, ISC_TIMESTAMP * tsStart, ISC_TIMESTAMP * tsEnd, 
		int * opr )
{
	string method, status;

	switch ( *opr )
	{
		case 0: // Insert
		case 1: // Update 
			method = "PUBLISH";
			status = "CONFIRMED";
			break;

		case 2: // Delete
			method = "CANCEL";
			status = "CANCELLED";
			break;

		default: return stringToChar( "Illegal opr code" ); // Shouldn't happen!
	}

	datetime dtStart = IsctstoDateTime( tsStart ),
		dtEnd = IsctstoDateTime( tsEnd );
	
	string descr = BlobToString( description );

	descr = strReplace( descr, "\x0D\x0A", "\\n\n  " );
	descr = strReplace( descr, "\x0A", "\\n\n  " );
	descr = strReplace( descr, "\x0D", "\\n\n  " );

	string str;

	str = string( "From: " ) + from + '\n' +
		"To: "  + to + '\n' +
		"Subject: "  + subject + '\n' +
		"Mime-Version: 1.0\n"  +
		"Content-Type:text/calendar; method="  + method + "; charset=US-ASCII\n"  +
		"Content-Transfer-Encoding: 7bit\n\n"  + 
		
		"BEGIN:VCALENDAR\n"  +
		"METHOD:"  + method + '\n' +
		"VERSION:2.0\n"  +
		"BEGIN:VEVENT\n"  +
		"ORGANIZER:mailto:"  + from + '\n' +
		"ATTENDEE;RSVP=FALSE\n"  +
		"DTSTAMP:"  + isoDate( now() )  + '\n' +
		"DTSTART:"  + isoDate( dtStart ) + '\n' +
		"DTEND:"  + isoDate( dtEnd )  + '\n' +
		"SUMMARY:"  + subject + '\n' +
		"DESCRIPTION:"  + descr + '\n' +
		"LOCATION:" + location + '\n' +
		"UID:"  + uid + '\n' +
		"STATUS:"  + status + '\n' +
		"END:VEVENT\n"  +
		"END:VCALENDAR\n";

	outfile file("/tmp/ical.txt");
	file.open( );

	file.put( str );
	file.put( "\n" );

	//string result = sendMail( str );
	string result = "Nope!";
			
	file.put( result );
	file.put( "\n\n" );
	
	return stringToChar( result );
}

} // extern "C"
