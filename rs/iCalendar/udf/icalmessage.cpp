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
#include "icalmessage.h"
#include <boost/algorithm/string/replace.hpp>

/*
	BEGIN:VCALENDAR
	PRODID:-//Google Inc//Google Calendar 70.9054//EN
	VERSION:2.0
	CALSCALE:GREGORIAN
	METHOD:CANCEL
	BEGIN:VEVENT
	DTSTART:20070917T153000Z
	DTEND:20070917T170000Z
	DTSTAMP:20070917T135424Z
	ORGANIZER;CN=Juliana Nakagawa:MAILTO:julianan@google.com
	UID:6t133or0lptta4411ocjps9b80@google.com
	ATTENDEE;CUTYPE=INDIVIDUAL;ROLE=REQ-PARTICIPANT;PARTSTAT=DECLINED;CN=Alan G
	range;X-NUM-GUESTS=0:MAILTO:grange@mindsearch.com.br
	CLASS:PRIVATE
	CREATED:20070913T192351Z
	LAST-MODIFIED:20070917T135424Z
	LOCATION:(Misc)-SAO-BRI-5-Cafe Table Arroz (4)
	SEQUENCE:2
	STATUS:CANCELLED
	SUMMARY:lunch - Alan
	TRANSP:OPAQUE
	END:VEVENT
	END:VCALENDAR
*/

ICalMessage::ICalMessage()
{}


ICalMessage::~ICalMessage()
{}

std::string intToString( int value, int digits )
{
	static const int len = 48;
	STATIC_MT char buffer[len];
	STATIC_MT char * p;
	
	p = buffer + len - 1;
	*p-- = 0;
	
	if ( digits > ( len - 1))
		digits = len - 1;
	
	if ( value )
	{
		while ( value && ( p != buffer ))
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
	
	while (( --digits >= 0 ) && ( p != buffer ))
		*p-- = '0';
	
	std::string result = ++p;
	
	return result;
}

std::string isoDate( const tm dt )
{
	std::string result = intToString( dt.tm_year + 1900, 4 ) + 
			intToString( dt.tm_mon + 1, 2 ) + 
			intToString( dt.tm_mday, 2 ) 
			+ 'T' + 
			intToString( dt.tm_hour, 2 ) + 
			intToString( dt.tm_min, 2 ) + 
			intToString( dt.tm_sec, 2 );
	
	return result;
}

std::string isoDate( const time_t dt )
{
	tm time;
	gmtime_r(&dt, &time);	
	return isoDate( time );
}

vmime::ref <vmime::message> ICalMessage::getMessageBody() const
{
	std::string method, status;

	switch ( m_operation )
	{
		case oprCreate:
		case oprModify:
			method = "PUBLISH";
			status = "CONFIRMED";
			break;

		case oprCancel: // Delete
			method = "CANCEL";
			status = "CANCELLED";
			break;

		default: throw std::string( "Illegal opr code" ); // Shouldn't happen!
	}

	std::string message = m_messageBody;
	{
		using namespace boost::algorithm;
		
		replace_all( message, "\x0D\x0A", "\\n\n  " );
		replace_all( message, "\x0A", "\\n\n  " );
		replace_all( message, "\x0D", "\\n\n  " );
	}

	std::string strEvent, strBody;

	strEvent = std::string ( "BEGIN:VCALENDAR\n" ) +
				"PRODID:-//RolTramInfo//Reservasr de Salas 0.0.1//BR\n"  +
				"VERSION:2.0\n"  +
				"CALSCALE:GREGORIAN\n"  +
				"METHOD:"  + method + '\n' +
				"BEGIN:VEVENT\n"  +
				"ORGANIZER:MAILTO:" + m_recipient + '\n' +
				"ATTENDEE;RSVP=FALSE\n"  +
				"DTSTAMP:"  + isoDate ( time(0) )  + '\n' +
				"DTSTART:"  + isoDate ( m_startTime ) + '\n' +
				"DTEND:"  + isoDate ( m_endTime )  + '\n' +
				"SUMMARY:"  + m_subject + '\n' +
				"DESCRIPTION:"  + message + '\n' +
				"LOCATION:" + m_location + '\n' +
				"UID: <"  + m_uid + "> \n" +
				"SEQUENCE:1\n"  +
				"STATUS:"  + status + '\n' +
				"TRANSP:OPAQUE\n"  +
				"END:VEVENT\n"  +
				"END:VCALENDAR\n";
	
	strBody = std::string ( "You have been invited to a meeting:\n" ) +
					"Location: " + m_location + '\n' +
					"Date/Time: " + isoDate ( m_startTime ) + '\n' +
					"Subject: " + m_subject + '\n' +
					'\n' + m_messageBody;
	
	try
	{
		vmime::messageBuilder	messageBuilder;
		
		vmime::addressList destAddresses;
		destAddresses.appendAddress(vmime::create <vmime::mailbox>(m_recipient.c_str()));
		
		vmime::text txtSubject;
		txtSubject.createFromString(m_subject.c_str(), g_charset);
		
		// Fill in some header fields and message body
		messageBuilder.setExpeditor(vmime::mailbox(m_sender.c_str()));
		messageBuilder.setRecipients(destAddresses);
		messageBuilder.setSubject(vmime::text(m_subject.c_str()));

		messageBuilder.getTextPart()->setCharset(vmime::charsets::UTF_8);
		messageBuilder.getTextPart()->setText(vmime::create <vmime::stringContentHandler>(strBody.c_str()));
		
		std::string strType = "text/calendar; method=" + method + "; charset=utf-8";
		
		vmime::mediaType mtype = vmime::mediaType(strType.c_str());
		
		vmime::ref <vmime::stringContentHandler> icalattach = vmime::create <vmime::stringContentHandler>(strEvent.c_str());
				
		vmime::ref <vmime::defaultAttachment> attach = 
				vmime::create <vmime::defaultAttachment>(icalattach,
															mtype,
															vmime::text(""),
															vmime::word("reserva.ics"));
		
		messageBuilder.appendAttachment( attach );
		
		// Construction
		vmime::ref <vmime::message> msg = messageBuilder.construct();

		// Raw text generation
		vmime::string dataToSend = msg->generate();
		
		return msg;
	}
	catch (vmime::exception& e)
	{
		//std::cerr << e;         // VMime exception
	}
	
	return 0;
}


