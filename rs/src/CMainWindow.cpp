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

#include "CMainWindow.h"
#include "CReservaItem.h"
#include "CSalas.h"
#include "CUsuarios.h"
#include "CSchemas.h"
#include "CModelos.h"
#include "CDBSettings.h"
#include "CAreas.h"
#include "CReservaList.h"
#include "CSalaList.h"

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
	m_needRefresh(true),
	m_initialized(false)
{
	setupUi(this);
	
	#ifndef __unix__
	QFont font(lbData->font());
	font.setFamily(QString::fromUtf8("Arial"));
	lbData->setFont(font);
	#endif

	if ( ! connect(pbAdicionar, SIGNAL(clicked()), actionAdicionar, SIGNAL(triggered())) )
		std::cout << "Unable to connect pbAdcionar(clicked()) -> actionAdcionar(triggered())" << std::endl;
		
	if ( ! connect(pbRemover, SIGNAL(clicked()), actionRemover, SIGNAL(triggered())) )
		std::cout << "Unable to connect pbRemover(clicked()) -> actionRemover(triggered())" << std::endl;
	
	m_mnPopupReserva.addAction(actionImprimirLista);
	m_mnPopupReserva.addAction(actionCopiar);
	
	//m_mnPopupReserva.addAction(actionImprimirReserva);
	
	m_mnPopupHoje.addAction(actionHoje);
	
	initialize();
}

CMainWindow::~CMainWindow()
{
	m_config->setLastArea( cbArea->currentText() );
			
	if (m_salaList)
		delete m_salaList;

	clearData();
}

