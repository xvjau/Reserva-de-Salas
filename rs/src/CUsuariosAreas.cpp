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
#include "CUsuariosAreas.h"
#include <QListWidgetItem>

CUsuariosAreas::CUsuariosAreas ( CData * _data, QWidget * _parent ) :
		QDialog ( _parent )
{
	setupUi ( this );
	setAttribute ( Qt::WA_DeleteOnClose );

	Transaction tr = TransactionFactory ( _data->m_db, amWrite, ilConcurrency, lrWait );
	tr->Start();

	Statement stmt = StatementFactory ( _data->m_db, tr );

	stmt->Execute ( "Select AREA From AREAS Order by AREA" );

	std::string s;

	while ( stmt->Fetch() )
	{
		stmt->Get ( 1, s );
		m_areas.append ( QString::fromStdString ( s ) );
	}
	stmt->Close();
}

CUsuariosAreas::~CUsuariosAreas()
{}

void CUsuariosAreas::setUserAreas ( QString _areas )
{
	lsAreas->clear();
	lsUserAreas->clear();

	QStringList list = _areas.split ( ", " );

	for ( int i = 0; i < m_areas.count(); ++i )
	{
		if ( list.indexOf ( m_areas[i] ) != -1 )
			lsUserAreas->addItem ( m_areas[i] );
		else
			lsAreas->addItem ( m_areas[i] );
	}
}

QString CUsuariosAreas::getUserAreas()
{
	QStringList list;

	for ( int i = 0; i < lsUserAreas->count(); ++i )
	{
		list.append ( lsUserAreas->item ( i )->text() );
	}

	return list.join ( ", " );
}

void CUsuariosAreas::on_tbAdd_clicked()
{
	int row = lsAreas->currentRow();
	QListWidgetItem * item = lsAreas->takeItem ( row );

	if ( item )
	{
		lsUserAreas->addItem ( item->text() );
		delete item;
	}
}

void CUsuariosAreas::on_tbAddAll_clicked()
{
	while ( lsAreas->count() )
	{
		QListWidgetItem * item = lsAreas->takeItem ( 0 );

		if ( item )
		{
			lsUserAreas->addItem ( item->text() );
			delete item;
		}
	}
}

void CUsuariosAreas::on_tbRemove_clicked()
{
	int row = lsUserAreas->currentRow();
	QListWidgetItem * item = lsUserAreas->takeItem ( row );

	if ( item )
	{
		lsAreas->addItem ( item->text() );
		delete item;
	}
}

void CUsuariosAreas::on_tbRemoveAll_clicked()
{
	while ( lsUserAreas->count() )
	{
		QListWidgetItem * item = lsUserAreas->takeItem ( 0 );

		if ( item )
		{
			lsAreas->addItem ( item->text() );
			delete item;
		}
	}
}


void CUsuariosAreas::on_lsAreas_doubleClicked ( const QModelIndex & index )
{
	Q_UNUSED( index );
	on_tbAdd_clicked();
}

void CUsuariosAreas::on_lsUserAreas_doubleClicked ( const QModelIndex & index )
{
	Q_UNUSED( index );
	on_tbRemove_clicked();
}
