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

#include "CUsuariosModel.h"
#include "CUsuarioDelegate.h"

#include <QStringList>

CUsuariosModel::CUsuariosModel ( CData* _data ) :
		QAbstractTableModel ( _data ),
		m_data ( _data )
{
	m_tr    = new Transaction();
	( *m_tr ) = TransactionFactory ( _data->m_db, amWrite, ilConcurrency, lrWait );
	( *m_tr )->Start();

	Statement stmt = StatementFactory ( m_data->m_db, *m_tr );
	Statement stmtArea = StatementFactory ( m_data->m_db, *m_tr );

	/*
	 * This SQL only checks if the user has an area.  If he does, then the 2nd SQL gets all the areas.
	 * This is done to avoid unnecessary round-trips to the server.
	 */
	stmt->Execute ( "Select \
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
							UA.USUARIOID = US.USUARIOID) HAS_AREA, \
						(Select \
							Count(*) \
						From \
							AREAS AR \
								join USUARIOS_AREAS UA on \
									UA.AREAID = AR.AREAID \
						Where \
							UA.USUARIOID = US.USUARIOID) AREA_COUNT \
					From \
						USUARIOS US \
					Order By \
						US.NOME" );

	stmtArea->Prepare ( "Select \
							AREA \
						From \
							AREAS AR \
								join USUARIOS_AREAS UA on \
									UA.AREAID = AR.AREAID \
						Where \
							UA.USUARIOID = ?" );

	std::string		s;
	QStringList		list;
	ROW_USUARIOS 	*row;

	while ( stmt->Fetch() )
	{
		row = new ROW_USUARIOS;
		m_rows.push_back ( row );

		stmt->Get ( 1, row->USUARIOID );

		stmt->Get ( 2, s );
		row->LOGIN = s.c_str();

		stmt->Get ( 3, s );
		row->NOME = s.c_str();

		stmt->Get ( 4, s );
		row->STYLE = s.c_str();

		if ( ! stmt->IsNull ( 5 ) )
			stmt->Get ( 5, row->SCHEMEID );
		else
			row->SCHEMEID = -1;

		stmt->Get ( 6, row->NIVEL );

		if ( ! stmt->IsNull ( 7 ) )
		{
			int count;
			stmt->Get ( 8, count );

			if ( count == 1 )
			{
				stmt->Get ( 7, s );
				row->AREA = QString::fromStdString ( s );
			}
			else
			{
				stmtArea->Set ( 1, row->USUARIOID );
				stmtArea->Execute();

				list.clear();
				while ( stmtArea->Fetch() )
				{
					if ( ! stmtArea->IsNull ( 1 ) )
					{
						stmtArea->Get ( 1, s );
						list.append ( QString::fromStdString ( s ) );
					}

					row->AREA = list.join ( ", " );
				}
			}
		}
	}
	stmt->Close();
}

CUsuariosModel::~CUsuariosModel()
{
	if ( ( *m_tr )->Started() )
		( *m_tr )->Rollback();

	TROW_USUARIOS::iterator it;
	for ( it = m_rows.begin(); it != m_rows.end(); ++it )
		delete *it;
}

bool CUsuariosModel::hasUsersWithoutAreas()
{
	Statement stmt = StatementFactory ( m_data->m_db, *m_tr );

	stmt->Execute ( "Select First 1 \
	                U.USUARIOID \
	                From \
	                USUARIOS U \
	                left join USUARIOS_AREAS UA on \
	                U.USUARIOID = UA.USUARIOID \
	                Where \
	                U.NIVEL > 0 \
	                and UA.AREAID IS NULL" );

	bool result = stmt->Fetch() && ! stmt->IsNull ( 1 );

	stmt->Close();

	return result;
}

void CUsuariosModel::addAllAreasToUsersWithoutAreas()
{
	Statement stmt = StatementFactory ( m_data->m_db, *m_tr );

	stmt->Execute ( "Insert into USUARIOS_AREAS (USUARIOID, AREAID) \
	                Select \
	                U.USUARIOID, \
	                A.AREAID \
	                From \
	                USUARIOS U \
	                left join USUARIOS_AREAS UA on \
	                U.USUARIOID = UA.USUARIOID \
	                join AREAS A on \
	                1 = 1 \
	                Where \
	                U.NIVEL > 0 \
	                and UA.AREAID IS NULL" );

	stmt->Close();
}

