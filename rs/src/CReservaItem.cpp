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
#include "CReserva.h"
#include "CSalaList.h"
#include "CConfig.h"
#include "CSala.h"

CReservaItem::CReservaItem(CReserva *_reserva,
CSalaList *_salas, CMainWindow *_form):
	QDialog( _form ), 
	m_reserva(_reserva),
	m_salas(_salas),
	m_form(_form)
{
	setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);

	glayRecorrente = new QGridLayout(frameRecorrente);
	glayRecorrente->addLayout(hboxLayout5, 0, 0, 1, 1);

	QString dow;
	#define setDOWCaption(INT) \
		dow = QDate::longDayName( INT ); \
		cbSemanal ## INT ->setText(dow); \
		cbMensalDOW->addItem(dow);

	setDOWCaption(1);
	setDOWCaption(2);
	setDOWCaption(3);
	setDOWCaption(4);
	setDOWCaption(5);
	setDOWCaption(6);
	setDOWCaption(7);
	
	connect(this, SIGNAL(rejected()), this, SLOT(cancel()));
	connect(this, SIGNAL(rejected()), this, SLOT(onClose()));

	m_list = _reserva->getOwner();
	connect(okButton, SIGNAL(clicked()), this, SLOT(ok()));
	connect(this, SIGNAL(accepted()), this, SLOT(onClose()));

	connect(teHoraIN, SIGNAL(timeChanged(QTime)), this, SLOT(onValidate()));
	connect(teHoraFim, SIGNAL(timeChanged(QTime)), this, SLOT(onValidate()));
	connect(leAssunto, SIGNAL(textChanged(QString)), this, SLOT(onValidate()));
	
	connect(cbRecorrente, SIGNAL(clicked()), this, SLOT(onValidate()));
	connect(cbSemanal1, SIGNAL(clicked()), this, SLOT(onValidate()));
	connect(cbSemanal2, SIGNAL(clicked()), this, SLOT(onValidate()));
	connect(cbSemanal3, SIGNAL(clicked()), this, SLOT(onValidate()));
	connect(cbSemanal4, SIGNAL(clicked()), this, SLOT(onValidate()));
	connect(cbSemanal5, SIGNAL(clicked()), this, SLOT(onValidate()));
	connect(cbSemanal6, SIGNAL(clicked()), this, SLOT(onValidate()));
	connect(cbSemanal7, SIGNAL(clicked()), this, SLOT(onValidate()));
	connect(deDataTermino, SIGNAL(dateChanged(QDate)), this, SLOT(onValidate()));

	m_form->setCanRefresh(false);
	
	PSala sala;

	CConfig *config = CConfig::getConfig();
	 
	CSalaList::TSalaList::iterator it;
	for(it = m_salas->m_salas.begin();
		   it != m_salas->m_salas.end();
		   ++it)
	{
		sala = *it;
		if ((config->getNivel() == 3) || (config->getUserSalaList()->indexOf(sala->getSalaID()) != -1))
			cbSala->addItem(sala->getNome(), sala->getSalaID());
	}
	
	cbSala->setCurrentIndex(cbSala->findData(sala->getSalaID()));
	
	teHoraIN->setTime(m_reserva->getHORAIN());
	teHoraFim->setTime(m_reserva->getHORAFIM());
	leAssunto->setText(m_reserva->getASSUNTO());
	leDepto->setText(m_reserva->getDEPTO());
	teNotas->document()->setPlainText(m_reserva->getNOTAS());

	if (m_reserva->getTIPO() == 'S')
		cbRecorrente->setCheckState(Qt::Unchecked);
	else
		cbRecorrente->setCheckState(Qt::Checked);
	
	if (m_reserva->getTIPO() == 'W')
	{
		cbSemanal1->setCheckState(m_reserva->getDOW(0) ? Qt::Checked : Qt::Unchecked);
		cbSemanal2->setCheckState(m_reserva->getDOW(1) ? Qt::Checked : Qt::Unchecked);
		cbSemanal3->setCheckState(m_reserva->getDOW(2) ? Qt::Checked : Qt::Unchecked);
		cbSemanal4->setCheckState(m_reserva->getDOW(3) ? Qt::Checked : Qt::Unchecked);
		cbSemanal5->setCheckState(m_reserva->getDOW(4) ? Qt::Checked : Qt::Unchecked);
		cbSemanal6->setCheckState(m_reserva->getDOW(5) ? Qt::Checked : Qt::Unchecked);
		cbSemanal7->setCheckState(m_reserva->getDOW(6) ? Qt::Checked : Qt::Unchecked);
			
		cbSemanalSN->setCheckState(m_reserva->getREVESA() ? Qt::Checked : Qt::Unchecked);
		
		deDataTermino->setDate(m_reserva->getDATAFIM());
	}
	else
	{
		cbSemanal1->setCheckState(Qt::Unchecked);
		cbSemanal2->setCheckState(Qt::Unchecked);
		cbSemanal3->setCheckState(Qt::Unchecked);
		cbSemanal4->setCheckState(Qt::Unchecked);
		cbSemanal5->setCheckState(Qt::Unchecked);
		cbSemanal6->setCheckState(Qt::Unchecked);
		cbSemanal7->setCheckState(Qt::Unchecked);
		
		cbSemanalSN->setCheckState(Qt::Unchecked);
		
		deDataTermino->setDate(QDate::currentDate().addMonths(6));
	}

	/*
	 * TODO: Add ReservaMesal support to Database.
	 */
	cbTipoRecorrencia->setVisible( false );
	
	onValidate();
	updateRecorrencia();
}

