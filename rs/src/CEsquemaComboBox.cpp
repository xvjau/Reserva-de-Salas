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

#include "CEsquemaComboBox.h"
#include "CSchemasModel.h"

#include <QPainter>
#include <QPalette>

#include <math.h>

CEsquemaDelegate::CEsquemaDelegate ( CData * _data, QObject * parent ) :
		QAbstractItemDelegate ( parent ),
		m_data ( _data )
{}

QSize CEsquemaDelegate::sizeHint ( const QStyleOptionViewItem &option,
                                   const QModelIndex &index ) const
{
	QString text = tr ( "Esquema " ) + index.data ( Qt::DisplayRole ).toString();
	QFontMetrics fontMetrics ( option.font );
	return QSize ( fontMetrics.width ( text ), lround ( fontMetrics.lineSpacing() * 1.5 ) );
}


void CEsquemaDelegate::paint ( QPainter *painter,
                               const QStyleOptionViewItem &option,
                               const QModelIndex &index ) const
{
	QString text = tr ( "Esquema " ) + index.data ( Qt::DisplayRole ).toString();

	QRect r = option.rect;

	if ( option.state & QStyle::State_Selected )
	{
		painter->save();
		painter->setBrush ( option.palette.highlight() );
		painter->setPen ( Qt::NoPen );
		painter->drawRect ( option.rect );
		painter->setPen ( QPen ( option.palette.highlightedText(), 0 ) );
	}
	else
	{
		painter->save();

		QPalette * palette = m_data->getColorScheme ( index.data ( Qt::DisplayRole ).toInt() );

		painter->setBrush ( palette->brush ( QPalette::Base ) );
		painter->setPen ( Qt::NoPen );
		painter->drawRect ( option.rect );
		painter->setPen ( QPen ( palette->brush ( QPalette::Text ), 0 ) );
	}
	painter->drawText ( r, Qt::AlignVCenter|Qt::AlignLeading|Qt::TextSingleLine, text );


	if ( option.state & QStyle::State_Selected )
		painter->restore();
}


CEsquemaComboBox::CEsquemaComboBox ( CData * _data, QWidget * parent ) :
		QComboBox ( parent )
{
	CSchemasModel model ( _data );

	for ( int i = 0; i < model.rowCount(); ++i )
	{
		addItem ( model.headerData ( i, Qt::Vertical ).toString() );
	}

	setItemDelegate ( new CEsquemaDelegate ( _data, this ) );
}

CEsquemaComboBox::~CEsquemaComboBox()
{}