int CUsuariosModel::rowCount ( const QModelIndex &parent ) const
{
	Q_UNUSED( parent );
	return m_rows.size();
}

int CUsuariosModel::columnCount ( const QModelIndex &parent ) const
{
	Q_UNUSED( parent );
	return 6;
}

QVariant CUsuariosModel::data ( const QModelIndex &index, int role ) const
{
	switch ( role )
	{
		case Qt::DisplayRole:
		case Qt::EditRole:
			{
				if ( index.row() >= 0 && index.row() < m_rows.size() &&
				        index.column() >= 0 && index.column() < 6 )
				{
					ROW_USUARIOS *row = m_rows[index.row() ];

					switch ( index.column() )
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

		case Qt::BackgroundRole:
		case Qt::ForegroundRole:
			{
				if ( index.row() >= 0 && index.row() < m_rows.size() )
				{
					ROW_USUARIOS *row = m_rows[ index.row() ];

					if ( index.column() == 3 )
					{
						QPalette * palette = m_data->getColorScheme ( row->SCHEMEID );
						if ( palette )
						{

							return ( role == Qt::BackgroundRole ) ?
							       palette->brush ( QPalette::Base ) :
							       palette->brush ( QPalette::Text );
						}
					}
					else if ( ( role == Qt::ForegroundRole ) && ( row->NIVEL > 0 ) && row->AREA.isEmpty() )
					{
						return QBrush ( QColor ( 225, 0, 0 ) );
					}
				}
			}
		case Qt::ToolTipRole:
			{
				if ( index.row() >= 0 && index.row() < m_rows.size() )
				{
					ROW_USUARIOS *row = m_rows[ index.row() ];

					if ( ( index.column() != 3 ) &&
					        ( row->NIVEL > 0 ) && row->AREA.isEmpty() )
					{
						return tr ( "Não há áreas associadas à este usuário." );
					}
				}
			}
		case Qt::LookUpRole:
			if ( index.column() == 5 )
			{
				QVariant result = *m_data->getAreas();
				return result;
			}
	}
	return QVariant();
}

QVariant CUsuariosModel::headerData ( int section, Qt::Orientation orientation, int role ) const
{
	if ( role == Qt::DisplayRole )
	{
		if ( orientation == Qt::Horizontal )
		{
			switch ( section )
			{
				case 0: return tr ( "Login" );
				case 1: return tr ( "Nome" );
				case 2: return tr ( "Estilo" );
				case 3: return tr ( "Esquema" );
				case 4: return tr ( "Nível" );
				case 5: return tr ( "Área" );
			}
			return QVariant();
		}
		else
		{
			if ( section < m_rows.size() )
				return m_rows[section]->USUARIOID;

			return QVariant();
		}
	}
	return QVariant();
}

bool CUsuariosModel::setData ( const QModelIndex &index, const QVariant &value, int role )
{
	if ( role == Qt::DisplayRole || role == Qt::EditRole )
	{
		ROW_USUARIOS *row = m_rows[ index.row() ];

		QString field, sql;

		switch ( index.column() )
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
					Statement stmt = StatementFactory ( m_data->m_db, *m_tr );

					stmt->Prepare ( "Delete From USUARIOS_AREAS where USUARIOID = ?" );
					stmt->Set ( 1, row->USUARIOID );
					stmt->Execute();
					stmt->Close();

					QStringList list = value.toString().split ( ", " );

					for ( int i = 0; i < list.count(); )
					{
						stmt->Prepare ( "Insert into USUARIOS_AREAS (USUARIOID, AREAID) \
											Select \
												?, \
												AREAID \
											From \
												AREAS \
											Where \
												AREA = ?" );

						stmt->Set ( 1, row->USUARIOID );
						stmt->Set ( 2, list[i].toStdString() );
						stmt->Execute();

						if ( ! stmt->AffectedRows() )
							list.removeAt ( i );
						else
							++i;
					}

					row->AREA = list.join ( ", " );

					stmt->Close();
				}
				catch ( Exception &e )
				{
					std::cerr << e.ErrorMessage() << std::endl;
					QMessageBox ( "Erro", e.ErrorMessage(), QMessageBox::Warning, QMessageBox::Cancel, 0, 0 ).exec();
					return false;
				}
				emit dataChanged ( index, index );
				return true;
			}
			default:
				return false;
		}
		sql = "Update USUARIOS Set " + field + " = ? Where USUARIOID = ?";

		try
		{
			Statement stmt = StatementFactory ( m_data->m_db, *m_tr );

			stmt->Prepare ( sql.toStdString() );

			if ( index.column() >= 3 )
				stmt->Set ( 1, value.toInt() );
			else
				stmt->Set ( 1, value.toString().toStdString() );

			stmt->Set ( 2, row->USUARIOID );

			stmt->Execute();

			if ( stmt->AffectedRows() )
			{
				switch ( index.column() )
				{
					case 0: row->LOGIN = value.toString(); break;
					case 1: row->NOME = value.toString(); break;
					case 2: row->STYLE = value.toString(); break;
					case 3: row->SCHEMEID = value.toInt(); break;
					case 4: row->NIVEL = value.toInt(); break;
				}
				stmt->Close();
				emit dataChanged ( index, index );
				return true;
			}
			stmt->Close();
		}
		catch ( Exception &e )
		{
			std::cerr << e.ErrorMessage() << std::endl;
			if ( index.column() == 3 ) // Scheme ID
				QMessageBox ( tr ( "Erro" ), tr ( "Esse esquema de cores não existe!" ), QMessageBox::Warning, QMessageBox::Cancel, 0, 0 ).exec();
			else
				QMessageBox ( tr ( "Erro" ), e.ErrorMessage(), QMessageBox::Warning, QMessageBox::Cancel, 0, 0 ).exec();
			return false;
		}
	}

	return false;
}

