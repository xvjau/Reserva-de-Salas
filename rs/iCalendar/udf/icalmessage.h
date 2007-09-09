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
#ifndef ICALMESSAGE_H
#define ICALMESSAGE_H

#include "globals.h"

/**
	@author Gianni Rossi <gianni.rossi@gmail.com>
*/
class ICalMessage
{
	friend class SMTPSend;
	
	public:
		ICalMessage();
		~ICalMessage();
		
		typedef enum eoperations
		{
			oprCreate = 0,
			oprModify,
			oprCancel
		} Operations;
		
	private:
		tm				m_startTime;
		tm				m_endTime;
		std::string		m_subject;
		std::string		m_messageBody;
		std::string		m_sender;
		std::string		m_recipient;
		std::string		m_location;
		std::string		m_uid;
		Operations	m_operation;
	
	protected:
		vmime::ref <vmime::message> getMessageBody() const;
		
	public:
		void setStartTime ( const tm& theValue )
		{
			m_startTime = theValue;
		}
	
		tm startTime() const
		{
			return m_startTime;
		}

		void setEndTime ( const tm& theValue )
		{
			m_endTime = theValue;
		}
		
		tm endTime() const
		{
			return m_endTime;
		}
	
		void setSubject ( const std::string& theValue )
		{
			m_subject = theValue;
		}
		
		std::string subject() const
		{
			return m_subject;
		}
	
		void setSender ( const std::string& theValue )
		{
			m_sender = theValue;
		}
	
		std::string sender() const
		{
			return m_sender;
		}
	
		void setRecipient ( const std::string& theValue )
		{
			m_recipient = theValue;
		}
		
		std::string recipient() const
		{
			return m_recipient;
		}

		void setOperation ( const Operations& theValue )
		{
			m_operation = theValue;
		}
		
		Operations operation() const
		{
			return m_operation;
		}

		void setMessageBody ( const std::string& theValue )
		{
			m_messageBody = theValue;
		}

		std::string messageBody() const
		{
			return m_messageBody;
		}

		void setLocation ( const std::string& theValue )
		{
			m_location = theValue;
		}
		
		std::string location() const
		{
			return m_location;
		}

		void setUid ( const std::string& theValue )
		{
			m_uid = theValue;
		}
			
		std::string uid() const
		{
			return m_uid;
		}
};

#endif
