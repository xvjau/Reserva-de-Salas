//
// C++ Implementation: CComboBoxDelegate
//
// Description: 
//
//
// Author: Gianni Rossi <gianni.rossi@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "CComboBoxDelegate.h"
#include <QtGui/QComboBox>

CComboBoxDelegate::CComboBoxDelegate(QObject *_parent):
	QItemDelegate(_parent)
{
	
}

QWidget * CComboBoxDelegate::createEditor ( QWidget * parent, const QStyleOptionViewItem & option,
									const QModelIndex & index ) const
{
	if (index.column() == 5)
	{
		QComboBox *editor = new QComboBox(parent);
		
		editor->installEventFilter(const_cast<CComboBoxDelegate*>(this));
	
		QStringList stringList = index.model()->data(index, Qt::LookUpRole).toStringList();
		editor->addItems(stringList);

		return editor;
	}
	else
		return static_cast<const QItemDelegate*>(this)->createEditor(parent, option, index);
}

void CComboBoxDelegate::setEditorData ( QWidget * editor, const QModelIndex & index ) const
{
	if (index.column() == 5)
	{
		QComboBox *comboBox = static_cast<QComboBox*>(editor);

		int listIndex = comboBox->findText( index.model()->data(index, Qt::DisplayRole).toString() );
		comboBox->setCurrentIndex(listIndex);
	}
	else
		static_cast<const QItemDelegate*>(this)->setEditorData(editor, index);
}

void CComboBoxDelegate::setModelData ( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const
{
	if (index.column() == 5)
	{
		QComboBox *comboBox = static_cast<QComboBox*>(editor);

		model->setData(index, comboBox->currentText());
	}
	else
		static_cast<const QItemDelegate*>(this)->setEditorData(editor, index);
}

