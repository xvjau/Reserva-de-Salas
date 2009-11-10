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

#ifndef CCOMBOBOXDELEGATE_H
#define CCOMBOBOXDELEGATE_H

#include <QItemDelegate>
#include <QAbstractItemView>

#include "main.h"
#include "CData.h"

/**
	@author Gianni Rossi <gianni.rossi@gmail.com>
*/
class CUsuarioDelegate: public QItemDelegate
{
		Q_OBJECT

	public:
		CUsuarioDelegate ( CData * _data, QObject *_parent = 0 );

	private:
		QStringList		* m_items;
		CData 			* m_data;

	public:
		virtual QWidget * createEditor ( QWidget * parent, const QStyleOptionViewItem & option,
		                                 const QModelIndex & index ) const;

		virtual void setEditorData ( QWidget * editor, const QModelIndex & index ) const;
		virtual void setModelData ( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const;
};

#endif
