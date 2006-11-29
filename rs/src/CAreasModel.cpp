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
 
#include "CAreasModel.h"
#include "main.h"

CAreasModel::CAreasModel(CData* _data):
	m_data(_data)
{
	m_tr    = new Transaction();
	(*m_tr) = TransactionFactory(_data->m_db, amWrite, ilConcurrency, lrWait);
	(*m_tr)->Start();
	
	Statement stmt = StatementFactory(_data->m_db, *m_tr);
	
	stmt->Execute("Select AREAID, AREA From AREAS Order By AREA");
	
	std::string     s;
	ROW_AREAS		*row;
	
	while (stmt->Fetch())
	{
		row = new ROW_AREAS;
		m_rows.push_back(row);
		
		stmt->Get(1, row->AREAID);
	
		stmt->Get(2, s);
		row->AREA = s.c_str();
	}
	stmt->Close();
}

CAreasModel::~CAreasModel()
{
	if ((*m_tr)->Started())
	    (*m_tr)->Rollback();

	TROW_AREAS::iterator it;
    for (it = m_rows.begin(); it != m_rows.end(); ++it)
		delete *it;
}

int CAreasModel::rowCount(const QModelIndex &parent) const
{
	return m_rows.size();
}

int CAreasModel::columnCount(const QModelIndex &parent) const
{
	return 1;
}

QVariant CAreasModel::data(const QModelIndex &index, int role) const
{
	switch (role)
	{
	    case Qt::DisplayRole:
		case Qt::EditRole:
		{
			if (index.row() >= 0 && index.row() < m_rows.size() &&
			    index.column() >= 0 && index.column() < 2)
			{
				ROW_AREAS *row = m_rows[index.row()];

				switch (index.column())
				{
		  			case 0: return row->AREA;
				}
			}
			return QVariant();
		}
	}
	return QVariant();
}

QVariant CAreasModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole)
	{
		if (orientation == Qt::Horizontal)
		{
			switch (section)
			{
				case 0: return QString("Área");
			}
			return QVariant();
		}
		else
		{
			if (section < m_rows.size())
				return m_rows[section]->AREAID;

			return QVariant();
		}
	}
	return QVariant();
}

bool CAreasModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (role == Qt::DisplayRole || role == Qt::EditRole)
	{
		ROW_AREAS *row = m_rows[index.row()];
		
		try
		{
			Statement stmt = StatementFactory(m_data->m_db, *m_tr);
	
			stmt->Prepare("Update AREAS Set AREA = ? Where AREAID = ?");
	
			stmt->Set(1, value.toString().toStdString());
			stmt->Set(2, row->AREAID);
	
			stmt->Execute();
			stmt->Close();
			
			row->AREA = value.toString();
			
			return true;
		}
		catch (Exception &e)
		{
			std::cerr << e.ErrorMessage() << std::endl;
			QMessageBox("Erro", e.ErrorMessage(), QMessageBox::Warning, QMessageBox::Cancel, 0, 0).exec();
			return false;
		}
	}

	return false;
}

Qt::ItemFlags CAreasModel::flags(const QModelIndex & index) const
{
	if (index.column() == 0)
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	else
		return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

bool CAreasModel::insertRows(int row, int count, const QModelIndex & parent)
{
	beginInsertRows(parent, row, row + count - 1);

	ROW_AREAS *rowData = new ROW_AREAS;
	try
	{
		Statement stmt = StatementFactory(m_data->m_db, *m_tr);
		
		stmt->Execute("Select GEN_ID(GENAREAS, 1) From RDB$DATABASE");
		stmt->Get(1, rowData->AREAID);
		stmt->Close();
		
		m_rows.push_back(rowData);
		endInsertRows();
		
		return true;
	}
	catch (Exception &e)
	{
		std::cerr << e.ErrorMessage() << std::endl;
		QMessageBox("Erro", e.ErrorMessage(), QMessageBox::Warning, QMessageBox::Cancel, 0, 0).exec();
	}
	return false;
}

bool CAreasModel::removeRows(int row, int count, const QModelIndex & parent)
{
	return false;
	beginRemoveRows(parent, row, row + count - 1);
	
	endRemoveRows();
}
