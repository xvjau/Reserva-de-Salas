#include "main.h"
#include "CData.h"

class CSchemasModel: public QAbstractTableModel
{
	public:
		CSchemasModel(CData* _data);
		virtual ~CSchemasModel();

	private:
        CData 			*m_data;
		Transaction     *m_tr;

		struct ROW_SCHEMAS
		{
			int SCHEMEID;
			int BACKGROUND;
			int FONT;
			int BORDER;
		};
		typedef QList<ROW_SCHEMAS*> TROW_SCHEMAS;
		TROW_SCHEMAS m_rows;

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
};

