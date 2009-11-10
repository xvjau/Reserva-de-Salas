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

#include "CModelos.h"
#include "main.h"
#include "CConfig.h"
#include "CModelosItem.h"

#include <QFile>
#include <QSettings>

void readString ( Statement *stmt, const int col, QString &_value )
{
	static std::string s;
	( *stmt )->Get ( col, s );
	_value = QString::fromStdString ( s );
}

CModelos::CModelos ( Database _db, CReservaList *_lista, const QString _sala, QPrinter *_printer ) :
		QDialog ( _lista ),
		m_db ( _db ),
		m_lista ( _lista ),
		m_reserva ( 0 ),
		m_sala ( _sala ),
		m_printer ( _printer )
{
	init();
}

CModelos::CModelos ( Database _db, CReserva *_reserva, const QString _sala, QPrinter *_printer ) :
		m_db ( _db ),
		m_lista ( 0 ),
		m_reserva ( _reserva ),
		m_sala ( _sala ),
		m_printer ( _printer )
{
	init();
}

void CModelos::init()
{
	setupUi ( this );
	setAttribute ( Qt::WA_DeleteOnClose );

	Transaction tr = TransactionFactory ( m_db );
	tr->Start();

	Statement stmt = StatementFactory ( m_db, tr );
	stmt->Prepare ( "Select MODELOID, NOME, BODY, ITEM, FUNDO, FUNDOALT From MODELOS Order By NOME" );
	stmt->Execute();

	ROW_MODELOS *row;
	while ( stmt->Fetch() )
	{
		row = new ROW_MODELOS;
		stmt->Get ( 1, row->MODELOID );
		readString ( &stmt, 2, row->NOME );
		readString ( &stmt, 3, row->BODY );
		readString ( &stmt, 4, row->ITEM );
		readString ( &stmt, 5, row->FUNDO );
		readString ( &stmt, 6, row->FUNDOALT );

		comboBox->addItem ( row->NOME );

		m_modelos.push_back ( row );
	}

	{
		QSettings settings ( "RolTram", "RS" );
		settings.beginGroup ( "modelos" );
		if ( settings.contains ( "last" ) )
		{
			int idx = comboBox->findText ( settings.value ( "last" ).toString() );
			if ( idx != -1 )
				comboBox->setCurrentIndex ( idx );
		}
	}

	stmt->Close();
	tr->Rollback();

	if ( ! connect ( comboBox, SIGNAL ( currentIndexChanged ( const int ) ), this, SLOT ( render ( const int ) ) ) )
		std::cerr << "Error in signal comboBox(currentIndexChanged) to this(render)";

	if ( ! connect ( this, SIGNAL ( accepted() ), this, SLOT ( onAccept() ) ) )
		std::cerr << "Error in signal this(accepted) to this(onAccept)";

	if ( ! connect ( this, SIGNAL ( rejected() ), this, SLOT ( onClose() ) ) )
		std::cerr << "Error in signal this(rejected) this(onClose)";

	if ( ! connect ( this, SIGNAL ( accepted() ), this, SLOT ( onClose() ) ) )
		std::cerr << "Error in signal this(accepted) to this(onClose)";

	if ( CConfig::getConfig()->getNivel() < 3 )
	{
		delete btAdd;
		delete btEdit;
		delete btDelete;
	}

	render ( comboBox->currentIndex() );
}

CModelos::~CModelos()
{
	TModelosList::iterator it;
	for ( it = m_modelos.begin(); it != m_modelos.end(); ++it )
		delete *it;
}

void CModelos::onClose()
{
	QSettings settings ( "RolTram", "RS" );
	settings.beginGroup ( "modelos" );
	settings.setValue ( "last", comboBox->currentText() );
}

