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
 
#ifndef SMTP_CONFIG_H
#define SMTP_CONFIG_H

#include "udf.h"

/**
	@author Gianni Rossi <gianni.rossi@gmail.com>
*/
class SMTP_Config
{
	private:
		SMTP_Config();

		~SMTP_Config();

#ifdef MT
		rwlock		m_rwlock;
#endif
		string		m_host;
		int			m_port;
		string		m_from;
		bool		m_auth;
		bool		m_tls;
		string		m_userName;
		string		m_password;
		string		m_certificatePassword;
		
	public:
		static SMTP_Config * config();
		
		void setHost( string value )
		{
			RWLOCK_WRITE;
			m_host = value; 
		}
		
		string host()
		{
			RWLOCK_READ;
			return m_host;
		}
		
		void setPort ( int theValue )
		{
			RWLOCK_WRITE;
			m_port = theValue;
		}
	
		int port()
		{
			RWLOCK_READ;
			return m_port;
		}
		
		void setFrom( string value )
		{
			RWLOCK_WRITE;
			m_from = value;
		}
		
		string from()
		{
			RWLOCK_READ;
			return m_from;
		}

		void setAuth ( bool theValue )
		{
			RWLOCK_WRITE;
			m_auth = theValue;
		}
		
	
		bool auth()
		{
			RWLOCK_READ;
			return m_auth;
		}

		void setTls ( bool theValue )
		{
			RWLOCK_WRITE;
			m_tls = theValue;
		}
		
	
		bool tls()
		{
			RWLOCK_READ;
			return m_tls;
		}

		void setCertificatePassword ( const string& theValue )
		{
			RWLOCK_WRITE;
			m_certificatePassword = theValue;
		}
			
		string certificatePassword()
		{
			RWLOCK_READ;
			return m_certificatePassword;
		}	

		void setUserName ( const string& theValue )
		{
			RWLOCK_WRITE;
			m_userName = theValue;
		}
	
		string userName()
		{
			RWLOCK_READ;
			return m_userName;
		}
	
		void setPassword ( const string& theValue )
		{
			RWLOCK_WRITE;
			m_password = theValue;
		}

		string password()
		{
			RWLOCK_READ;
			return m_password;
		}
};

extern "C" 
{
	extern int set_smtp_host( char * value );
	extern int set_smtp_from( char * value );
	
	extern int set_smtp_auth( int * value );
	extern int set_smtp_tls( int * value );
	
	extern int set_smtp_user_name( char * value );
	extern int set_smtp_password( char * value );
}
#endif
