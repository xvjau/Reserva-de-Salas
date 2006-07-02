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
	m_db(0)
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
			settings.setValue("User", "SYSDBA");
			settings.setValue("Password", "masterkey");
			settings.setValue("Characterset", "ISO8859_1");
		}
		
		sdbName = settings.value("DB", "RS").toString();
		sserverName = settings.value("Server", "localhost").toString();
		suserName = settings.value("User", "SYSDBA").toString();
		spassword = settings.value("Password", "masterkey").toString();
		scharset = settings.value("Characterset", "ISO8859_1").toString();
		
		m_db = DatabaseFactory(sserverName.toStdString(), sdbName.toStdString(),
					suserName.toStdString(), spassword.toStdString(), "",
	                        	scharset.toStdString(), "");
		m_db->Connect();
		loadColorSchemes();

		try
		{
			#ifdef __ASYNC_EVENTS
			m_event = EventsFactory(m_db, true);
			#else
			m_event = EventsFactory(m_db, false);
			m_notify.setEvents(m_event);
			#endif
			m_event->Add("reserva_ins", &m_notify);
			m_event->Add("reserva_upd", &m_notify);
			m_event->Add("reserva_del", &m_notify);
		}
		catch (Exception &e)
		{
			std::cerr << e.ErrorMessage() << std::endl;
			QMessageBox("Aten�o", e.ErrorMessage(), QMessageBox::Warning, QMessageBox::Cancel, 0, 0).exec();
		}
		return true;
	}
	catch (Exception &e)
	{
		std::cerr << e.ErrorMessage() << std::endl;
		QMessageBox("Erro", e.ErrorMessage(), QMessageBox::Warning, QMessageBox::Cancel, 0, 0).exec();
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

CSalaList::CSalaList(CData *_owner):
    m_owner(_owner)
{
	m_tr = new Transaction;
	*m_tr = TransactionFactory(m_owner->m_db, amWrite);
	(*m_tr)->Start();
}

CSalaList::~CSalaList()
{
	if((*m_tr)->Started())
		(*m_tr)->Rollback();

	delete m_tr;
}

bool CSalaList::loadList()
{
	try
	{
		Statement stmt = StatementFactory(m_owner->m_db, *m_tr);

		stmt->Prepare("Select SALAID, ANDAR, NOME From SALAS Order By ANDAR, SALAID");
		stmt->Execute();

		CSala* sala;

		std::string snome;

		int i = 0;
		while (stmt->Fetch())
		{
			sala = addSala();

			stmt->Get(1, i);
			sala->setSalaID(i);
			sala->oldSALAID = sala->SALAID;

			stmt->Get(2, sala->ANDAR);
			
			stmt->Get(3, snome);
			sala->NOME = snome.c_str();
		}

		return true;
	}
	catch (Exception &e)
	{
		std::cerr << e.ErrorMessage() << std::endl;
		QMessageBox("Erro", e.ErrorMessage(), QMessageBox::Warning, QMessageBox::Cancel, 0, 0).exec();
		return false;
	}
}

PSala CSalaList::addSala()
{
	CSala *result = new CSala(this);
	return result;
}

void CSalaList::save()
{
	(*m_tr)->Commit();
}

CSala::CSala(CSalaList *_owner):
    m_owner(_owner),
	SALAID(0),
	oldSALAID(-1),
	ANDAR(0)
{
}

CSala::~CSala()
{
}

void CSala::setSalaID(const int _salaID)
{
	if (SALAID)
		m_owner->m_salas.remove(SALAID);
		
	SALAID = _salaID;
	m_owner->m_salas.insert(SALAID, this);
}

bool CSala::save()
{
	Database db = m_owner->m_owner->m_db;
	
	try
	{
		Statement stmt = StatementFactory(db, *(m_owner->m_tr));

		if (oldSALAID == -1)
		{
			stmt->Prepare("insert into SALAS (SALAID, ANDAR, NOME) values (?, ?, ?)");

			stmt->Set(1, SALAID);
			stmt->Set(2, ANDAR);
			stmt->Set(3, NOME.toStdString());
		}
		else
		{
			stmt->Prepare("update SALAS set SALAID = ?, ANDAR = ?, NOME = ? where SALAID = ?");

			stmt->Set(1, SALAID);
			stmt->Set(2, ANDAR);
			stmt->Set(3, NOME.toStdString());
			stmt->Set(4, oldSALAID);
		}

		stmt->Execute();

		oldSALAID = SALAID;
		m_owner->m_salas.insert(SALAID, this);

		return true;
	}
	catch (SQLException &e)
	{
		std::cerr << e.ErrorMessage() << std::endl;
		throw e.SqlCode();
		return false;
	}
	catch (Exception &e)
	{
		std::cerr << e.ErrorMessage() << std::endl;
		QMessageBox("Erro", e.ErrorMessage(), QMessageBox::Warning, QMessageBox::Cancel, 0, 0).exec();
		return false;
	}
}

bool CSala::del()
{
	Database db = m_owner->m_owner->m_db;

	try
	{
		Statement stmt = StatementFactory(db, *(m_owner->m_tr));

		if (oldSALAID != -1)
		{
			stmt->Prepare("delete from SALAS where SALAID = ?");

			stmt->Set(1, oldSALAID);
		}

		stmt->Execute();
		(*m_owner->m_tr)->CommitRetain();

		m_owner->m_salas.remove(SALAID);

		delete this;
		return true;
	}
	catch (Exception &e)
	{
		std::cerr << e.ErrorMessage() << std::endl;
		QMessageBox("Erro", e.ErrorMessage(), QMessageBox::Warning, QMessageBox::Cancel, 0, 0).exec();
		return false;
	}
}

//====================  RESERVAS ===================================

CReservaList::CReservaList(CSemana *_owner, QDate _date, int _salaID):
	QFrame(_owner->m_parent),
	m_owner(_owner),
	m_date(_date),
	m_salaID(_salaID)

{
	gridLayout = new QGridLayout(this);
	gridLayout->setSpacing(0);
	gridLayout->setMargin(0);
	gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
	vboxLayout = new QVBoxLayout();
	vboxLayout->setSpacing(1);
	vboxLayout->setMargin(0);
	vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
	
	gridLayout->addLayout(vboxLayout, 0, 0, 1, 1);
}

CReservaList::~CReservaList()
{
	delete vboxLayout;
	delete gridLayout;
}

bool CReservaList::loadList(rowRESERVA* _row)
{
	CReserva* reserva;

	while (_row->checkRow(this))
	{
		reserva = addReserva();
		
		reserva->oldRESERVAID	= _row->RESERVAID;
		reserva->RESERVAID		= _row->RESERVAID;
		reserva->SALAID			= _row->SALAID;
		reserva->oldSALAID		= _row->SALAID;
		reserva->DATA			= _row->DATA;
		reserva->HORAIN			= _row->HORAIN;
		reserva->HORAFIM 		= _row->HORAFIM;
		reserva->USUARIOID 		= _row->USUARIOID;
		reserva->USUARIO 		= _row->USUARIO;
		reserva->ASSUNTO 		= _row->ASSUNTO;
		reserva->DEPTO 			= _row->DEPTO;
		reserva->NOTAS			= _row->NOTAS;
		reserva->TIPO			= _row->TIPO;
		reserva->m_colorScheme = m_owner->m_owner->getColorScheme(_row->SCHEMEID);

		reserva->refreshData();
		vboxLayout->addWidget(reserva);
		
		if (m_owner->m_reservaItems[reserva->RESERVAID])
			m_owner->m_reservaItems[reserva->RESERVAID]->addChild(reserva);
		else
			m_owner->m_reservaItems.insert(reserva->RESERVAID, reserva);

		m_owner->fetchRow();
	}
	return true;
}

CReservaList::CReserva* CReservaList::addReserva()
{
	CReserva *result = new CReserva(this);
	m_reservas.push_back(result);

	result->m_colorScheme = m_owner->m_owner->getColorScheme(CConfig::getConfig()->getColorScheme());
	return result;
}

void CReservaList::insertReserva(CReserva* _reserva)
{
	CReserva* reserva;
	TReservaList::iterator it;
	int i = 0;
	
	for (it = m_reservas.begin(); it != m_reservas.end(); ++it, ++i)
	{
		reserva = *it;
		
		if (reserva->HORAIN > _reserva->HORAIN)
		{
			m_reservas.insert(it, _reserva);
			vboxLayout->insertWidget(i, _reserva);
			return;
		}
	}
	m_reservas.push_back(_reserva);
	vboxLayout->addWidget(_reserva);
}

void CReservaList::mouseDoubleClickEvent(QMouseEvent * event)
{
	switch (CConfig::getConfig()->getNivel())
	{
		case 0: return;
	}
	
	CReserva* reserva = addReserva();

	CReservaItem *reservaItem = new CReservaItem(reserva, m_owner->m_salas,
								m_owner->m_parent);
	
	reservaItem->setSala(m_salaID);
	reservaItem->setDate(m_date);

	if (m_date == QDate::currentDate())
	{
		int h = QTime::currentTime().hour();
		reservaItem->setTime( QTime(h, 0) );
	}
	else
		reservaItem->setTime( QTime(8, 0) );
	
	reservaItem->setModal(true);
	reservaItem->show();
}

void CReservaList::mousePressEvent(QMouseEvent * event)
{
	CMainWindow* form = m_owner->m_parent;
	
	form->setActiveReserva(0);
	form->setActiveDate(m_date);
	form->setActiveSalaID(m_salaID);
	
	if (event->button() == Qt::RightButton)
		emit showReservaMenu(mapToGlobal(event->pos()));
	
	CSalaList* salaList = m_owner->m_salas;
	PSala sala = salaList->m_salas[m_salaID];

	form->tbReservas->setCurrentCell(m_date.dayOfWeek() - 1, sala->m_column);
}

CReservaList::CReserva::CReserva(CReservaList *_owner):
	QFrame(_owner, 0),
    m_owner(_owner),
	oldRESERVAID(-1),
	RESERVAID(0),
	SALAID(0),
	oldSALAID(0),
	HORAIN(QTime(0,0,0)),
	HORAFIM(QTime(0,0,0)),
	DATA(QDate(0,0,0)),
	USUARIOID(0),
	TIPO('S'),
	m_relocate(false),
	m_height(0),
	m_deleting(false),
	m_readSemanal(false),
	m_readMensal(false),
	m_parent(0)
{
	resize(QSize(215, 56).expandedTo(minimumSizeHint()));
	QSizePolicy FormsizePolicy(static_cast<QSizePolicy::Policy>(3), static_cast<QSizePolicy::Policy>(0));
	FormsizePolicy.setHorizontalStretch(0);
	FormsizePolicy.setVerticalStretch(0);
	FormsizePolicy.setHeightForWidth(false);
	setSizePolicy(FormsizePolicy);
		
	vboxLayout = new QVBoxLayout(this);
	vboxLayout->setSpacing(1);
	vboxLayout->setMargin(0);
	vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
	hboxLayout = new QHBoxLayout();
	hboxLayout->setSpacing(0);
	hboxLayout->setMargin(0);
	hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
	vboxLayout1 = new QVBoxLayout();
	vboxLayout1->setSpacing(0);
	vboxLayout1->setMargin(0);
	vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
	
	QSizePolicy sizePolicyFixed(static_cast<QSizePolicy::Policy>(0), static_cast<QSizePolicy::Policy>(13));
	sizePolicyFixed.setHorizontalStretch(0);
	sizePolicyFixed.setVerticalStretch(0);
	sizePolicyFixed.setHeightForWidth(false);

	QFont font;
	#ifdef __unix__
	font.setPointSize(11);
	#else
	font.setFamily(QString::fromUtf8("Sans Serif"));
	font.setPointSize(9);
	#endif
	font.setBold(true);
	font.setItalic(false);
	font.setUnderline(false);
	font.setStrikeOut(false);

	lblHoraIn = new QLabel(this);
	lblHoraIn->setObjectName(QString::fromUtf8("lblHoraIn"));
	lblHoraIn->resize(QSize(140, 132));
	lblHoraIn->setSizePolicy(sizePolicyFixed);
	lblHoraIn->setMargin(0);
	lblHoraIn->setFont(font);
	lblHoraIn->setTextFormat(Qt::PlainText);
	lblHoraIn->setAlignment(Qt::AlignCenter);

	vboxLayout1->addWidget(lblHoraIn);
	
	lblHoraFim = new QLabel(this);
	lblHoraFim->setObjectName(QString::fromUtf8("lblHoraFim"));
	lblHoraFim->setTextFormat(Qt::PlainText);
	lblHoraFim->setScaledContents(false);
	lblHoraFim->setAlignment(Qt::AlignCenter);
	lblHoraFim->resize(QSize(140, 132));
	lblHoraFim->setSizePolicy(sizePolicyFixed);
	lblHoraFim->setMargin(0);
	lblHoraFim->setFont(font);
	
	vboxLayout1->addWidget(lblHoraFim);
	
	hboxLayout->addLayout(vboxLayout1);
	
	lblTitulo = new QLabel(this);
	lblTitulo->setObjectName(QString::fromUtf8("lblTitulo"));
	QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(13), static_cast<QSizePolicy::Policy>(13));
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(lblTitulo->sizePolicy().hasHeightForWidth());
	lblTitulo->setSizePolicy(sizePolicy);
	lblTitulo->setTextFormat(Qt::RichText);
	lblTitulo->setAlignment(Qt::AlignCenter);
	lblTitulo->setWordWrap(true);
	lblTitulo->setMargin(0);
	
	hboxLayout->addWidget(lblTitulo);
	
	vboxLayout->addLayout(hboxLayout);
	
	setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
	setLineWidth(1);
	setMidLineWidth(2);
}

