#include "CMainWindow.h"
#include "CReservaItem.h"
#include "CSalas.h"
#include "CUsuarios.h"
#include "CSchemas.h"
#include "CModelos.h"

#include <QStyleFactory>
#include <QPrintDialog>
#include <QHeaderView>
#include <QScrollBar>
#include <QClipboard>

static const int PSALA_COL_ROLE = 1025;
static const int TABLE_ROW_HEIGHT = 30;

CMainWindow::CMainWindow():
		m_salaList(0),
	m_semana(0),
	m_stylesgroup(this),
	m_config(0),
	m_canRefresh(true),
	m_needRefresh(true)
{
	setupUi(this);
	
	#ifndef __unix__
	QFont font(lbData->font());
	font.setFamily(QString::fromUtf8("Arial"));
	lbData->setFont(font);
	#endif

	connect(pbAdicionar, SIGNAL(clicked()), actionAdicionar, SIGNAL(activated()));
	connect(pbRemover, SIGNAL(clicked()), actionRemover, SIGNAL(activated()));

	m_config = CConfig::getConfig(&m_data);

	if (! m_data.connect())
		throw -1;
	
	m_date = QDate::currentDate();
	m_date = m_date.addDays(m_date.dayOfWeek() * -1 + 1);
	refreshData(m_date);
	
	QStringList styles = QStyleFactory::keys();
	QAction *action;
	
	QString defStyle = m_config->getStyle();
	g_application->setStyle(defStyle);
	
	for(int i = 0; i < styles.count(); ++i)
	{
		action = new QAction(styles[i], this);
		action->setActionGroup(&m_stylesgroup);
		action->setCheckable(true);
		action->setChecked(defStyle == styles[i]);
		
		connect(action, SIGNAL(triggered()), this, SLOT(onSetStyle()));

		menuEstilo->addAction(action);
	}
	
	switch (m_config->getNivel())
	{
		case 0:
		{
			delete pbAdicionar;
			delete pbRemover;
		}
		case 1:
		case 2:
		{
			delete menuMenu;
			break;
		}
	}

	if (m_config->getNivel() > 1)
	{
		m_mnPopupReserva.addAction(actionAdicionar);
		m_mnPopupReserva.addAction(actionRemover);
		m_mnPopupReserva.addAction(actionAlterar);
		m_mnPopupReserva.addSeparator();
	}
	m_mnPopupReserva.addAction(actionImprimirLista);
	m_mnPopupReserva.addAction(actionCopiar);
	
	actionCopiar->setShortcut(QKeySequence("Ctrl+C"));

	//m_mnPopupReserva.addAction(actionImprimirReserva);
	
	m_mnPopupHoje.addAction(actionHoje);
}

CMainWindow::~CMainWindow()
{
	if (m_salaList)
		delete m_salaList;

	clearData();
}

void CMainWindow::onSetStyle()
{
	const QAction *action = dynamic_cast<QAction*>(sender());
	
	m_config->setStyle(action->text());
	g_application->setStyle(action->text());
}

void CMainWindow::checkRowHeight(int _row, int _salaID)
{
	CReservaList::TReservaList::iterator it;
	int iHeight = 0;

	CReservaList* reservaList = m_semana->getReservaList(_row+1, _salaID);

	for (it = reservaList->m_reservas.begin(); it != reservaList->m_reservas.end(); ++it)
		iHeight += (*it)->getHeight();

	if (iHeight > tbReservas->rowHeight(_row))
		tbReservas->setRowHeight(_row, iHeight);
}

void CMainWindow::resizeEvent(QResizeEvent * event)
{
	if (width() > 700)
	{
		int iWidth = tbReservas->verticalHeader()->width();
		
		if (tbReservas->verticalScrollBar())
			iWidth += tbReservas->verticalScrollBar()->visibleRegion().boundingRect().width();
		
		for(int i = 0; i < tbReservas->columnCount(); ++i)
		{
			tbReservas->setColumnWidth(i, ((tbReservas->width() - iWidth) / tbReservas->columnCount())- 2);
		}
	}
}

void CMainWindow::showEvent ( QShowEvent * event )
{
	resizeEvent( 0 );
}

void CMainWindow::mousePressEvent ( QMouseEvent * event )
{
	QPoint pos = event->pos();
	pos = mapToGlobal(event->pos());

	if (lbData->visibleRegion().contains(lbData->mapFromGlobal(pos)))
		m_mnPopupHoje.popup(pos);
}

void CMainWindow::clearData()
{
	if (m_semana)
	{
		delete m_semana;
		m_semana = 0;
	}
}

