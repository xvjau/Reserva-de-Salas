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

#include "CReservaList.h"
#include "CSemana.h"
#include "CConfig.h"
#include "CReservaItem.h"
#include "CSala.h"

CReservaList::CReservaList(CSemana *_owner, QDate _date, int _salaID):
	QFrame( _owner->getParent() ),
	m_owner( _owner ),
	m_date( _date ),
	m_salaID( _salaID )

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

CReserva* CReservaList::addReserva()
{
	CReserva *result = new CReserva(this);
	m_reservas.push_back(result);

	result->m_colorScheme = m_owner->m_owner->getColorScheme(CConfig::getConfig()->getColorScheme());
	return result;
}

void CReservaList::insertReserva(CReserva* _reserva)
{
	CReserva* reserva;
	TListaReserva::iterator it;
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
		case 1:
		case 2:
		{
			if ( CConfig::getConfig()->getUserSalaList()->indexOf(m_salaID) == -1 )
				return;
		}
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