void CMainWindow::initialize()
{
	if (m_initialized)
		return;
		
	if (! m_data.connect())
	{
		CDBSettings *dbsettings = new CDBSettings(this);
					
		dbsettings->setModal(true);
		dbsettings->show();
		return;
	}

	m_config = CConfig::getConfig(&m_data);

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

	refreshAreas();
	int index = cbArea->findText( m_config->getLastArea() );
	if (index >= 0)
		cbArea->setCurrentIndex( index );
	
	m_date = QDate::currentDate();
	m_date = m_date.addDays(m_date.dayOfWeek() * -1 + 1);
	refreshData(m_date);
	
	switch (m_config->getNivel())
	{
		case 0:
		{
			delete pbAdicionar;
			delete pbRemover;
			pbAdicionar = 0;
			pbRemover = 0;
		}
		case 1:
		case 2:
		{
			delete menuMenu;
			menuMenu = 0;

			if (m_config->getUserSalaList()->count() == 0)
			{
				delete pbAdicionar;
				delete pbRemover;
				pbAdicionar = 0;
				pbRemover = 0;
			}
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
	
	bool userHasArea = (m_config->getNivel() == 3) || (m_data.getAreaId( cbArea->currentIndex() ) == CConfig::getConfig()->getUserAreaID());
	
	actionAdicionar->setEnabled(userHasArea);

	if (pbAdicionar)
		pbAdicionar->setEnabled(userHasArea);
	
	connect(cbArea, SIGNAL(currentIndexChanged(int)), this, SLOT(cbAreaChanged(int)));
	
	m_initialized = true;
	resizeEvent( 0 );
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
	int iWidth = tbReservas->verticalHeader()->width();

	if (tbReservas->verticalScrollBar())
		iWidth += tbReservas->verticalScrollBar()->visibleRegion().boundingRect().width();

    if (tbReservas->columnCount())
    	iWidth = ((tbReservas->width() - iWidth - 4) / tbReservas->columnCount()) - 2;
	else
		iWidth = 0;
	
	if (iWidth < 170)
		iWidth = 170;
	
	for(int i = 0; i < tbReservas->columnCount(); ++i)
		tbReservas->setColumnWidth(i, iWidth);
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
	CWaitCursor waitCursor();

	setActiveReserva(0);
	
	clearData();
	
	lbData->setText(m_date.toString("dd/MM/yyyy"));
	
	if (! m_salaList)
	{
		m_salaList = new CSalaList(&m_data, m_data.getAreaId( cbArea->currentIndex() ));
		m_salaList->loadList();
	}
	
	QTableWidgetItem* item;
	{
		CSalaList::TSalaList::iterator it;
		
		tbReservas->setColumnCount(m_salaList->m_salas.size());
		tbReservas->setRowCount(7);
		
		btProx->setEnabled(m_salaList->m_salas.size());
		btAnte->setEnabled(m_salaList->m_salas.size());
		if (pbAdicionar)
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
			
			nomeSala = sala->getNome().length() ? sala->getNome() : QString("Sala ") + QString::number(sala->getSalaID());
			
			item = tbReservas->horizontalHeaderItem(icol);

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
			int iareaId = m_data.getAreaId( cbArea->currentText() );
			m_semana = new CSemana(this, m_date, &m_data, m_salaList, iareaId);
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

void CMainWindow::on_actionSalas_triggered()
{
	CSalas *salas = new CSalas(this, &m_data);
	salas->setModal(true);
	salas->show();
}

void CMainWindow::on_actionUsuarios_triggered()
{
	CUsuarios *usuarios = new CUsuarios(&m_data);
	usuarios->setModal(true);
	usuarios->show();
}

void CMainWindow::on_actionCores_triggered()
{
	CSchemas *schemas = new CSchemas(&m_data);
	schemas->setModal(true);
	schemas->show();
}

void CMainWindow::on_actionAdicionar_triggered()
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

void CMainWindow::on_actionRemover_triggered()
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

void CMainWindow::on_actionAlterar_triggered()
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
	resizeEvent( 0 );
}

void CMainWindow::refreshAreas()
{
	if (cbArea->count())
		cbArea->clear();
	
	cbArea->addItems(*m_data.refreshAreas());
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
			bool benabled = false;
			bool userHasArea = false;

			if (_reserva && (tbReservas->currentColumn() != -1))
			{
				int isalaid = tbReservas->horizontalHeaderItem(tbReservas->currentColumn())->data(PSALA_COL_ROLE).toInt();
				userHasArea = (m_config->getUserSalaList()->indexOf(isalaid) != -1);
				
				benabled = (_reserva->getUSUARIOID() == m_config->getUsuarioID()) && userHasArea;
			}

			if (pbRemover)
				pbRemover->setEnabled(benabled);
			
			actionRemover->setEnabled(benabled);
			actionAlterar->setEnabled(benabled);
			
			return;
		}
		case 2:
		{
			bool benabled = false;
			bool userHasArea = false;

			if (_reserva && (tbReservas->currentColumn() != -1))
			{
				int isalaid = tbReservas->horizontalHeaderItem(tbReservas->currentColumn())->data(PSALA_COL_ROLE).toInt();
				userHasArea = (m_config->getUserSalaList()->indexOf(isalaid) != -1);

				benabled = userHasArea;
			}

			if (pbRemover)
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

void CMainWindow::on_actionImprimirLista_triggered()
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

void CMainWindow::on_actionImprimirReserva_triggered()
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

void CMainWindow::on_actionCopiar_triggered()
{
	QString str;
	const char sep = 9;
	
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

void CMainWindow::on_actionHoje_triggered()
{
	QDate date = QDate::currentDate();
	date = date.addDays(date.dayOfWeek() * -1 + 1);
	refreshData(date);
}

void CMainWindow::on_actionAreas_triggered()
{
	CAreas *areas = new CAreas(&m_data, this);
	areas->setModal(true);
	areas->show();
}

void CMainWindow::cbAreaChanged(int index)
{
	refreshSalas();
	refreshData(m_activeDate);
	resizeEvent(0);

	bool userHasArea = m_data.getAreaId( cbArea->currentIndex() ) == CConfig::getConfig()->getUserAreaID();
			
	actionAdicionar->setEnabled(userHasArea);

	if (pbAdicionar)
		pbAdicionar->setEnabled(userHasArea);

}
