//
// C++ Interface: CComboBoxDelegate
//
// Description: 
//
//
// Author: Gianni Rossi <gianni.rossi@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CCOMBOBOXDELEGATE_H
#define CCOMBOBOXDELEGATE_H

#include <QtGui/QItemDelegate>
#include "main.h"

/**
	@author Gianni Rossi <gianni.rossi@gmail.com>
*/
class CComboBoxDelegate: public QItemDelegate
{
	Q_OBJECT
	
	public:
		CComboBoxDelegate(QObject *_parent = 0);

	private:
		QStringList	*m_items;

	public:
		virtual QWidget * createEditor ( QWidget * parent, const QStyleOptionViewItem & option,
										 const QModelIndex & index ) const;

		virtual void setEditorData ( QWidget * editor, const QModelIndex & index ) const;
		virtual void setModelData ( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const;
};

#endif
