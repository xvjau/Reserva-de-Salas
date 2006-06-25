#ifndef __INCLUDE_CSALASITEM_H
#define __INCLUDE_CSALASITEM_H

#include "main.h"
#include "../obj/ui_frsalasitem.h"
#include "CData.h"

#include <QtGui/QIntValidator>

class CSalasItem: public QDialog, public Ui::SalasItem
{
	Q_OBJECT
	
	public:
        CSalasItem(PSala _sala, CSalas *_form);
		~CSalasItem();
		
	private:
        PSala m_sala;
        
		QIntValidator m_validAndar;
		QIntValidator m_validSalaID;
		
	private slots:
        void onClose();
        void ok();
        void cancel();
        
    public slots:
		void onValidate();
};

#endif //__INCLUDE_CSALASITEM_H
