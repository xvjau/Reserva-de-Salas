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

#include "udf.h"
#include "smtp_config.h"
#include "smtp.h"

extern "C" 
{
		
extern char * icalendar( char * uid, char * to, char * subject, BLOBCALLBACK description, char * location, ISC_TIMESTAMP * tsStart, ISC_TIMESTAMP * tsEnd, int * opr )
{
	static const string contentType = "text/calendar; method=REQUEST; charset=US-ASCII";
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

	SMTP_Config * config = SMTP_Config::config();
	
	str = string( "BEGIN:VCALENDAR\n" ) + 
		"METHOD:"  + method + '\n' +
		"VERSION:2.0\n"  +
		"BEGIN:VEVENT\n"  +
		"ORGANIZER:mailto:" + config->from() + '\n' +
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

	string result = sendMail( to, subject, contentType, str );
	
	return stringToChar( result );
	//return stringToChar( str );
}

} // extern "C"