CReservaList::CReserva::~CReserva()
{
	m_deleting = true;
	
	if (m_parent) 
	{
		m_parent->m_children.removeAll(this);
		if (! m_parent->m_deleting)
			delete m_parent;
	}
	else	
		if (m_children.count())
			while(m_children.count())
				delete m_children[m_children.count()-1];
	
	delete lblTitulo;
	delete lblHoraFim;
	delete lblHoraIn;
	delete vboxLayout1;
	delete hboxLayout;
	delete vboxLayout;
}

void CReservaList::CReserva::readSemanal()
{
	if (m_deleting)
		return;
		
	Database db = m_owner->m_owner->m_owner->m_db;
	
	try
	{
		Transaction tr = TransactionFactory(db, amWrite);
		tr->Start();
		
		Statement stmt = StatementFactory(db, tr);
		
		stmt->Prepare("Select \
							DATAIN, DATAFIM, SEG, TER, QUA, QUI, SEX, SAB, DOM, REVESA \
						From \
							RESERVA_SEMANAL \
						Where \
							RESERVAID = ?");
		stmt->Set(1, RESERVAID);
		stmt->Execute();
		
		stmt->Fetch();
		
		Date dt;
		int y, mo, d;
		
		stmt->Get(1, dt);
		dt.GetDate(y, mo, d);
		DATAIN = QDate(y, mo, d);
		
		stmt->Get(2, dt);
		dt.GetDate(y, mo, d);
		DATAFIM = QDate(y, mo, d);
		
		#define readDOWValues(INT) \
			if (stmt->IsNull(INT + 3)) \
				DIAS[INT] = false; \
			else \
			{ \
				stmt->Get(INT + 3, y); \
				DIAS[INT] = y; \
			} 
		
		readDOWValues(0);
		readDOWValues(1);
		readDOWValues(2);
		readDOWValues(3);
		readDOWValues(4);
		readDOWValues(5);
		readDOWValues(6);
		
		if (stmt->IsNull(10)) 
			REVESA = false; 
		else 
		{ 
				stmt->Get(10, y); 
				REVESA = y; 
		} 
		
		stmt->Close();
		
		m_readSemanal = true;
	}
	catch (Exception &e)
	{
		std::cerr << e.ErrorMessage() << std::endl;
		QMessageBox("Erro", e.ErrorMessage(), QMessageBox::Warning, QMessageBox::Cancel, 0, 0).exec();
		throw;
	}
}

