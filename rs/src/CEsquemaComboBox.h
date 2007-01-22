#ifndef CESQUEMACOMBOBOX_H_
#define CESQUEMACOMBOBOX_H_

#include "CData.h"
#include <QComboBox>

class CEsquemaComboBox: public QComboBox
{
	Q_OBJECT
	
	public:
		CEsquemaComboBox( CData * _data, QWidget * parent = 0 );
		virtual ~CEsquemaComboBox();
};

#endif /*CESQUEMACOMBOBOX_H_*/
