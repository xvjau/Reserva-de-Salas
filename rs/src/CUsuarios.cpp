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

#include "CUsuarios.h"

#include <QFontMetrics>
#include <QHeaderView>

CUsuarios::CUsuarios ( CData* _data, QWidget *_parent ) :
		QDialog ( _parent ),
		m_data ( _data ),
		m_model ( 0 ),
		m_delegate ( 0 )
{
	setupUi ( this );
	setAttribute ( Qt::WA_DeleteOnClose );

	connect ( this, SIGNAL ( accepted() ), this, SLOT ( onAccept() ) );

	m_model = new CUsuariosModel ( m_data );
	tableView->setModel ( m_model );

	m_delegate = new CUsuarioDelegate ( m_data, this );
	tableView->setItemDelegate ( m_delegate );

	QFontMetrics fontMetrics ( tableView->horizontalHeader()->font() );

	int width = fontMetrics.width ( tr ( " Esquema XX " ) );;
	int x = 0;
	for ( int i = 3; i < 5; ++i )
	{
		x = fontMetrics.width ( m_model->headerData ( i, Qt::Horizontal ).toString() );
		if ( x > width ) width = x;
	}

	for ( int i = 3; i < 5; ++i )
	{
		tableView->setColumnWidth ( i, width + 10 );
	}
}

CUsuarios::~CUsuarios()
{
	if ( m_model )
		delete m_model;

	if ( m_delegate )
		delete m_delegate;
}

void CUsuarios::on_okButton_clicked()
{
	if ( m_model->hasUsersWithoutAreas() )
	{
		int ret = QMessageBox::warning ( this, tr ( "Usuários" ),
		                                 tr ( "Alguns usuários que possuem diretos de reservas<br>não possuem areas.<br><br>Deseja adciona-los à <b>todas</b> áreas?" ) +
		                                 tr ( "<br><br><small>Um usuário que não está alocado à nenhuma área não poderá fazer reservas." ),
		                                 QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
		                                 QMessageBox::Yes );

		switch ( ret )
		{
			case QMessageBox::Yes:
				m_model->addAllAreasToUsersWithoutAreas();
			case QMessageBox::No:
				emit accepted();
			case QMessageBox::Cancel:
				return;
		}
	}
	else
		emit accepted();
}

void CUsuarios::onAccept()
{
	m_model->CommitData();
	close();
}

void CUsuarios::on_pbRemover_clicked()
{
	QModelIndex index = tableView->selectionModel()->currentIndex();

	if ( index.isValid() )
	{

		QString usuario = m_model->data ( m_model->index ( index.row(), 0 ) ).toString();
		QString nome = m_model->data ( m_model->index ( index.row(), 1 ) ).toString();

		if ( nome.length() )
			usuario += " <i>(" + nome + ") </i>";

		if ( ! QMessageBox::question (
		            this,
		            tr ( "Excluir Usuário?" ),
		            tr ( "Tem certeza que deseja excluir o usuário(a) %1?" )
		            .arg ( usuario ),
		            tr ( "&Sim" ), tr ( "&Não" ),
		            QString(), 1, 0 ) )
		{
			if ( ! QMessageBox::question (
			            this,
			            tr ( "Excluir Usuário?" ),
			            tr ( "Isso irá remover também todas as reservas feitas por %1, tem certeza <i>mesmo</i> que você quer fazer isso?" )
			            .arg ( usuario ),
			            tr ( "&Sim" ), tr ( "&Não" ),
			            QString(), 1, 0 ) )
			{
				m_model->removeRows ( index.row(), 1 );
			}
		}
	}
}