Qt::ItemFlags CUsuariosModel::flags ( const QModelIndex & index ) const
{
	Q_UNUSED( index );
	return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

bool CUsuariosModel::insertRows ( int row, int count, const QModelIndex & parent )
{
	beginInsertRows ( parent, row, row + count - 1 );
	bool result = false;

	ROW_USUARIOS *rowData = new ROW_USUARIOS;
	try
	{
		Statement stmt = StatementFactory ( m_data->m_db, *m_tr );

		stmt->Execute ( "Select GEN_ID(GENUSUARIOS, 1) From RDB$DATABASE" );

		if ( stmt->Fetch() )
			stmt->Get ( 1, rowData->USUARIOID );
		else
			throw QString ( "Unknown Error" );

		stmt->Close();

		stmt->Execute ( "Select First 1 \
							SCHEMEID, \
							COUNT(*) \
						From \
							USUARIOS \
						Where \
							Not SCHEMEID is Null \
						Group By \
							SCHEMEID \
						Order By \
							COUNT(*) Desc" );

		if ( stmt->Fetch() )
			stmt->Get ( 1, rowData->SCHEMEID );
		else
			throw QString ( "Unknown Error" );

		stmt->Close();

		rowData->NOME = QString::number ( rowData->USUARIOID );

		m_rows.push_back ( rowData );
		result = true;
	}
	catch ( Exception &e )
	{
		std::cerr << e.ErrorMessage() << std::endl;
		QMessageBox ( tr ( "Erro" ), e.ErrorMessage(), QMessageBox::Warning, QMessageBox::Cancel, 0, 0 ).exec();
	}

	endInsertRows();
	return result;
}

bool CUsuariosModel::removeRows ( int row, int count, const QModelIndex & parent )
{
	beginRemoveRows ( parent, row, row + count - 1 );
	bool result = false;

	ROW_USUARIOS *rowData = m_rows[row];
	try
	{
		Statement stmt = StatementFactory ( m_data->m_db, *m_tr );

		stmt->Prepare ( "Delete From USUARIOS Where USUARIOID = ?" );
		stmt->Set ( 1, rowData->USUARIOID );
		stmt->Execute();
		stmt->Close();

		m_rows.removeAt ( row );
		delete rowData;
		result = true;
	}
	catch ( Exception &e )
	{
		std::cerr << e.ErrorMessage() << std::endl;
		QMessageBox ( tr ( "Erro" ), e.ErrorMessage(), QMessageBox::Warning, QMessageBox::Cancel, 0, 0 ).exec();
	}

	endRemoveRows();
	return result;
}