void CReservaList::CReserva::readMensal()
{
	if (m_deleting)
		return;
		
	Database db = m_owner->m_owner->m_owner->m_db;
	
	try
	{
		Transaction tr = TransactionFactory(db, amWrite);
		tr->Start();
		
		Statement stmt = StatementFactory(db, tr);
		
		stmt->Prepare("Select \
							DATAFIM, DIA_S, ORDEM \
						From \
							RESERVA_MENSAL \
						Where \
							RESERVAID = ?");
		stmt->Set(1, RESERVAID);
		stmt->Execute();
		
		stmt->Fetch();
		
		Date dt;
		int y, mo, d;
		
		stmt->Get(1, dt);
		dt.GetDate(y, mo, d);
		DATAFIM = QDate(y, mo, d);
		
		stmt->Get(2, ORDEM);
		stmt->Get(3, DIA_S);
		
		stmt->Close();
		
		m_readSemanal = true;
	}
	catch (Exception &e)
	{
		std::cerr << e.ErrorMessage() << std::endl;
		QMessageBox("Erro", e.ErrorMessage(), QMessageBox::Warning, QMessageBox::Cancel, 0, 0).exec();
		throw;
	}
}

QDate CReservaList::CReserva::getDATAIN()
{
	if ((TIPO == 'W') && (!m_readSemanal))
		readSemanal();
	else
		if ((TIPO == 'M') && (!m_readMensal))
			readMensal();
	
	return DATAIN;
}

