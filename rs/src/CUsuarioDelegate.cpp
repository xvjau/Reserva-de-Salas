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
#include "CEsquemaComboBox.h"
#include <QComboBox>

CUsuarioDelegate::CUsuarioDelegate ( CData * _data, QObject *_parent ) :
		QItemDelegate ( _parent ),
		m_data ( _data )
{}

QWidget * CUsuarioDelegate::createEditor ( QWidget * parent, const QStyleOptionViewItem & option,
        const QModelIndex & index ) const
{
	switch ( index.column() )
	{
		case 3: // Esquema de Cores
		{
			CEsquemaComboBox * editor = new CEsquemaComboBox ( m_data, parent );
			editor->installEventFilter ( const_cast<CUsuarioDelegate*> ( this ) );

			return editor;
		}
		case 4: // Nivel
		{
			QComboBox * editor = new QComboBox ( parent );
			editor->installEventFilter ( const_cast<CUsuarioDelegate*> ( this ) );

			editor->addItem ( tr ( "0 - Somente leitura" ) );
			editor->addItem ( tr ( "1 - Criar reservas" ) );
			editor->addItem ( tr ( "2 - Criar, Alterar reservas de qualquer usuário" ) );
			editor->addItem ( tr ( "3 - Administrador(a)" ) );

			return editor;
		}
		case 5: // Areas
		{
			CUsuariosAreas * editor = new CUsuariosAreas ( m_data, parent );
			editor->installEventFilter ( const_cast<CUsuarioDelegate*> ( this ) );
			return editor;
		}
		default:
			return QItemDelegate::createEditor ( parent, option, index );
	}
}

void CUsuarioDelegate::setEditorData ( QWidget * editor, const QModelIndex & index ) const
{
	switch ( index.column() )
	{
		case 3: // Esquema de Cores
		{
			CEsquemaComboBox * combo = static_cast<CEsquemaComboBox*> ( editor );
			int i = combo->findText ( index.data ( Qt::DisplayRole ).toString() );
			combo->setCurrentIndex ( i );
			break;
		}
		case 4: // Nivel
		{
			QComboBox * combo = static_cast<QComboBox*> ( editor );
			combo->setCurrentIndex ( index.data ( Qt::DisplayRole ).toInt() );
			break;
		}
		case 5: // Areas
		{
			CUsuariosAreas * form = static_cast<CUsuariosAreas*> ( editor );
			form->setUserAreas ( index.data ( Qt::DisplayRole ).toString() );
			break;
		}
		default:
			QItemDelegate::setEditorData ( editor, index );
	}
}

void CUsuarioDelegate::setModelData ( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const
{
	switch ( index.column() )
	{
		case 3: // Esquema de Cores
		{
			CEsquemaComboBox * combo = static_cast<CEsquemaComboBox*> ( editor );
			model->setData ( index, combo->currentText() );
			break;
		}
		case 4: // Nivel
		{
			QComboBox * combo = static_cast<QComboBox*> ( editor );
			model->setData ( index, combo->currentIndex() );
			break;
		}
		case 5: // Areas
		{
			CUsuariosAreas * form = static_cast<CUsuariosAreas*> ( editor );
			model->setData ( index, form->getUserAreas() );
			break;
		}
		default:
			QItemDelegate::setModelData ( editor, model, index );
	}
}

