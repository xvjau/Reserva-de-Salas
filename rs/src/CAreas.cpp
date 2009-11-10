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

#include "CAreas.h"
#include <QMessageBox>

CAreas::CAreas ( CData* _data, CMainWindow* _form ) :
		QDialog ( _form ),
		m_model ( 0 )
{
	setupUi ( this );
	setAttribute ( Qt::WA_DeleteOnClose );

	m_model = new CAreasModel ( _data );
	tableView->setModel ( m_model );

	connect ( this, SIGNAL ( accepted() ), this, SLOT ( onAccept() ) );
	connect ( this, SIGNAL ( accepted() ), _form, SLOT ( refreshAreas() ) );
}

CAreas::~CAreas()
{
	if ( m_model )
		delete m_model;
}

void CAreas::onAccept()
{
	m_model->CommitData();
}

void CAreas::on_pbAdcionar_pressed()
{
	m_model->insertRows ( m_model->rowCount(), 1 );
}

void CAreas::on_pbRemover_pressed()
{
	QModelIndex index = tableView->selectionModel()->currentIndex();

	if ( index.isValid() )
	{

		if ( ! QMessageBox::question (
		            this,
		            tr ( "Excluir Área?" ),
		            tr ( "Tem certeza que deseja excluir a área %1?" )
		            .arg ( index.data().toString() ),
		            tr ( "&Sim" ), tr ( "&Não" ),
		            QString(), 1, 0 ) )
		{
			m_model->removeRows ( index.row(), 1 );
		}
	}
}
