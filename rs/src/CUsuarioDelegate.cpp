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

#include "CUsuarioDelegate.h"
#include "CUsuariosModel.h"
#include "CUsuariosAreas.h"

CUsuarioDelegate::CUsuarioDelegate( CData * _data, QObject *_parent ):
	QItemDelegate(_parent),
	m_data( _data )
{
	
}

QWidget * CUsuarioDelegate::createEditor ( QWidget * parent, const QStyleOptionViewItem & option,
									const QModelIndex & index ) const
{
	if (index.column() == 5)
	{
		CUsuariosAreas * editor = new CUsuariosAreas( m_data, parent );
		editor->installEventFilter(const_cast<CUsuarioDelegate*>(this));
		return editor;
	}
	else
		return QItemDelegate::createEditor(parent, option, index);
}

void CUsuarioDelegate::setEditorData ( QWidget * editor, const QModelIndex & index ) const
{
	if ( index.column() == 5 )
	{
		CUsuariosAreas * form = static_cast<CUsuariosAreas*>(editor);
		form->setUserAreas( index.model()->data(index, Qt::DisplayRole).toString() );
	}
	else
		QItemDelegate::setEditorData(editor, index);
}

void CUsuarioDelegate::setModelData ( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const
{
	if ( index.column() == 5 )
	{
		CUsuariosAreas * form = static_cast<CUsuariosAreas*>(editor);
		model->setData( index, form->getUserAreas() );
	}
	else
		QItemDelegate::setModelData(editor, model, index);
}

