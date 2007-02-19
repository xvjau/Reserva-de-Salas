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

#include "CSchemasModel.h"

CSchemasModel::CSchemasModel( CData * _data ):
	QAbstractTableModel( _data ),
	m_data( _data )
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
	return 4;
}

QVariant CSchemasModel::data(const QModelIndex &index, int role) const
{
	switch (role)
	{
		case Qt::DisplayRole:
		{
			if (index.row() >= 0 && index.row() < m_rows.size() &&
			    index.column() == 3)
			{
				return tr(" Exemplo ");
			}
			break;
		}
		case Qt::BackgroundColorRole:
		{
			if (index.row() >= 0 && index.row() < m_rows.size() &&
			    index.column() >= 0 && index.column() < 4)
			{
				ROW_SCHEMAS *row = m_rows[index.row()];

				int i = 0;
				switch (index.column())
				{
					case 0:
					case 3: 
						i = row->BACKGROUND; 
						break;
					case 1: 
						i = row->FONT; 
						break;
					case 2: 
						i = row->BORDER; 
						break;
				}
				QString s;
				s.setNum(i, 16);
    			s = "#" + s.rightJustified(6, '0');
                return QColor(s);
			}
			return QVariant();
		}
		case Qt::ForegroundRole:
		{
			if (index.row() >= 0 && index.row() < m_rows.size() &&
			    index.column() == 3)
			{
				ROW_SCHEMAS *row = m_rows[index.row()];
				
				QString s;
				s.setNum(row->FONT, 16);
    			s = "#" + s.rightJustified(6, '0');
                return QColor(s);
			}
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
	  			case 0: return tr("Fundo");
				case 1: return tr("Fonte");
				case 2: return tr("Borda");
				case 3: return tr("Exemplo");
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
	if (role == Qt::BackgroundColorRole)
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
				
				emit dataChanged( index, index );
				return true;
			}
			stmt->Close();
		}
		catch (Exception &e)
		{
	        std::cerr << e.ErrorMessage() << std::endl;
	        QMessageBox(tr("Erro"), e.ErrorMessage(), QMessageBox::Warning, QMessageBox::Cancel, 0, 0).exec();
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
	beginInsertRows(parent, row, row + count - 1);
	bool result = false;

	ROW_SCHEMAS *rowData = new ROW_SCHEMAS;
	try
	{
		Statement stmt = StatementFactory(m_data->m_db, *m_tr);
		
		stmt->Execute("Select Max(SCHEMEID) From COLOR_SCHEME");
		
		if ( stmt->Fetch() )
			stmt->Get(1, rowData->SCHEMEID);
		else	
			throw QString("Unknown Error");
		
		stmt->Close();
		
		rowData->SCHEMEID++;
		rowData->BACKGROUND = 0;
		rowData->BORDER = 0;
		rowData->FONT = 0;

		stmt->Prepare("INSERT INTO COLOR_SCHEME (SCHEMEID) VALUES (?)");
		stmt->Set(1, rowData->SCHEMEID);
		stmt->Execute();
		stmt->Close();
		
		m_rows.push_back(rowData);
		result = true;
	}
	catch (Exception &e)
	{
		std::cerr << e.ErrorMessage() << std::endl;
		QMessageBox(tr("Erro"), e.ErrorMessage(), QMessageBox::Warning, QMessageBox::Cancel, 0, 0).exec();
	}
	
	endInsertRows();
	return result;
}

bool CSchemasModel::removeRows(int row, int count, const QModelIndex & parent)
{
	beginRemoveRows(parent, row, row + count - 1);
	bool result = false;

	ROW_SCHEMAS *rowData = m_rows[row];
	try
	{
		Statement stmt = StatementFactory(m_data->m_db, *m_tr);
		
		stmt->Prepare("Delete From COLOR_SCHEME Where SCHEMEID = ?");
		stmt->Set(1, rowData->SCHEMEID);	
		stmt->Execute();
		stmt->Close();
		
		m_rows.removeAt(row);
		delete rowData;
	
		result = true;
	}
	catch (Exception &e)
	{
		std::cerr << e.ErrorMessage() << std::endl;
		QMessageBox(tr("Erro"), e.ErrorMessage(), QMessageBox::Warning, QMessageBox::Cancel, 0, 0).exec();
	}
	
	endRemoveRows();
	return result;
}