CReservaItem::~CReservaItem()
{
	delete glayRecorrente;
}

void CReservaItem::setSala(int _sala)
{
	cbSala->setCurrentIndex(cbSala->findData(_sala));
}

void CReservaItem::setDate(QDate _date)
{
	deData->setDate(_date);
}

void CReservaItem::setDateFim(QDate _date)
{
	deDataTermino->setDate(_date);
}

void CReservaItem::setTime(QTime _time)
{
	teHoraIN->setTime(_time);
	teHoraFim->setTime(_time);
}

void CReservaItem::onClose()
{
	m_form->setCanRefresh(true);
	m_form->checkRefresh();
	close();
}

void CReservaItem::ok()
{
	m_reserva->setSALAID(cbSala->itemData(cbSala->currentIndex()).value<int>());
	m_reserva->setDATA(deData->date());

	m_reserva->setHORAIN(teHoraIN->time());
	m_reserva->setHORAFIM(teHoraFim->time());

	m_reserva->setUSUARIOID(CConfig::getConfig()->getUsuarioID());

	m_reserva->setASSUNTO(leAssunto->text());
	m_reserva->setDEPTO(leDepto->text());
	m_reserva->setNOTAS(teNotas->document()->toPlainText());
	
	if (cbRecorrente->checkState() == Qt::Checked)
	{
		m_reserva->setDATAFIM(deDataTermino->date());
	
		if (cbTipoRecorrencia->currentIndex() == 0)
		{
			m_reserva->setTIPO('W');
			
			#define setReservaDOW(INT) \
				m_reserva->setDOW(INT - 1, cbSemanal ## INT ->checkState() == Qt::Checked);
			
			setReservaDOW(1);
			setReservaDOW(2);
			setReservaDOW(3);
			setReservaDOW(4);
			setReservaDOW(5);
			setReservaDOW(6);
			setReservaDOW(7);
		
			m_reserva->setREVESA(cbSemanalSN->checkState() == Qt::Checked);
		}
		else
		{
			m_reserva->setTIPO('M');
		}
	}
	else
		m_reserva->setTIPO('S');

	try
	{
		if (m_reserva->save())
		{
			if (m_reserva->getTIPO() == 'W')
				m_form->refreshData(m_form->getDate());
			else
				m_form->checkRowHeight(deData->date().dayOfWeek()-1, m_reserva->getSALAID());
		}
	
		emit accepted();
	}
	catch (int SQLCode)
	{
		std::cerr << "SQLCode:" << SQLCode << std::endl; 
		if (SQLCode == -836)
		{
			QMessageBox msg("Erro", "J&aacute; existe uma reserva<br>nessa sala neste hor&aacute;rio.", QMessageBox::Warning, QMessageBox::Cancel, 0, 0);
			msg.setWindowIcon(QIcon(":/png/buttons/16/agt_update_critical.png"));
			msg.setIconPixmap(QPixmap(":/png/buttons/16/kopete016.png"));
			msg.exec();
		}
		else
		{
			QMessageBox("Erro", QString("La maison est tombe&eacute;<br>SQLError:") + QString::number(SQLCode), QMessageBox::Warning, QMessageBox::Cancel, 0, 0).exec();
		}
	}
}

void CReservaItem::cancel()
{
	if (m_reserva->getoldRESERVAID() == -1)
		m_reserva->getOwner()->m_reservas.removeAll(m_reserva);
}

void CReservaItem::onValidate()
{
	bool validState = leAssunto->text().length() &&
			(cbSala->currentIndex() != -1) &&
			(teHoraFim->time() > teHoraIN->time());
	
	if (validState && (cbRecorrente->checkState() == Qt::Checked))
	{
		if (cbTipoRecorrencia->currentIndex() == 0)
		{
			validState = (deDataTermino->date() >= deData->date().addDays(1)) &&
					((cbSemanal1->checkState() == Qt::Checked) ||
					(cbSemanal2->checkState() == Qt::Checked) ||
					(cbSemanal3->checkState() == Qt::Checked) ||
					(cbSemanal4->checkState() == Qt::Checked) ||
					(cbSemanal5->checkState() == Qt::Checked) ||
					(cbSemanal6->checkState() == Qt::Checked) ||
					(cbSemanal7->checkState() == Qt::Checked));
		}
		else 
			validState = false;
	}
		
	okButton->setEnabled(validState);
}

void CReservaItem::updateRecorrencia()
{
	CUpdateLock lock(frameRecorrente);
	
	frameRecorrente->setEnabled(cbRecorrente->checkState() == Qt::Checked);
		
	if (cbTipoRecorrencia->currentText() == QString::fromUtf8("Semanal"))
	{
		frameSemanal->setVisible(true);
		frameMensal->setVisible(false);

		glayRecorrente->addWidget(frameSemanal);
		glayRecorrente->removeWidget(frameMensal);
		
		if (deDataTermino->date() < deData->date())
			deDataTermino->setDate(deData->date().addMonths(6));
	}
	else
	{
		frameSemanal->setVisible(false);
		frameMensal->setVisible(true);

		glayRecorrente->removeWidget(frameSemanal);
		glayRecorrente->addWidget(frameMensal);
	}
}

void CReservaItem::on_cbRecorrente_stateChanged(int state)
{
	updateRecorrencia();
}

void CReservaItem::on_cbTipoRecorrencia_currentIndexChanged(int index)
{
	updateRecorrencia();
}
