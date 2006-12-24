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

static QPalette *g_disabledPalette = 0;

void exitFunction()
{
	if (g_disabledPalette)
	    delete g_disabledPalette;
}

CData::CData():
	m_db(0),
	m_notify(0),
	m_areas(0),
	m_areasId(0)
{
	if (! g_disabledPalette)
	{
		g_disabledPalette = new QPalette();
		atexit(exitFunction);

		g_disabledPalette->setColor(QPalette::Base, QColor(200,200,200));
		g_disabledPalette->setColor(QPalette::Text, QColor(0,0,0));
	}
}

CData::~CData()
{
	disconnect();
	
	TColorSchemeList::iterator it;
	
	for (it = m_colorSchemes.begin(); it != m_colorSchemes.end(); ++it)
	{
		delete *it;
	}

	if (m_notify)
		delete m_notify;

	if (m_areas)
		delete m_areas;

	if(m_areasId)
		delete m_areasId;
}

bool CData::connect()
{
	QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope, ".");
	QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "Conf","RS");
	
	try
	{
		QString sdbName, sserverName, suserName, spassword, scharset;
		
		settings.beginGroup("DB");
		
		if (! settings.contains("DB"))
		{
			settings.setValue("DB", "RS");
			settings.setValue("Server", "localhost");
			settings.setValue("User", "RS");
			settings.setValue("Password", "rs");
			settings.setValue("Characterset", "ISO8859_1");
		}
		
		sdbName = settings.value("DB", "RS").toString();
		sserverName = settings.value("Server", "localhost").toString();
		suserName = settings.value("User", "RS").toString();
		spassword = settings.value("Password", "rs").toString();
		scharset = settings.value("Characterset", "ISO8859_1").toString();
		
		m_db = DatabaseFactory(sserverName.toStdString(), sdbName.toStdString(),
					suserName.toStdString(), spassword.toStdString(), "",
	                        	scharset.toStdString(), "");
		m_db->Connect();
		loadColorSchemes();

		try
		{
			m_notify = new CNotification();
			#ifdef __ASYNC_EVENTS
			m_event = EventsFactory(m_db, true);
			#else
			m_event = EventsFactory(m_db, false);
			m_notify->setEvents(m_event);
			#endif
			m_event->Add("reserva_ins", m_notify);
			m_event->Add("reserva_upd", m_notify);
			m_event->Add("reserva_del", m_notify);
		}
		catch (Exception &e)
		{
			std::cerr << e.ErrorMessage() << std::endl;
			QMessageBox("Erro", e.ErrorMessage(), QMessageBox::Warning, QMessageBox::Cancel, 0, 0).exec();
		}
		return true;
	}
	catch (Exception &e)
	{
		std::cerr << e.ErrorMessage() << std::endl;
		QMessageBox("Erro ao conectar-se ao Servidor",
					QString("<b>Foi impossiv&eacute;l connectar-se ao servidor</b><br><br><small>Mensagem do banco:<br>") +
							QString(e.ErrorMessage()).replace(QChar(10), "<br>") + QString("</small>"),
					QMessageBox::Warning, QMessageBox::Cancel, 0, 0).exec();
					

		return false;
	}
}

void CData::disconnect()
{
	m_event->Clear();
	m_db->Disconnect();
}

void CData::loadColorSchemes()
{
	Transaction tr = TransactionFactory(m_db, amRead);
	tr->Start();
	
	Statement stmt = StatementFactory(m_db, tr);

	stmt->Prepare("Select SCHEMEID, BACKGROUND, FONT, BORDER From COLOR_SCHEME");
	stmt->Execute();
	
	QPalette* scheme;
	int i;
	QString s;

	while (stmt->Fetch())
	{
		scheme = new QPalette(g_application->palette());
		
		#define GET_COLOR(INT, NAME) \
			if (! stmt->IsNull(INT)) \
			{                        \
				stmt->Get(INT, i);       \
				s.setNum(i, 16);         \
				s = "#" + s.rightJustified(6, '0');\
				scheme->setColor(NAME, QColor(s)); \
			}

		GET_COLOR(2, QPalette::Base);
		GET_COLOR(3, QPalette::Text);
		GET_COLOR(4, QPalette::Shadow);
		
		stmt->Get(1, i);
		m_colorSchemes.insert(i, scheme);
	}
	stmt->Close();
	
	tr->Rollback();
}

int CData::getAreaId(const int _areaListIndex)
{
	if (! m_areasId)
		getAreas();
	
	return m_areasId->value(_areaListIndex);
};

QStringList* CData::refreshAreas()
{
	if (m_areas)
	{
		delete m_areas;
		m_areas = 0;
	}
	return getAreas();
}

QStringList* CData::getAreas()
{
	if (m_areas)
		return m_areas;

	m_areas = new QStringList;
	m_areasId = new QList<int>;
	
	Transaction tr = TransactionFactory(m_db, amRead);
	tr->Start();
	
	Statement stmt = StatementFactory(m_db, tr);

	stmt->Prepare("Select AREAID, AREA From AREAS Order by AREA");
	stmt->Execute();

	std::string s;
	int id;
	while (stmt->Fetch())
	{
		stmt->Get(1, id);
		stmt->Get(2, s);

		m_areasId->append(id);
		m_areas->append( QString::fromAscii( s.c_str() ) );
	}
		
	stmt->Close();
	tr->Rollback();

	return m_areas;
}
