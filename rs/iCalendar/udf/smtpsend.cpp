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
#include "smtpsend.h"

class certificateVerifier : public vmime::security::cert::defaultCertificateVerifier
{
	public:
		void verify(vmime::ref <vmime::security::cert::certificateChain> chain)
		{
			(void)chain;
			return;
		}
};

class interactiveAuthenticator : public vmime::security::sasl::defaultSASLAuthenticator
{
	private:
		const SMTPConfig	*m_config;
	public:
		interactiveAuthenticator(const SMTPConfig * _config):
			m_config(_config)
		{
		}
	
	private:
		const std::vector <vmime::ref <vmime::security::sasl::SASLMechanism> > getAcceptableMechanisms
		(const std::vector <vmime::ref <vmime::security::sasl::SASLMechanism> >& available,
			vmime::ref <vmime::security::sasl::SASLMechanism> suggested) const
		{
			return defaultSASLAuthenticator::getAcceptableMechanisms(available, suggested);
		}

		void setSASLMechanism(vmime::ref <vmime::security::sasl::SASLMechanism> mech)
		{
			defaultSASLAuthenticator::setSASLMechanism(mech);
		}

		const vmime::string getUsername() const
		{
			return m_config->userName().c_str();
		}

		const vmime::string getPassword() const
		{
			return m_config->password().c_str();
		}
};

SMTPSend::SMTPSend ( const SMTPConfig *config ) :
		m_config ( config )
{}


SMTPSend::~SMTPSend()
{}

void SMTPSend::send(const ICalMessage & message)
{
	try
	{
		vmime::ref <vmime::message> msg = message.getMessageBody();
		
		std::string str;
		str = "smtp://" + m_config->host();
		vmime::utility::url url(str.c_str());
		
		vmime::ref <vmime::net::session> session = vmime::create <vmime::net::session>();
		
		//session->getProperties().setProperty("options.sasl", true);
		session->getProperties().setProperty("transport.smtp.options.need-authentication", true);
		session->getProperties().setProperty("transport.smtps.options.need-authentication", true);
		
		//vmime::ref <vmime::net::transport> transport = session->getTransport(url, vmime::create <interactiveAuthenticator>(m_config));
		
		vmime::ref <vmime::net::transport> transport = session->getTransport(url);
		
		//transport->setProperty("connection.tls", true);
		transport->setProperty("auth.username", m_config->userName().c_str());
		transport->setProperty("auth.password", m_config->password().c_str());
		
		transport->setCertificateVerifier(vmime::create <certificateVerifier>());
		
		transport->connect();
		transport->send(msg);
		transport->disconnect();
	}
	catch (vmime::exception& e)
	{
// 		std::cerr << std::endl;
// 		std::cerr << e << std::endl;
		throw;
	}
	catch (std::exception& e)
	{
/*		std::cerr << std::endl;
		std::cerr << "std::exception: " << e.what() << std::endl;*/
		throw;
	}
	
}



