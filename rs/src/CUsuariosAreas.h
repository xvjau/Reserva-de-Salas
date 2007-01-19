#ifndef CUSUARIOSAREAS_H_
#define CUSUARIOSAREAS_H_

#include <QWidget>
#include <QDialog>
#include "../obj/ui_frusersareas.h"

class CUsuariosAreas: public QDialog, public Ui::frUsersAreas
{
	Q_OBJECT
	
	public:
		CUsuariosAreas( QWidget * _parent );
		virtual ~CUsuariosAreas();
};

#endif /*CUSUARIOSAREAS_H_*/
