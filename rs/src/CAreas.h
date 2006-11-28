#ifndef CAREAS_H_
#define CAREAS_H_

#include <QDialog>
#include "../obj/ui_frareas.h"

class CAreas: public QDialog, public Ui::areas
{
	Q_OBJECT
	
	public:
		CAreas();
		~CAreas();
};

#endif /*CAREAS_H_*/
