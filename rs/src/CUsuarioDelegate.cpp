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
		return QItemDelegate::createEditor(parent, option, index);
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
		QItemDelegate::setEditorData(editor, index);
}

void CComboBoxDelegate::setModelData ( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const
{
	if (index.column() == 5)
	{
		QComboBox *comboBox = static_cast<QComboBox*>(editor);

		model->setData(index, comboBox->currentText());
	}
	else
		QItemDelegate::setModelData(editor, model, index);
}

