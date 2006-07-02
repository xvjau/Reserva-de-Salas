#ifndef __INCLUDE_CRESERVAITEM_H
#define __INCLUDE_CRESERVAITEM_H

#include "main.h"
#include "../obj/ui_frreserva.h"
#include "CData.h"

class CReservaItem : public QDialog, public Ui::Reserva
{
	Q_OBJECT
	
	public:
		CReservaItem(CReservaList::CReserva *_reserva, CSalaList *_salas,
					CMainWindow *_form);
		virtual ~CReservaItem();
		
		void setSala(int _sala);
		void setDate(QDate _date);
		void setDateFim(QDate _date);
		void setTime(QTime _time);
		
	private:
		QGridLayout			*glayRecorrente;
		
		CReservaList::CReserva		*m_reserva;
		CReservaList				*m_list;
		CSalaList					*m_salas;
		CMainWindow					*m_form;
		
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
