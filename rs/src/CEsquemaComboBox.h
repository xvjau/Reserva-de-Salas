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

#ifndef CESQUEMACOMBOBOX_H_
#define CESQUEMACOMBOBOX_H_

#include "CData.h"
#include <QComboBox>

class CEsquemaComboBox: public QComboBox
{
		Q_OBJECT

	public:
		CEsquemaComboBox ( CData * _data, QWidget * parent = 0 );
		virtual ~CEsquemaComboBox();
};

class CEsquemaDelegate : public QAbstractItemDelegate
{
		Q_OBJECT

	public:
		CEsquemaDelegate ( CData * _data, QObject * parent );

	private:
		CData * m_data;

	public:
		// painting
		virtual void paint ( QPainter * painter, const QStyleOptionViewItem & option,
		                     const QModelIndex & index ) const;

		virtual QSize sizeHint ( const QStyleOptionViewItem & option,
		                         const QModelIndex & index ) const;
};

#endif /*CESQUEMACOMBOBOX_H_*/