void CModelos::render ( const int _id )
{
	if ( _id < 0 || _id >= m_modelos.size() )
		return;

	ROW_MODELOS *row = m_modelos[_id];

	QString sbody, sitem, sdata;

	bool icolorCycle = false;

	QString s;
	CReserva					*reserva;
	TListaReserva::iterator		it;

	if ( m_reserva )
	{
		reserva = m_reserva;
		goto render_reserva;
	}

	for ( it = m_lista->m_reservas.begin(); it != m_lista->m_reservas.end(); ++it )
	{
	render_reserva:
		reserva = *it;
		QString sassunto, sreserva = row->ITEM;

		sreserva.replace ( "%COLOR%", icolorCycle ? row->FUNDOALT : row->FUNDO );
		sreserva.replace ( "%HORAIN%", reserva->getHORAIN().toString ( "hh:mm" ) );
		sreserva.replace ( "%HORAFIM%", reserva->getHORAFIM().toString ( "hh:mm" ) );
		sreserva.replace ( "%ASSUNTO%", reserva->getASSUNTO() );

		if ( reserva->getDEPTO().length() )
			s = "(" + reserva->getDEPTO() + ")";
		else
			s = "";
		sreserva.replace ( "%DEPTO%", s );

		sdata += sreserva;

		if ( m_reserva )
			goto loop_end;

		icolorCycle = ! icolorCycle;
	}

loop_end:
	sbody = row->BODY;

	QString sdia;

	if ( m_reserva )
		sdia = m_reserva->getDATA().toString ( tr( "dddd dd 'de' MMMM" ));
	else
		sdia = m_lista->getDATA().toString ( tr("dddd dd 'de' MMMM" ));

	sbody.replace ( "%SALA%", m_sala );
	sbody.replace ( "%DATA%", sdia );
	sbody.replace ( "%BODY%", sdata );

	textBrowser->setHtml ( sbody );
	m_doc = sbody;
}

void CModelos::onAccept()
{
	QRect pageSize = m_printer->pageRect();
	QTextDocument doc;

	doc.setPageSize ( QSizeF ( pageSize.width(),pageSize.height() ) );
	doc.setHtml ( m_doc );

	doc.print ( m_printer );
}

void CModelos::on_btAdd_clicked()
{
	CModelosItem *modelositem = new CModelosItem ( m_db, this );

	modelositem->setModal ( true );
	modelositem->show();
}

void CModelos::on_btEdit_clicked()
{
	ROW_MODELOS *row = m_modelos[comboBox->currentIndex() ];

	CModelosItem *modelositem = new CModelosItem ( m_db, this );

	modelositem->setModeloID ( row->MODELOID );
	modelositem->leModelo->setText ( row->NOME );
	modelositem->leFundo->setText ( row->FUNDO );
	modelositem->leFundoAlt->setText ( row->FUNDOALT );

	QTextDocument *text;

	text = new QTextDocument ( modelositem->teBody );
	text->setPlainText ( row->BODY );
	modelositem->teBody->setDocument ( text );

	text = new QTextDocument ( modelositem->teItems );
	text->setPlainText ( row->ITEM );
	modelositem->teItems->setDocument ( text );

	modelositem->setModal ( true );
	modelositem->show();
}

void CModelos::on_btDelete_clicked()
{
	if ( ! QMessageBox::question (
				this,
				tr ( "Excluir Reserva" ),
				tr ( "Tem certeza que deseja excluir a reserva %1" )
				.arg ( comboBox->currentText() ),
				tr ( "&Sim" ), tr ( "&Não" ),
				QString(), 1, 0 ) )
	{
		try
		{
			Transaction tr = TransactionFactory ( m_db, IBPP::amWrite );
			tr->Start();

			Statement stmt = StatementFactory ( m_db, tr );
			stmt->Prepare ( "Delete From MODELOS Where MODELOID = ?" );

			stmt->Set ( 1, m_modelos[comboBox->currentIndex() ]->MODELOID );
			stmt->Execute();
			stmt->Close();
			tr->Commit();
		}
		catch ( Exception &e )
		{
			std::cerr << e.ErrorMessage() << std::endl;
			QMessageBox ( tr ( "Erro" ), e.ErrorMessage(), QMessageBox::Warning, QMessageBox::Cancel, 0, 0 ).exec();
			return;
		}
		m_modelos.removeAt ( comboBox->currentIndex() );
		comboBox->removeItem ( comboBox->currentIndex() );
	}
}
