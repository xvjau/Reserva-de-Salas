#ifndef CAREASMODEL_H_
#define CAREASMODEL_H_

#include <QAbstractTableModel>

class CAreasModel: public QAbstractTableModel
{
	public:
		CAreasModel(CData* _data);
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
		void CommitData() {(*m_tr)->Commit();};

		virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
		virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
		virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

		virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
		
		virtual bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
		virtual Qt::ItemFlags flags(const QModelIndex & index) const;
		
		virtual bool insertRows(int row, int count, const QModelIndex & parent = QModelIndex());
		virtual bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex());
}

#endif /*CAREASMODEL_H_*/
