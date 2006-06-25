#ifndef __INCLUDE_CUSUARIO_MODEL_H
#define __INCLUDE_CUSUARIO_MODEL_H

#include "main.h"
#include "CData.h"

class CUsuariosModel : public QAbstractTableModel
{
	public:
    	CUsuariosModel(CData* _data);
    	virtual ~CUsuariosModel();
    	
	private:
        CData 			*m_data;
		Transaction     *m_tr;
		
		struct ROW_USUARIOS
		{
			int 	USUARIOID;
			QString	LOGIN;
			QString	NOME;
			QString	STYLE;
			int		SCHEMEID;
			int		NIVEL;
		};
		typedef QList<ROW_USUARIOS*> TROW_USUARIOS;
		TROW_USUARIOS m_rows;
		
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

#endif //__INCLUDE_CUSUARIO_MODEL_H
