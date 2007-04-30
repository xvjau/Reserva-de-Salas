/*
	Reserva de Salas
	Copyright 2006 Gianfranco Rossi.

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

#include "CData.h"
#include "CReservaItem.h"
#include "CConfig.h"
#include "CMainWindow.h"
#include "CNotification.h"

#include "main.h"

#include <QtCore/QSettings>
#include <QtGui/QMessageBox>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <string>

#include <iostream>

CData::CData ( QObject * _parent ) :
		QObject ( _parent ),
		m_areas ( 0 ),
		m_db ( 0 ),
		m_connected ( false ),
		m_notify ( 0 ),
		m_areasId ( 0 ),
		m_event ( 0 )
{}

CData::~CData()
{
	disconnect();

	TColorSchemeList::iterator it;

	for ( it = m_colorSchemes.begin(); it != m_colorSchemes.end(); ++it )
	{
		delete *it;
	}

	if ( m_notify )
		delete m_notify;

	if ( m_areas )
		delete m_areas;

	if ( m_areasId )
		delete m_areasId;
}

bool CData::connect()
{
	QSettings* settings = getConfigFile();

	try
	{
		QString sdbName, sserverName, suserName, spassword, scharset;

		settings->beginGroup ( "DB" );

		if ( ! settings->contains ( "DB" ) )
		{
			settings->setValue ( "DB", "RS" );
			settings->setValue ( "Server", "localhost" );
			settings->setValue ( "User", "RS" );
			settings->setValue ( "Password", "rs" );
			settings->setValue ( "Characterset", "ISO8859_1" );
		}

		sdbName = settings->value ( "DB" ).toString();
		sserverName = settings->value ( "Server" ).toString();
		suserName = settings->value ( "User" ).toString();
		spassword = settings->value ( "Password" ).toString();
		scharset = settings->value ( "Characterset" ).toString();

		settings->endGroup();

		m_db = DatabaseFactory ( sserverName.toStdString(), sdbName.toStdString(),
		                         suserName.toStdString(), spassword.toStdString(), "",
		                         scharset.toStdString(), "" );
		m_db->Connect();
		loadColorSchemes();

		try
		{
			m_notify = new CNotification();
			m_event = EventsFactory ( m_db );
			m_notify->setEvents ( m_event );
			m_event->Add ( "reserva_ins", m_notify );
			m_event->Add ( "reserva_upd", m_notify );
			m_event->Add ( "reserva_del", m_notify );
		}
		catch ( Exception &e )
		{
			std::cerr << e.ErrorMessage() << std::endl;
			QMessageBox ( tr ( "Erro" ), e.ErrorMessage(), QMessageBox::Warning, QMessageBox::Cancel, 0, 0 ).exec();
		}

		m_connected = true;
		deleteConfigFile();
	}
	catch ( Exception &e )
	{
		std::cerr << e.ErrorMessage() << std::endl;
		QMessageBox ( tr ( "Erro ao conectar-se ao Servidor" ),
		              tr ( "<b>Foi impossiv&eacute;l connectar-se ao servidor</b><br><br><small>Mensagem do banco:<br>" ) +
		              QString ( e.ErrorMessage() ).replace ( QChar ( 10 ), "<br>" ) + QString ( "</small>" ),
		              QMessageBox::Warning, QMessageBox::Cancel, 0, 0 ).exec();


		m_connected = false;
	}

	return m_connected;
}

void CData::disconnect()
{
	if ( m_connected )
	{
		m_event->Clear();
		m_db->Disconnect();
	}
}

void CData::loadColorSchemes()
{
	Transaction tr = TransactionFactory ( m_db, amRead );
	tr->Start();

	Statement stmt = StatementFactory ( m_db, tr );

	stmt->Prepare ( "Select SCHEMEID, BACKGROUND, FONT, BORDER From COLOR_SCHEME" );
	stmt->Execute();

	QPalette* scheme;
	int i;
	QString s;

	while ( stmt->Fetch() )
	{
		scheme = new QPalette ( app()->palette() );

#define GET_COLOR(INT, NAME) \
			if (! stmt->IsNull(INT)) \
			{                        \
				stmt->Get(INT, i);       \
				s.setNum(i, 16);         \
				s = "#" + s.rightJustified(6, '0');\
				scheme->setColor(NAME, QColor(s)); \
			}

		GET_COLOR ( 2, QPalette::Base );
		GET_COLOR ( 3, QPalette::Text );
		GET_COLOR ( 4, QPalette::Shadow );

		stmt->Get ( 1, i );
		m_colorSchemes.insert ( i, scheme );
	}
	stmt->Close();

	tr->Rollback();
}

int CData::getAreaId ( const int _areaListIndex )
{
	if ( ! m_areasId )
		getAreas();

	return m_areasId->value ( _areaListIndex );
};

QStringList* CData::refreshAreas()
{
	if ( m_areas )
	{
		delete m_areas;
		m_areas = 0;
	}
	return getAreas();
}

QStringList* CData::getAreas()
{
	if ( m_areas )
		return m_areas;

	m_areas = new QStringList;
	m_areasId = new QList<int>;

	Transaction tr = TransactionFactory ( m_db, amRead );
	tr->Start();

	Statement stmt = StatementFactory ( m_db, tr );

	stmt->Prepare ( "Select AREAID, AREA From AREAS Order by AREA" );
	stmt->Execute();

	std::string s;
	int id;
	while ( stmt->Fetch() )
	{
		stmt->Get ( 1, id );
		stmt->Get ( 2, s );

		m_areasId->append ( id );
		m_areas->append ( QString::fromAscii ( s.c_str() ) );
	}

	stmt->Close();
	tr->Rollback();

	return m_areas;
}
