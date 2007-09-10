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
#ifndef SMTPCONFIG_H
#define SMTPCONFIG_H

#include "globals.h"

/**
	@author Gianni Rossi <gianni.rossi@gmail.com>
*/
using namespace std;

class SMTPConfig
{
	public:
		SMTPConfig ();
		virtual ~SMTPConfig();	

	private:
		std::string	m_host;
		int			m_port;
		std::string	m_userName;
		std::string	m_password;
		bool		m_smtpAuth;
		bool		m_smtpSSL;
		
	public:
		void setHost ( const std::string& theValue )
		{
			m_host = theValue;
		}
	
		std::string host() const
		{
			return m_host;
		}

		void setPort ( int theValue )
		{
			m_port = theValue;
		}
		
		int port() const
		{
			return m_port;
		}
	
		void setUserName ( const std::string& theValue )
		{
			m_userName = theValue;
		}
		
		std::string userName() const
		{
			return m_userName;
		}
	
		void setPassword ( const std::string& theValue )
		{
			m_password = theValue;
		}

		std::string password() const
		{
			return m_password;
		}

		void setSmtpAuth ( bool theValue )
		{
			m_smtpAuth = theValue;
		}
		
		bool smtpAuth() const
		{
			return m_smtpAuth;
		}
	
		void setSmtpSSL ( bool theValue )
		{
			m_smtpSSL = theValue;
		}
		
		bool smtpSSL() const
		{
			return m_smtpSSL;
		}
};

#endif
