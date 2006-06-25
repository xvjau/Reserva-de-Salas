#include "CSchemasModel.h"

CSchemasModel::CSchemasModel(CData* _data):
	m_data(_data)
{
	m_tr    = new Transaction();
	(*m_tr) = TransactionFactory(_data->m_db, amWrite, ilConcurrency, lrWait);
	(*m_tr)->Start();

	Statement stmt = StatementFactory(_data->m_db, *m_tr);

	stmt->Execute("Select SCHEMEID, BACKGROUND, FONT, BORDER From COLOR_SCHEME Order By SCHEMEID");

	ROW_SCHEMAS 	*row;

	while (stmt->Fetch())
	{
		row = new ROW_SCHEMAS;
        m_rows.push_back(row);

		stmt->Get(1, row->SCHEMEID);
		stmt->Get(2, row->BACKGROUND);
		stmt->Get(3, row->FONT);
		stmt->Get(4, row->BORDER);
	}
	stmt->Close();
}

CSchemasModel::~CSchemasModel()
{
	if ((*m_tr)->Started())
	    (*m_tr)->Rollback();

    TROW_SCHEMAS::iterator it;
    for (it = m_rows.begin(); it != m_rows.end(); ++it)
		delete *it;
}

int CSchemasModel::rowCount(const QModelIndex &parent) const
{
	return m_rows.size();
}

int CSchemasModel::columnCount(const QModelIndex &parent) const
{
	return 3;
}

QVariant CSchemasModel::data(const QModelIndex &index, int role) const
{
	switch (role)
	{
		case Qt::BackgroundColorRole:
		{
			if (index.row() >= 0 && index.row() < m_rows.size() &&
			    index.column() >= 0 && index.column() < 4)
			{
				ROW_SCHEMAS *row = m_rows[index.row()];

				int i = 0;
				switch (index.column())
				{
					case 0: i = row->BACKGROUND; break;
					case 1: i = row->FONT; break;
					case 2: i = row->BORDER; break;
				}
				QString s;
				s.setNum(i, 16);
    			s = "#" + s.rightJustified(6, '0');
                return QColor(s);
			}
			return QVariant();
		}
	}
	return QVariant();
}

QVariant CSchemasModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole)
	{
		if (orientation == Qt::Horizontal)
		{
			switch (section)
			{
	  			case 0: return QString("Fundo");
				case 1: return QString("Fonte");
				case 2: return QString("Borda");
			}
			return QVariant();
		}
		else
		{
			if (section < m_rows.size())
				return m_rows[section]->SCHEMEID;

			return QVariant();
		}
	}
	return QVariant();
}

bool CSchemasModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (role == Qt::DisplayRole || role == Qt::EditRole)
	{
        ROW_SCHEMAS *row = m_rows[index.row()];

		QString field, sql;

		switch (index.column())
		{
			case 0: field = "BACKGROUND"; break;
			case 1: field = "FONT"; break;
			case 2: field = "BORDER"; break;
			default:
			    return false;
		}
		sql = "Update COLOR_SCHEME Set " + field + " = ? Where SCHEMEID = ?";

		try
		{
			Statement stmt = StatementFactory(m_data->m_db, *m_tr);

			stmt->Prepare(sql.toStdString());
			stmt->Set(1, value.toInt());
			stmt->Set(2, row->SCHEMEID);

			stmt->Execute();

			if (stmt->AffectedRows())
			{
				switch (index.column())
				{
		  			case 0: row->BACKGROUND = value.toInt(); break;
					case 1: row->FONT = value.toInt(); break;
					case 2: row->BORDER = value.toInt(); break;
				}
				stmt->Close();
				return true;
			}
			stmt->Close();
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

Qt::ItemFlags CSchemasModel::flags(const QModelIndex & index) const
{
	return Qt::ItemIsEnabled;
}

bool CSchemasModel::insertRows(int row, int count, const QModelIndex & parent)
{
	return false;
}

bool CSchemasModel::removeRows(int row, int count, const QModelIndex & parent)
{
    return false;
	beginRemoveRows(parent, row, row + count - 1);

	endRemoveRows();
}
