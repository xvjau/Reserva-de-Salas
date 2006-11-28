#include "CAreasModel.h"

CAreasModel::CAreasModel(CData* _data):
	m_data(_data)
{
	m_tr    = new Transaction();
	(*m_tr) = TransactionFactory(_data->m_db, amWrite, ilConcurrency, lrWait);
	(*m_tr)->Start();
	
	Statement stmt = StatementFactory(_data->m_db, *m_tr);
	
	stmt->Execute("Select \
						US.USUARIOID, \
						US.LOGIN, \
						US.NOME, \
						US.STYLE, \
						US.SCHEMEID, \
						US.NIVEL, \
						(Select First 1 \
							AR.AREA \
						From \
							AREAS AR \
								join USUARIOS_AREAS UA on \
									UA.AREAID = AR.AREAID \
						Where \
							UA.USUARIOID = US.USUARIOID) AREA \
					From \
						USUARIOS US \
					Order By \
						US.NOME");
	
	std::string     s;
	ROW_USUARIOS 	*row;
	
	while (stmt->Fetch())
	{
		row = new ROW_USUARIOS;
		m_rows.push_back(row);
		
		stmt->Get(1, row->USUARIOID);
	
		stmt->Get(2, s);
		row->LOGIN = s.c_str();

		stmt->Get(3, s);
		row->NOME = s.c_str();

		stmt->Get(4, s);
		row->STYLE = s.c_str();

		stmt->Get(5, row->SCHEMEID);
		stmt->Get(6, row->NIVEL);

		if (! stmt->IsNull(7))
		{
			stmt->Get(7, s);
			row->AREA = s.c_str();
		}
	}
	stmt->Close();
}

CAreasModel::~CAreasModel()
{
	if ((*m_tr)->Started())
	    (*m_tr)->Rollback();

	TROW_USUARIOS::iterator it;
    for (it = m_rows.begin(); it != m_rows.end(); ++it)
		delete *it;
}

int CAreasModel::rowCount(const QModelIndex &parent) const
{
	return m_rows.size();
}

int CAreasModel::columnCount(const QModelIndex &parent) const
{
	return 6;
}

QVariant CAreasModel::data(const QModelIndex &index, int role) const
{
	switch (role)
	{
	    case Qt::DisplayRole:
		case Qt::EditRole:
		{
			if (index.row() >= 0 && index.row() < m_rows.size() &&
			    index.column() >= 0 && index.column() < 6)
			{
				ROW_USUARIOS *row = m_rows[index.row()];

				switch (index.column())
				{
		  			case 0: return row->LOGIN;
					case 1: return row->NOME;
					case 2: return row->STYLE;
					case 3: return row->SCHEMEID;
					case 4: return row->NIVEL;
					case 5: return row->AREA;
				}
			}
			return QVariant();
		}
		case Qt::LookUpRole:
			if (index.column() == 5)
			{
				QVariant result = *m_data->getAreas();
				return result;
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
	  			case 0: return QString("Login");
				case 1: return QString("Nome");
				case 2: return QString("Estilo");
				case 3: return QString("Esquema");
				case 4: return QString("N�vel");
				case 5: return QString("�rea");
			}
			return QVariant();
		}
		else
		{
			if (section < m_rows.size())
				return m_rows[section]->USUARIOID;

			return QVariant();
		}
	}
	return QVariant();
}

bool CAreasModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (role == Qt::DisplayRole || role == Qt::EditRole)
	{
		ROW_USUARIOS *row = m_rows[index.row()];

		QString field, sql;

		switch (index.column())
		{
			case 0: field = "LOGIN"; break;
			case 1: field = "NOME"; break;
			case 2: field = "STYLE"; break;
			case 3: field = "SCHEMEID"; break;
			case 4: field = "NIVEL"; break;
			case 5:
			{
				try
				{
					Statement stmt = StatementFactory(m_data->m_db, *m_tr);
	
					stmt->Prepare("delete from USUARIOS_AREAS where USUARIOID = ?");
					stmt->Set(1, row->USUARIOID);
					stmt->Execute();
					stmt->Close();

					stmt->Prepare("insert into USUARIOS_AREAS (USUARIOID, AREAID) \
									select \
										?, \
										AREAID \
									from \
										AREAS \
									where \
										AREA = ?");
					stmt->Set(1, row->USUARIOID);
					stmt->Set(2, value.toString().toStdString());
					stmt->Execute();

					if (stmt->AffectedRows())
						row->AREA = value.toString();
					
					stmt->Close();
				}
				catch (Exception &e)
				{
					std::cerr << e.ErrorMessage() << std::endl;
					QMessageBox("Erro", e.ErrorMessage(), QMessageBox::Warning, QMessageBox::Cancel, 0, 0).exec();
					return false;
				}
				return true;
			}
			default:
			    return false;
		}
		sql = "Update USUARIOS Set " + field + " = ? Where USUARIOID = ?";
		
		try
		{
			Statement stmt = StatementFactory(m_data->m_db, *m_tr);
	
			stmt->Prepare(sql.toStdString());
	
			if (index.column() >= 3)
				stmt->Set(1, value.toInt());
			else
				stmt->Set(1, value.toString().toStdString());
	
			stmt->Set(2, row->USUARIOID);
	
			stmt->Execute();
	
			if (stmt->AffectedRows())
			{
				switch (index.column())
				{
		  			case 0: row->LOGIN = value.toString(); break;
					case 1: row->NOME = value.toString(); break;
					case 2: row->STYLE = value.toString(); break;
					case 3: row->SCHEMEID = value.toInt(); break;
					case 4: row->NIVEL = value.toInt(); break;
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

Qt::ItemFlags CAreasModel::flags(const QModelIndex & index) const
{
	return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

bool CAreasModel::insertRows(int row, int count, const QModelIndex & parent)
{
	beginInsertRows(parent, row, row + count - 1);

	ROW_USUARIOS *rowData = new ROW_USUARIOS;
	try
	{
		Statement stmt = StatementFactory(m_data->m_db, *m_tr);
		
		stmt->Execute("Select GEN_ID(GENUsuarioS, 1) From RDB$DATABASE");
		stmt->Get(1, rowData->USUARIOID);
		stmt->Close();
	
		stmt->Execute("Select First 1 SCHEMEID, COUNT(*) \
								From UsuarioS                    \
								Where                            \
								  Not SCHEMEID is Null           \
								Group By SCHEMEID                \
								Order By COUNT(*) Desc");
		stmt->Get(1, rowData->SCHEMEID);
		stmt->Close();
		
		rowData->NOME = QString::number(rowData->USUARIOID);
		
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