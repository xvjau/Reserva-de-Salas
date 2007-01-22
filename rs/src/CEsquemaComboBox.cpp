#include "CEsquemaComboBox.h"
#include "CSchemasModel.h"

#include <QPainter>
#include <QPalette>

class CEsquemaDelegate : public QAbstractItemDelegate
{
    public:
    	CEsquemaDelegate( CData * _data, QObject * parent );
    
    private:
    	CData * m_data;
    		
	public:
	    // painting
	    virtual void paint ( QPainter * painter, const QStyleOptionViewItem & option, 
	    						const QModelIndex & index ) const;
	               
		virtual QSize sizeHint ( const QStyleOptionViewItem & option, 
									const QModelIndex & index ) const;
};

CEsquemaDelegate::CEsquemaDelegate( CData * _data, QObject * parent ):
	QAbstractItemDelegate( parent ),
	m_data( _data )
{
}

QSize CEsquemaDelegate::sizeHint(const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const
{
    QString text = QString::fromAscii("Esquema ") + index.data(Qt::DisplayRole).toString();
    QFontMetrics fontMetrics(option.font);
    return QSize( fontMetrics.width(text), fontMetrics.lineSpacing() * 1.5 );
}


void CEsquemaDelegate::paint(QPainter *painter,
                                const QStyleOptionViewItem &option,
                                const QModelIndex &index) const
{
    QString text = QString::fromAscii("Esquema ") + index.data(Qt::DisplayRole).toString();
	
    QRect r = option.rect;

    if (option.state & QStyle::State_Selected) 
    {
        painter->save();
        painter->setBrush(option.palette.highlight());
        painter->setPen(Qt::NoPen);
        painter->drawRect(option.rect);
        painter->setPen(QPen(option.palette.highlightedText(), 0));
    }
    else
    {
        painter->save();
        
        QPalette * palette = m_data->getColorScheme( index.data(Qt::DisplayRole).toInt() );
        
        painter->setBrush( palette->brush(QPalette::Base) );
        painter->setPen(Qt::NoPen);
        painter->drawRect(option.rect);
        painter->setPen(QPen(palette->brush(QPalette::Text), 0));
    }	
    painter->drawText(r, Qt::AlignVCenter|Qt::AlignLeading|Qt::TextSingleLine, text);
    
    
    if (option.state & QStyle::State_Selected)
        painter->restore();
}


CEsquemaComboBox::CEsquemaComboBox( CData * _data, QWidget * parent ):
	QComboBox( parent )
{
	CSchemasModel model( _data );
	
	for ( int i = 0; i < model.rowCount(); ++i )
	{
		addItem( model.headerData( i, Qt::Vertical ).toString() );
	}
	
	setItemDelegate( new CEsquemaDelegate( _data, this ));
}

CEsquemaComboBox::~CEsquemaComboBox()
{
}
