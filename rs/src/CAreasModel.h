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

#ifndef CAREASMODEL_H_
#define CAREASMODEL_H_

#include <QAbstractTableModel>
#include "CData.h"

class CAreasModel: public QAbstractTableModel
{
		Q_OBJECT

	public:
		CAreasModel ( CData* _data );
		virtual ~CAreasModel();

	private:
		CData			*m_data;
		Transaction		*m_tr;

		struct ROW_AREAS
		{
			int 	AREAID;
			QString	AREA;
		};
		typedef QList<ROW_AREAS*> TROW_AREAS;
		TROW_AREAS m_rows;

	public:
		void CommitData() { ( *m_tr )->Commit();};

		virtual int rowCount ( const QModelIndex &parent = QModelIndex() ) const;
		virtual int columnCount ( const QModelIndex &parent = QModelIndex() ) const;
		virtual QVariant data ( const QModelIndex &index, int role = Qt::DisplayRole ) const;

		virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

		virtual bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
		virtual Qt::ItemFlags flags ( const QModelIndex & index ) const;

		virtual bool insertRows ( int row, int count, const QModelIndex & parent = QModelIndex() );
		virtual bool removeRows ( int row, int count, const QModelIndex & parent = QModelIndex() );
};

#endif /*CAREASMODEL_H_*/
