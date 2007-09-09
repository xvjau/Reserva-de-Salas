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

#include "CSalas.h"
#include "CSalasItem.h"
#include "CMainWindow.h"

#include <QtGui/QHeaderView>

CSalas::CSalas ( CMainWindow *_form, CData *_data ) :
		QDialog ( _form ),
		m_data ( _data ),
		m_salaList ( _data )
{
	setupUi ( this );
	setAttribute ( Qt::WA_DeleteOnClose );

	tbSalas->verticalHeader()->setVisible ( false );

	connect ( this, SIGNAL ( accepted() ), this, SLOT ( saveSalas() ) );
	connect ( this, SIGNAL ( accepted() ), _form, SLOT ( refreshSalas() ) );

	connect ( tbSalas, SIGNAL ( cellDoubleClicked ( int, int ) ), this, SLOT ( on_btAlterar_clicked() ) );

	m_salaList.loadList();
	refreshData();

	btAlterar->setEnabled ( false );
	btExcluir->setEnabled ( false );
};

CSalas::~CSalas()
{}

void CSalas::saveSalas()
{
	m_salaList.save();
}

void CSalas::setItemText ( const int _row, const int _col, const QString &_text )
{
	QTableWidgetItem *item = tbSalas->item ( _row, _col );

	Qt::ItemFlags flags;
	flags |= Qt::ItemIsSelectable;
	flags |= Qt::ItemIsEnabled;

	if ( item )
	{
		item->setText ( _text );
	}
	else
	{
		item = new QTableWidgetItem ( _text );
		item->setFlags ( flags );
		tbSalas->setItem ( _row, _col, item );
	}
}

void CSalas::refreshData()
{
	CSalaList::TSalaList::iterator it;

	tbSalas->setRowCount ( m_salaList.m_salas.count() );

	int irow = 0;

	PSala sala;

	for ( it = m_salaList.m_salas.begin();
	        it != m_salaList.m_salas.end();
	        ++it, ++irow )
	{
		sala = *it;
		sala->m_column = irow;

		setItemText ( irow, 0, QString::number ( sala->getSalaID() ) );
		setItemText ( irow, 1, sala->getNome() );
		setItemText ( irow, 2, QString::number ( sala->getAndar() ) );
		setItemText ( irow, 3, sala->getArea() );
	}
}

void CSalas::on_btAdicionar_clicked()
{
	CSalasItem *salaitem = new CSalasItem ( m_salaList.addSala(), this );
	salaitem->setModal ( true );

	try
	{
		Statement stmt = StatementFactory ( m_data->m_db, * ( m_salaList.getTransaction() ) );

		stmt->Execute ( "Select MAX(SALAID)+1 From SALAS" );

		int i;
		if ( stmt->Fetch() && ! stmt->IsNull ( 1 ) )
		{
			stmt->Get ( 1, i );
			stmt->Close();
		}
		else
			i = 1;

		salaitem->leSalaID->setText ( QString::number ( i ) );
	}
	catch ( Exception &e )
	{
		std::cerr << "Fail" << std::endl;
		std::cerr << e.ErrorMessage() << std::endl;
	}

	salaitem->show();
}

void CSalas::on_btAlterar_clicked()
{
	int irow = tbSalas->currentRow();

	PSala sala;
	CSalaList::TSalaList::iterator it;
	for ( it = m_salaList.m_salas.begin();
	        it != m_salaList.m_salas.end();
	        ++it )
	{
		sala = *it;

		if ( sala->m_column == irow )
		{
			CSalasItem *salaitem = new CSalasItem ( sala, this );
			salaitem->setModal ( true );
			salaitem->show();
			break;
		}
	}
}

void CSalas::on_btExcluir_clicked()
{
	int irow = tbSalas->currentRow();

	PSala sala;
	CSalaList::TSalaList::iterator it;
	for ( it = m_salaList.m_salas.begin();
	        it != m_salaList.m_salas.end();
	        ++it )
	{
		sala = *it;

		if ( sala->m_column == irow )
		{
			QString txtSala = sala->getNome().length() ? sala->getNome() : QString::number ( sala->getSalaID() );

			if ( ! QMessageBox::question (
			            this,
			            tr ( "Excluir Sala?" ),
			            tr ( "Tem certeza que deseja excluir a sala %1" )
			            .arg ( txtSala ),
			            tr ( "&Sim" ), tr ( "&Não" ),
			            QString(), 1, 0 ) )
			{
				sala->del();
				refreshData();
				btAlterar->setEnabled ( false );
				btExcluir->setEnabled ( false );
			}
			break;
		}
	}
}

void CSalas::on_tbSalas_currentCellChanged ( int currentRow, int currentColumn, int previousRow, int previousColumn )
{
	Q_UNUSED( previousRow );
	Q_UNUSED( previousColumn );
	
	if ( currentRow >= 0 && currentColumn >= 0 )
	{
		btAlterar->setEnabled ( true );
		btExcluir->setEnabled ( true );
	}
	else
	{
		btAlterar->setEnabled ( false );
		btExcluir->setEnabled ( false );
	}
}