QDate CReservaList::CReserva::getDATAFIM()
{
	if ((TIPO == 'W') && (!m_readSemanal))
		readSemanal();
	else
		if ((TIPO == 'M') && (!m_readMensal))
			readMensal();
	
	return DATAFIM;
}

bool CReservaList::CReserva::getDOW(int _day)
{
	if ((_day < 0) || (_day > 6))
	{
		std::cerr << "Index out of bounds!" << std::endl;
		std::cerr << "CReservaList::getDOW(" << _day << ")" << std::endl;
		throw QString("Index out of bounds!");
	}
	
	if (!m_readSemanal)
		readSemanal();
	
	return DIAS[_day];
}

bool CReservaList::CReserva::getREVESA()
{
	if (!m_readSemanal)
		readSemanal();

	return REVESA;
}

void CReservaList::CReserva::setDOW(int _day, bool _value)
{
	if ((_day < 0) || (_day > 6))
	{
		std::cerr << "Index out of bounds!" << std::endl;
		std::cerr << "CReservaList::getDOW(" << _day << ")" << std::endl;
		throw QString("Index out of bounds!");
	}
	
	DIAS[_day] = _value;
}

bool CReservaList::CReserva::save()
{
	if (m_deleting)
		return false;
		
	Database db = m_owner->m_owner->m_owner->m_db;
	
	try
	{
		Transaction tr = TransactionFactory(db, amWrite);
		tr->Start();
		
		Statement stmt = StatementFactory(db, tr);

		if (oldRESERVAID == -1)
		{
			USUARIOID = CConfig::getConfig()->getUsuarioID();
			USUARIO = CConfig::getConfig()->getUserName();

			stmt->Execute("Select GEN_ID(GENRESERVAS, 1) From RDB$DATABASE");
			stmt->Fetch();
			stmt->Get(1, RESERVAID);
			stmt->Close();
			
			if (m_owner->m_owner->m_reservaItems[RESERVAID])
				m_owner->m_owner->m_reservaItems[RESERVAID]->addChild(this);
			else
				m_owner->m_owner->m_reservaItems.insert(RESERVAID, this);
			
			m_relocate = true;
		}

		switch (TIPO)
		{
			case 'S': // Simples
			{
				stmt->Prepare("Select OUT_RESERVAID, SEQ From CREATE_RESERVA_SIMPLES(?, ?, ?, ?, ?, ?, ?, ?, ?)");
		
				stmt->Set(1, oldRESERVAID);
				stmt->Set(2, SALAID);
				stmt->Set(3, USUARIOID);
				stmt->Set(4, ASSUNTO.toStdString());
				stmt->Set(5, DEPTO.toStdString());
				stmt->Set(6, NOTAS.toStdString());
				stmt->Set(7, Date(DATA.year(), DATA.month(), DATA.day()));
				stmt->Set(8, Time(HORAIN.hour(), HORAIN.minute(), HORAIN.second()));
				stmt->Set(9, Time(HORAFIM.hour(), HORAFIM.minute(), HORAFIM.second()));
				
				break;
			}
			case 'W': // Semanal
			{
				stmt->Prepare("Select OUT_RESERVAID, SEQ From CREATE_RESERVA_SEMANAL(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
		
				stmt->Set(1, oldRESERVAID);
				stmt->Set(2, SALAID);
				stmt->Set(3, USUARIOID);
				stmt->Set(4, ASSUNTO.toStdString());
				stmt->Set(5, DEPTO.toStdString());
				stmt->Set(6, NOTAS.toStdString());
				stmt->Set(7, Date(DATA.year(), DATA.month(), DATA.day()));
				stmt->Set(8, Date(DATAFIM.year(), DATAFIM.month(), DATAFIM.day()));
				stmt->Set(9, Time(HORAIN.hour(), HORAIN.minute(), HORAIN.second()));
				stmt->Set(10, Time(HORAFIM.hour(), HORAFIM.minute(), HORAFIM.second()));

				for (int i = 0; i < 7; ++i)
					stmt->Set(i + 11, DIAS[i] ? 1 : 0);

				stmt->Set(18, (int)(REVESA));
				
				break;
			}
			case 'M': // Mensal
			{
				std::cerr << "Unknown TYPE:" << TIPO << std::endl;
				break;
			}
			default:
			{
				std::cerr << "Unknown TYPE:" << TIPO << std::endl;
				break;
			}
		}

		stmt->Execute();
		
		stmt->Fetch();
		stmt->Get(1, RESERVAID);
		stmt->Get(2, m_owner->m_owner->m_lastUpdate);
		
		stmt->Close();

		m_owner->m_owner->m_owner->m_notify.incIgnoreCount();

		tr->Commit();

		oldRESERVAID = RESERVAID;
		
		if (m_relocate)
		{
			relocate();
			if (m_deleting)
				return false;
		}

		refreshData();

		return true;
	}
	catch (SQLException &e)
	{
		std::cerr << e.ErrorMessage() << std::endl;
		throw e.SqlCode();
		return false;
	}
	catch (Exception &e)
	{
		std::cerr << e.ErrorMessage() << std::endl;
		QMessageBox("Erro", e.ErrorMessage(), QMessageBox::Warning, QMessageBox::Cancel, 0, 0).exec();
		return false;
	}
}

void CReservaList::CReserva::relocate()
{
	QDate segunda = m_owner->m_owner->m_date;
	
	if ((DATA < segunda) || (DATA >= segunda.addDays(8)))
	{
		m_owner->m_reservas.removeAll(this);
		delete this;
		return;
	}
	else
	{
		if ((DATA.daysTo(m_owner->m_date)) || (oldSALAID != SALAID))
		{
			m_owner->m_reservas.removeAll(this);

			int idow = segunda.daysTo(DATA) + 1;

			CReservaList *reservaList = m_owner->m_owner->getReservaList(idow, SALAID);

			if (reservaList)
			{
				reservaList->insertReserva(this);
				m_owner = reservaList;
				
				oldSALAID = SALAID;
			}
			else
			{
				delete this;
				return;
			}
		}
	}
	
	m_relocate = false;
}

bool CReservaList::CReserva::del()
{
	Database db = m_owner->m_owner->m_owner->m_db;

	try
	{
		Transaction tr = TransactionFactory(db, amWrite);
		tr->Start();
	
		Statement stmt = StatementFactory(db, tr);
	
		if (oldRESERVAID != -1)
		{
			stmt->Prepare("delete from RESERVAS where RESERVAID = ?");

			stmt->Set(1, oldRESERVAID);
		}

		stmt->Execute();

		stmt->Execute("Select GEN_ID(SEQRESERVAS, 0) From RDB$DATABASE");
		stmt->Fetch();
		stmt->Get(1, m_owner->m_owner->m_lastUpdate);

		m_owner->m_owner->m_owner->m_notify.incIgnoreCount();
		tr->Commit();
	
		m_owner->m_reservas.removeAll(this);
	
		delete this;
		return true;
	}
	catch (Exception &e)
	{
		std::cerr << e.ErrorMessage() << std::endl;
		QMessageBox("Erro", e.ErrorMessage(), QMessageBox::Warning, QMessageBox::Cancel, 0, 0).exec();
		return false;
	}
}

void CReservaList::CReserva::refreshData()
{
	if (m_deleting)
		return;
	
	setToolTip("Resp.: " + USUARIO + '\n' + NOTAS);

	lblHoraIn->setText(HORAIN.toString("HH:mm"));
	lblHoraFim->setText(HORAFIM.toString("HH:mm"));

	QString s = ASSUNTO;
	
	if (DEPTO.length())
		s += "<br><i>(" + DEPTO + ")</i>";
	
	lblTitulo->setText(s);

	if ((m_colorScheme) &&
			(((DATA == QDate::currentDate()) && (HORAFIM >= QTime::currentTime())) ||
			(DATA > QDate::currentDate())))
		this->setPalette(*m_colorScheme);
	else
		this->setPalette(*g_disabledPalette);

	m_height = lblTitulo->heightForWidth(150) + 5;
}

void CReservaList::CReserva::mouseDoubleClickEvent(QMouseEvent * event)
{
	switch (CConfig::getConfig()->getNivel())
	{
		case 0: return;
		case 1: if (USUARIOID != CConfig::getConfig()->getUsuarioID()) return;
	}

	CReservaItem *reservaItem = new CReservaItem(this, m_owner->m_owner->m_salas,
										m_owner->m_owner->m_parent);

	reservaItem->setSala(SALAID);
	
	if (TIPO == 'S')
		reservaItem->setDate(DATA);
	else
	{
		reservaItem->setDate(getDATAIN());
		reservaItem->setDateFim(getDATAFIM());
	}
	
	reservaItem->setModal(true);
	reservaItem->show();
}

void CReservaList::CReserva::mousePressEvent(QMouseEvent * event)
{
	CMainWindow* form = m_owner->m_owner->m_parent;
	form->setActiveReserva(this);
	
	if (event->button() == Qt::RightButton)
		m_owner->emit showReservaMenu(mapToGlobal(event->pos()));

	CSalaList* salaList = m_owner->m_owner->m_salas;
	PSala sala = salaList->m_salas[SALAID];
	
	form->tbReservas->setCurrentCell(DATA.dayOfWeek() - 1, sala->m_column);
}

void CReservaList::CReserva::paintEvent(QPaintEvent * event)
{
	{
		QPainter painter(this);
		
		if ((m_colorScheme) &&
				(((DATA == QDate::currentDate()) && (HORAFIM >= QTime::currentTime())) ||
				(DATA > QDate::currentDate())))
			painter.fillRect(event->rect(), m_colorScheme->color(QPalette::Base));
		else
			painter.fillRect(event->rect(), QColor(225,225,225));
	}
	
	QFrame::paintEvent(event);
}


CSemana::CSemana(CMainWindow *_parent, QDate &_segunda, CData *_owner,
			CSalaList *_salas):
	m_date(_segunda),
	m_owner(_owner),
	m_salas(_salas),
	m_parent(_parent),
	m_firstReservaList(0),
	m_row(0)
{
	connect(&(m_owner->m_notify), SIGNAL(FBEvent(int, int)), this, SLOT(onFBEvent(int , int)), Qt::QueuedConnection);
	
	for(int idow = 0; idow < 7; ++idow)
	{
		for (int i = 0; i < m_reservas[idow].count(); ++i)
		{
			m_reservas[idow][i] = 0;
		}
	}
}

CSemana::~CSemana()
{
	clear();
	if(m_tr->Started())
		m_tr->Rollback();
}

void CSemana::fetchRow()
{
	if (m_stmt->Fetch())
		m_row = rowRESERVA(&m_stmt);
	else
		m_row = rowRESERVA(0);
}

bool CSemana::loadData()
{
	try
	{
		m_tr = TransactionFactory(m_owner->m_db, amRead);
	
		m_tr->Start();
		
		m_stmt = StatementFactory(m_owner->m_db, m_tr);
	
		m_stmt->Prepare("Select \
							RESERVAID, SALAID, DATA, HORAIN, HORAFIM, USUARIOID, \
							USUARIO_NOME, ASSUNTO, DEPTO, NOTAS, SCHEMEID, \
							TIPO, GEN_ID(SEQRESERVAS, 0) \
						From \
							GET_RESERVAS_SEMANA(?, ?) \
						Order By \
							SALAID, \
							DATA, \
							HORAIN");

		QDate day;
		Date pdate = Date(m_date.year(), m_date.month(), m_date.day());
		m_stmt->Set(1, pdate);
		pdate.Add(6);
		m_stmt->Set(2, pdate);

		m_stmt->Execute();
		fetchRow();
		
		m_stmt->Get(13, m_lastUpdate);
	
		CReservaList*   reservaList;
	
		CSalaList::TSalaList::iterator it;
		PSala sala;
		
		for(it = m_salas->m_salas.begin();
			it != m_salas->m_salas.end();
			++it)
		{
			sala = *it;
			
			for(int idow = 0; idow < 7; ++idow)
			{
				day = m_date.addDays(idow);
	
				reservaList = new CReservaList(this, day, sala->getSalaID());
				reservaList->loadList(&m_row);
	
				if (! connect(reservaList, SIGNAL(showReservaMenu(QPoint)), m_parent, SLOT(showReservaMenu(QPoint))))
					std::cerr << "Unable to connect reservaList(showReservaMenu) to m_parent(showReservaMenu)" << std::endl;
				
				m_reservas[idow].insert(sala->getSalaID(), reservaList);
		
				if (! m_firstReservaList)
					m_firstReservaList = reservaList;
			}
		}

		m_stmt->Close();
		m_tr->Rollback();
		
		return true;
	}
	catch (Exception &e)
	{
		std::cerr << e.ErrorMessage() << std::endl;
		QMessageBox("Erro", e.ErrorMessage(), QMessageBox::Warning, QMessageBox::Cancel, 0, 0).exec();
		return false;
	}
}

void CSemana::clear()
{
	for(int idow = 0; idow < 7; ++idow)
	{
		for (int i = 0; i < m_reservas[idow].count(); ++i)
		{
			if (m_reservas[idow][i])
			{
				delete m_reservas[idow][i];
			}
		}
	
		m_reservas[idow].clear();
	}
}

CReservaList* CSemana::getReservaList(int _dow, int _salaID)
{
	return m_reservas[_dow-1][_salaID];
}

void CSemana::onFBEvent(int event, int count)
{
	try
	{
		if (event == CNotification::FBEUnknown) return;

		m_tr = TransactionFactory(m_owner->m_db, amRead);
		m_tr->Start();

		m_stmt = StatementFactory(m_owner->m_db, m_tr);

		int iRow = 0,
			iSalaID = 0;

		switch (event)
		{
			case CNotification::FBEInsert:
			case CNotification::FBEUpdate:
			{
				m_stmt->Prepare("Select \
									RESERVAID, SALAID, DATA, HORAIN, HORAFIM, USUARIOID, \
									USUARIO_NOME, ASSUNTO, DEPTO, NOTAS, SCHEMEID, \
									TIPO, SEQ  \
								From \
									GET_RESERVAS_SEMANA(?, ?) \
								Where \
									SEQ > ?\
								Order By \
									SALAID, \
									DATA, \
									HORAIN");

				m_stmt->Set(1, Date(m_date.year(), m_date.month(), m_date.day()));
				m_stmt->Set(2, Date(m_date.year(), m_date.month(), m_date.day() + 6));
				m_stmt->Set(3, m_lastUpdate);

				m_stmt->Execute();
	
				CReservaList*   reservaList;
	
				fetchRow();
				m_stmt->Get(13, m_lastUpdate);
				
				if (m_row.TIPO == 'W')
				{
					m_parent->refreshData( m_parent->getDate() );
					return;
				}
				else	
					while (m_row.RESERVAID != -1)
					{
						iRow = m_row.DATA.dayOfWeek();
						iSalaID = m_row.SALAID;
						
						reservaList = getReservaList(iRow,iSalaID);
		
						if (event == CNotification::FBEInsert)
						{
							if (reservaList)
							{
								reservaList->loadList(&m_row);
							}
							else
							{
								std::cerr << "Warning!  Event recieved for non-existant Row" << std::endl;
								fetchRow();
							}
						}
						else
						{
							CReservaList::CReserva* reserva = m_reservaItems[m_row.RESERVAID];
							
							if (reserva)
							{
								reserva->setDATA(m_row.DATA);
								reserva->setSALAID(m_row.SALAID);
								reserva->setHORAIN(m_row.HORAIN);
								reserva->setHORAFIM(m_row.HORAFIM);
								reserva->setUSUARIOID(m_row.USUARIOID);
								reserva->setUSUARIO(m_row.USUARIO);
								reserva->setASSUNTO(m_row.ASSUNTO);
								reserva->setDEPTO(m_row.DEPTO);
								reserva->setNOTAS(m_row.NOTAS);
								reserva->m_colorScheme = m_owner->getColorScheme(m_row.SCHEMEID);
		
								reserva->relocate();
								reserva->refreshData();
							}
							
							fetchRow(); 
						}
	
						m_parent->checkRowHeight(iRow - 1, iSalaID); 
					}
					m_stmt->Close(); 
					break; 
			}
		    case CNotification::FBEDelete:
			{
				m_stmt->Prepare("Select \
									RESERVAID, GEN_ID(SEQRESERVAS, 0) \
								From \
									DEL$RESERVAS \
								Where \
									SEQ > ?");

				m_stmt->Set(1, m_lastUpdate);
				m_stmt->Execute();
				
				Timestamp ts;
				int iReservaID;
				
				CReservaList* reservaList;
				CReservaList::CReserva* reserva;
				CReservaList::TReservaList::iterator it;
				
				while (m_stmt->Fetch())
				{
					m_stmt->Get(1, iReservaID);
					m_stmt->Get(2, m_lastUpdate);
	
					reserva = m_reservaItems[iReservaID];
					if (reserva)
					{
						delete reserva;
					}
				}
				break;
			}
		}
	}
	catch (Exception &e)
	{
		std::cerr << e.ErrorMessage() << std::endl; std::cout << __LINE__ << std::endl;
		QMessageBox("Erro", e.ErrorMessage(), QMessageBox::Warning, QMessageBox::Cancel, 0, 0).exec(); std::cout << __LINE__ << std::endl;
		return; std::cout << __LINE__ << std::endl;
	}
}
