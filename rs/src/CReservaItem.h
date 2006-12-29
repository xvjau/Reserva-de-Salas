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

#ifndef __INCLUDE_CRESERVAITEM_H
#define __INCLUDE_CRESERVAITEM_H

#include "main.h"
#include "../obj/ui_frreserva.h"
#include "CData.h"
#include "CReserva.h"
#include "CReservaList.h"
#include "CSalaList.h"

class CReservaItem : public QDialog, public Ui::Reserva
{
	Q_OBJECT
	
	public:
		CReservaItem(CReserva *_reserva, CSalaList *_salas,
					CMainWindow *_form);
		virtual ~CReservaItem();
		
		void setSala(int _sala);
		void setDate(QDate _date);
		void setDateFim(QDate _date);
		void setTime(QTime _time);
		
	private:
		QGridLayout			*glayRecorrente;
		
		CReserva		*m_reserva;
		CReservaList	*m_list;
		CSalaList		*m_salas;
		CMainWindow		*m_form;
		
		void updateRecorrencia();
	
	private slots:
		void onClose();
		void ok();
		void cancel();
		
	public slots:
		void onValidate();

		void on_cbRecorrente_stateChanged(int state);
		void on_cbTipoRecorrencia_currentIndexChanged(int index);
};

#endif // __INCLUDE_CRESERVAITEM_H
