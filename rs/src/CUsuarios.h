#ifndef __INCLUDE_CUSUARIOS_H
#define __INCLUDE_CUSUARIOS_H

#include "main.h"
#include "../obj/ui_frusers.h"
#include "CData.h"
#include "CUsuariosModel.h"

#include <QtGui/QIntValidator>

class CUsuarios : public QDialog, public Ui::frUsers
{
	Q_OBJECT
	
	public:
		CUsuarios(CData* _data);
		~CUsuarios();
		
	private:
		CData			*m_data;
		CUsuariosModel  *m_model;
		
	private slots:
		void onAccept();
};

#endif // __INCLUDE_CUSUARIOS_H