void CMainWindow::refreshData(const QDate &_date)
{
	m_date = _date;
	m_activeDate = m_date;
	m_needRefresh = true;
	
	if (!m_canRefresh)
		return;
	
	CUpdateLock lock(this);

	setActiveReserva(0);
	
	clearData();
		
	lbData->setText(m_date.toString("dd/MM/yyyy"));
	
	if (! m_salaList)
	{
		m_salaList = new CSalaList(&m_data);
		m_salaList->loadList();
	}
	
	QTableWidgetItem* item;
	{
		CSalaList::TSalaList::iterator it;

		tbReservas->setColumnCount(m_salaList->m_salas.size());
		tbReservas->setRowCount(7);

		btProx->setEnabled(m_salaList->m_salas.size());
		btAnte->setEnabled(m_salaList->m_salas.size());
		pbAdicionar->setEnabled(m_salaList->m_salas.size());

		int icol = 0;
		PSala sala;
		QString nomeSala;

		for( it = m_salaList->m_salas.begin();
				   it != m_salaList->m_salas.end();
				   ++it, ++icol)
		{
			sala = *it;
			sala->m_column = icol;

			nomeSala = sala->getNome().length() ?
						sala->getNome() :
					QString("Sala ") + QString::number(sala->getSalaID());

			item = tbReservas->horizontalHeaderItem(icol);
			//tbReservas->setColumnWidth(icol, 150);

			if (item)
			{
				item->setText(nomeSala);
				item->setData(PSALA_COL_ROLE, sala->getSalaID());
			}
			else
			{
				item = new QTableWidgetItem(nomeSala);
				item->setData(PSALA_COL_ROLE, sala->getSalaID());
				tbReservas->setHorizontalHeaderItem (icol, item);
			}
		}
	}
	
	QString sdow;
	for (int iday = 1; iday < 8; ++iday)
	{
		sdow = QDate::shortDayName(iday) + " " + m_date.addDays(iday - 1).toString("dd");
		item = tbReservas->verticalHeaderItem(iday - 1);

		if (item)
			item->setText(sdow);
		else
		{
			item = new QTableWidgetItem(sdow);
			tbReservas->setVerticalHeaderItem (iday - 1, item);
		}
	}
	
	if (! m_semana)
	{
		try
		{
			m_semana = new CSemana(this, m_date, &m_data, m_salaList);
			m_semana->loadData();
		}
		catch (Exception &e)
		{
			std::cerr << e.ErrorMessage() << std::endl;
			QMessageBox("Erro", e.ErrorMessage(), QMessageBox::Warning, QMessageBox::Cancel, 0, 0).exec();
			throw -1;
		}
	}
	
	int iHeight, iRowHeight;
	int isalaid;

	{
		CReservaList::TReservaList::iterator it;
		CReservaList* reserva;
		for (int iday = 1; iday < 8; ++iday)
		{
			iRowHeight = TABLE_ROW_HEIGHT;

			for(int icol = 0; icol < tbReservas->columnCount(); ++icol)
			{
				isalaid = tbReservas->horizontalHeaderItem(icol)->data(PSALA_COL_ROLE).value<int>();
				reserva = m_semana->getReservaList(iday, isalaid);
				tbReservas->setCellWidget(iday - 1, icol, reserva);

				iHeight = 0;
				for (it = reserva->m_reservas.begin(); it != reserva->m_reservas.end(); ++it)
					iHeight += (*it)->getHeight();

				if (iHeight > iRowHeight)
					iRowHeight = iHeight;
			}

			tbReservas->setRowHeight(iday - 1, iRowHeight);
		}
	}
	
	m_needRefresh = false;
}

void CMainWindow::on_actionSalas_activated()
{
	CSalas *salas = new CSalas(this, &m_data);
	salas->setModal(true);
	salas->show();
}

void CMainWindow::on_actionUsuarios_activated()
{
	CUsuarios *usuarios = new CUsuarios(&m_data);
	usuarios->setModal(true);
	usuarios->show();
}

void CMainWindow::on_actionCores_activated()
{
	CSchemas *schemas = new CSchemas(&m_data);
	schemas->setModal(true);
	schemas->show();
}

void CMainWindow::on_actionAdicionar_activated()
{
	CReservaItem *reservaItem;

	if (m_activeReserva)
	{
		CReservaList::CReserva* reserva = m_activeReserva->getOwner()->addReserva();

		reservaItem = new CReservaItem(reserva, m_salaList, this);

		reservaItem->setSala(m_activeReserva->getSALAID());
		reservaItem->setDate(m_activeReserva->getDATA());
	}
	else
	{
		CReservaList::CReserva* reserva = m_semana->getFirstReservaList()->addReserva();

		reservaItem = new CReservaItem(reserva, m_salaList, this);

		reservaItem->setSala(m_activeSalaID);
		reservaItem->setDate(m_activeDate);
	}

	if (m_activeDate == QDate::currentDate())
	{
		int h = QTime::currentTime().hour();
		reservaItem->setTime( QTime(h, 0) );
	}
	else
		reservaItem->setTime( QTime(8, 0) );

	reservaItem->setModal(true);
	reservaItem->show();
}

