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

ICalMessage::ICalMessage()
{}


ICalMessage::~ICalMessage()
{}

String strReplace(String str, String from, String to)
{
	int len = length(from), i = pos(from, str);
	
#warning FIXME  Very ugly and inefficient way of replacing strings
	while (i != -1)
	{
		del(str, i, len);
		ins(str, to, i);
		
		i = pos(from, str);
	}
	
	return str;
}

String intToString( int value, int digits )
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
	
	String result = ++p;
	
	return result;
}


String isoDate( const datetime dt )
{
	STATIC_MT int year, month, day, hours, mins, secs, msecs;
	
	decodedate( dt, year, month, day );
	decodetime( dt, hours, mins, secs, msecs );

	String result;
	
#warning FIXME Should return time in Zulu-time
	result = intToString( year, 4 ) + intToString( month, 2 ) + intToString( day, 2 )
			+ 'T' + intToString( hours, 2 ) + intToString( mins, 2 ) + intToString( secs, 2 );
	
	return result;
}

vmime::ref <vmime::message> ICalMessage::getMessageBody() const
{
	String method, status;

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

		default: throw String( "Illegal opr code" ); // Shouldn't happen!
	}

	String message = m_messageBody;
	message = strReplace( message, "\x0D\x0A", "\\n\n  " );
	message = strReplace( message, "\x0A", "\\n\n  " );
	message = strReplace( message, "\x0D", "\\n\n  " );

	String strEvent, strBody;

	strEvent = String ( "BEGIN:VCALENDAR\n" ) +
				"METHOD:"  + method + '\n' +
				"VERSION:2.0\n"  +
				"BEGIN:VEVENT\n"  +
				"ORGANIZER:mailto:" + m_recipient + '\n' +
				"ATTENDEE;RSVP=FALSE\n"  +
				"DTSTAMP:"  + isoDate ( now() )  + '\n' +
				"DTSTART:"  + isoDate ( m_startTime ) + '\n' +
				"DTEND:"  + isoDate ( m_endTime )  + '\n' +
				"SUMMARY:"  + m_subject + '\n' +
				"DESCRIPTION:"  + message + '\n' +
				"LOCATION:" + m_location + '\n' +
				"UID: <"  + m_uid + "> \n" +
				"STATUS:"  + status + '\n' +
				"END:VEVENT\n"  +
				"END:VCALENDAR\n";

	strBody = String ( "You have been invited to a meeting:\n" ) +
					"Location: " + m_location + '\n' +
					"Date/Time: " + isoDate ( m_startTime ) + '\n' +
					"Subject: " + m_subject + '\n' +
					'\n' + m_messageBody;
	
	try
	{
		vmime::messageBuilder	messageBuilder;
		
		vmime::addressList destAddresses;
		destAddresses.appendAddress(vmime::create <vmime::mailbox>(static_cast<const char*>(m_recipient)));
		
		vmime::text txtSubject;
		txtSubject.createFromString(static_cast<const char*>(m_subject), g_charset);
		
		// Fill in some header fields and message body
		messageBuilder.setExpeditor(vmime::mailbox(static_cast<const char*>(m_sender)));
		messageBuilder.setRecipients(destAddresses);
		messageBuilder.setSubject(vmime::text(static_cast<const char*>(m_subject)));

		messageBuilder.getTextPart()->setCharset(vmime::charsets::UTF_8);
		messageBuilder.getTextPart()->setText(vmime::create <vmime::stringContentHandler>(static_cast<const char*>(strBody)));
		
		String strType = "text/calendar; method=" + method + "; charset=utf-8";
		
		vmime::mediaType mtype = vmime::mediaType(static_cast<const char*>(strType));
		
		vmime::ref <vmime::stringContentHandler> icalattach = vmime::create <vmime::stringContentHandler>(static_cast<const char*>(strEvent));
				
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

		std::cout << "Generated message:" << std::endl;
		std::cout << "==================" << std::endl;
		std::cout << std::endl;
		std::cout << dataToSend << std::endl;
		
		return msg;
	}
	catch (vmime::exception& e)
	{
		std::cerr << e;         // VMime exception
	}
	
	return 0;
}


