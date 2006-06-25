#ifndef __INCLUDE_CMODELOSITEM_H
#define __INCLUDE_CMODELOSITEM_H

#include "CData.h"
#include "../obj/ui_frmodelositem.h"

class CModelos;

class CModelosItem: public QDialog, public Ui::ModeloItem
{
	Q_OBJECT

	public:
		CModelosItem(Database _db, CModelos *_owner);
		~CModelosItem();

	private:
		Database	m_db;
		int			m_ModeloID;
		CModelos	*m_owner;
		
	public:
		void setModeloID(const int _value) {m_ModeloID = _value;};
		
	public slots:
		void onAccept();
		void onClose();
};

#endif //__INCLUDE_CMODELOSITEM_H