void CMainWindow::on_actionRemover_activated()
{
	if (m_activeReserva)
		if (! QMessageBox::question(
			this,
			"Excluir Reserva",
			"Tem certeza que deseja excluir a reserva " +
					m_activeReserva->getASSUNTO(),
			"&Sim", "&Não",
			QString(), 1, 0))
		{
			m_activeReserva->del();
			setActiveReserva(0);
		}
}

void CMainWindow::on_actionAlterar_activated()
{
	if (m_activeReserva)
	{
		CReservaItem *reservaItem = new CReservaItem(m_activeReserva, m_salaList, this);
		reservaItem->setSala(m_activeReserva->getSALAID());
		
		if (m_activeReserva->getTIPO() == 'S')
			reservaItem->setDate(m_activeReserva->getDATA());
		else
		{
			reservaItem->setDate(m_activeReserva->getDATAIN());
			reservaItem->setDateFim(m_activeReserva->getDATAFIM());
		}

		reservaItem->setModal(true);
		reservaItem->show();
	}
}

void CMainWindow::refreshSalas()
{
	delete m_salaList;
	m_salaList = 0;
	refreshData(m_date);
}

void CMainWindow::on_btAnte_clicked()
{
	refreshData(m_date.addDays(-7));
}

void CMainWindow::on_btProx_clicked()
{
	refreshData(m_date.addDays(7));
}

void CMainWindow::setActiveReserva(CReservaList::CReserva *_reserva)
{
	m_activeReserva = _reserva;
	
	actionImprimirLista->setEnabled(m_activeReserva);
	actionImprimirReserva->setEnabled(m_activeReserva);
	actionCopiar->setEnabled(m_activeReserva);
	
	switch (m_config->getNivel())
	{
		case 0:
		{
			return;
		}
		case 1:
		{
			bool benabled = (_reserva) && (_reserva->getUSUARIOID() == m_config->getUsuarioID());
			
			pbRemover->setEnabled(benabled);
			actionRemover->setEnabled(benabled);
			actionAlterar->setEnabled(benabled);
			return;
		}
		default:
		{
			pbRemover->setEnabled(_reserva);
			actionRemover->setEnabled(_reserva);
			actionAlterar->setEnabled(_reserva);
			return;
		}
	}
}

void CMainWindow::showReservaMenu(const QPoint _pos)
{
	m_mnPopupReserva.popup(_pos);
}

void CMainWindow::on_actionImprimirLista_activated()
{
	QPrintDialog printDialog(&m_printer, this);
	if (printDialog.exec() == QDialog::Accepted)
	{
		CSala *sala = m_salaList->m_salas[m_activeReserva->getSALAID()];

		CModelos *modelos = new CModelos(m_data.m_db, m_activeReserva->getOwner(),
										 sala->getNome(), &m_printer);

		modelos->setModal(true);
		modelos->show();
	};
}

void CMainWindow::on_actionImprimirReserva_activated()
{
	QPrintDialog printDialog(&m_printer, this);
	if (printDialog.exec() == QDialog::Accepted)
	{
		CSala *sala = m_salaList->m_salas[m_activeReserva->getSALAID()];

		CModelos *modelos = new CModelos(m_data.m_db, m_activeReserva,
										 sala->getNome(), &m_printer);

		modelos->setModal(true);
		modelos->show();
	};
}

void CMainWindow::on_actionCopiar_activated()
{
	QString str;
	char sep = 9;
	
	CReservaList::TReservaList::iterator it;

	CReservaList* reservaList = m_activeReserva->getOwner();

	for (it = reservaList->m_reservas.begin(); it != reservaList->m_reservas.end(); ++it)
	{
		str += (*it)->getHORAIN().toString();
		str += sep;
		str += (*it)->getHORAFIM().toString();
		str += sep;
		str += (*it)->getASSUNTO();
		str += sep;
		str += (*it)->getDEPTO();
		str += sep;
		str += (*it)->getNOTAS();
		str += "\n";
	}
	
	QApplication::clipboard()->setText(str);
}

void CMainWindow::on_actionHoje_activated()
{
	QDate date = QDate::currentDate();
	date = date.addDays(date.dayOfWeek() * -1 + 1);
	refreshData(date);
}
