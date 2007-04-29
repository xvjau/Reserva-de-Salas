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

#include "CSemana.h"
#include "CSala.h"

CSemana::CSemana ( CMainWindow *_parent, QDate &_segunda, CData *_owner,
                   CSalaList *_salas, const int _areaId ) :
		m_date ( _segunda ),
		m_owner ( _owner ),
		m_salas ( _salas ),
		m_parent ( _parent ),
		m_firstReservaList ( 0 ),
		m_row ( 0 ),
		m_areaId ( _areaId )
{
	connect ( m_owner->m_notify, SIGNAL ( FBEvent ( int ) ), this, SLOT ( onFBEvent ( int ) ), Qt::QueuedConnection );
}

CSemana::~CSemana()
{
	clear();
	if ( m_tr->Started() )
		m_tr->Rollback();
}

void CSemana::fetchRow()
{
	if ( m_stmt->Fetch() )
		m_row = rowRESERVA ( &m_stmt );
	else
		m_row = rowRESERVA ( 0 );
}

bool CSemana::loadData()
{
	try
	{
		m_tr = TransactionFactory ( m_owner->m_db, amRead );

		m_tr->Start();

		m_stmt = StatementFactory ( m_owner->m_db, m_tr );

		m_stmt->Prepare ( "Select \
		                  RESERVAID, SALAID, DATA, HORAIN, HORAFIM, USUARIOID, \
		                  USUARIO_NOME, ASSUNTO, DEPTO, NOTAS, SCHEMEID, \
		                  TIPO, GEN_ID(SEQRESERVAS, 0) \
		                  From \
		                  GET_RESERVAS_SEMANA(?, ?, ?) \
		                  Order By \
		                  SALAID, \
		                  DATA, \
		                  HORAIN" );

		QDate day;
		Date pdate = Date ( m_date.year(), m_date.month(), m_date.day() );
		m_stmt->Set ( 1, pdate );
		pdate.Add ( m_parent->getDayInterval() - 1 );
		m_stmt->Set ( 2, pdate );
		m_stmt->Set ( 3, m_areaId );

		m_stmt->Execute();
		fetchRow();

		m_stmt->Get ( 13, m_lastUpdate );

		CReservaList*   reservaList;

		CSalaList::TSalaList::iterator it;
		PSala sala;

		for ( int idow = 0; idow < m_parent->getDayInterval(); ++idow )
		{
			m_reservas.append ( TSemanaListMap() );
			for ( int i = 0; i < m_reservas[idow].count(); ++i )
			{
				m_reservas[idow][i] = 0;
			}
		}

		for ( it = m_salas->m_salas.begin();
		        it != m_salas->m_salas.end();
		        ++it )
		{
			sala = *it;

			for ( int idow = 0; idow < m_parent->getDayInterval(); ++idow )
			{
				day = m_date.addDays ( idow );

				reservaList = new CReservaList ( this, day, sala->getSalaID() );
				reservaList->loadList ( &m_row );

				if ( ! connect ( reservaList, SIGNAL ( showReservaMenu ( QPoint ) ), m_parent, SLOT ( showReservaMenu ( QPoint ) ) ) )
					std::cerr << "Unable to connect reservaList(showReservaMenu) to m_parent(showReservaMenu)" << std::endl;

				m_reservas[idow].insert ( sala->getSalaID(), reservaList );

				if ( ! m_firstReservaList )
					m_firstReservaList = reservaList;
			}
		}

		m_stmt->Close();
		m_tr->Rollback();

		return true;
	}
	catch ( Exception &e )
	{
		std::cerr << e.ErrorMessage() << std::endl;
		QMessageBox ( tr ( "Erro" ), e.ErrorMessage(), QMessageBox::Warning, QMessageBox::Cancel, 0, 0 ).exec();
		return false;
	}
}

void CSemana::clear()
{
	for ( int idow = 0; idow < m_reservas.count(); ++idow )
	{
		for ( int i = 0; i < m_reservas[idow].count(); ++i )
		{
			if ( m_reservas[idow][i] )
			{
				delete m_reservas[idow][i];
			}
		}

		m_reservas[idow].clear();
	}

	m_reservas.clear();
}

CReservaList* CSemana::getReservaList ( int _dow, int _salaID )
{
	return m_reservas[_dow-1][_salaID];
}

void CSemana::onFBEvent ( int event )
{
	bool needRefresh = false;

	try
	{
		if ( event == CNotification::FBEUnknown ) return;

		m_tr = TransactionFactory ( m_owner->m_db, amRead );
		m_tr->Start();

		m_stmt = StatementFactory ( m_owner->m_db, m_tr );

		int iRow = 0,
		           iSalaID = 0;

		switch ( event )
		{
			case CNotification::FBEInsert:
			case CNotification::FBEUpdate:
				{
					m_stmt->Prepare ( "Select \
					                  RESERVAID, SALAID, DATA, HORAIN, HORAFIM, USUARIOID, \
					                  USUARIO_NOME, ASSUNTO, DEPTO, NOTAS, SCHEMEID, \
					                  TIPO, SEQ  \
					                  From \
					                  GET_RESERVAS_SEMANA(?, ?, ?) \
					                  Where \
					                  SEQ > ?\
					                  Order By \
					                  SALAID, \
					                  DATA, \
					                  HORAIN" );

					m_stmt->Set ( 1, Date ( m_date.year(), m_date.month(), m_date.day() ) );
					m_stmt->Set ( 2, Date ( m_date.year(), m_date.month(), m_date.day() + 6 ) );
					m_stmt->Set ( 3, m_areaId );
					m_stmt->Set ( 4, m_lastUpdate );

					m_stmt->Execute();

					CReservaList*   reservaList;

					fetchRow();
					m_stmt->Get ( 13, m_lastUpdate );

					while ( ( m_row.RESERVAID != -1 ) && ( ! needRefresh ) )
					{
						iRow = m_date.daysTo ( m_row.DATA ) + 1;
						iSalaID = m_row.SALAID;

						reservaList = getReservaList ( iRow, iSalaID );

						if ( event == CNotification::FBEInsert )
						{
							if ( reservaList )
							{
								reservaList->loadList ( &m_row );
							}
							else
							{
								std::cerr << "Warning!  Event recieved for non-existant Row" << std::endl;
								fetchRow();
							}
						}
						else
						{
							if ( m_row.TIPO == 'W' )
							{
								needRefresh = true;
							}
							else
							{
								CReserva* reserva = m_reservaItems[m_row.RESERVAID];

								if ( reserva )
								{
									reserva->setDATA ( m_row.DATA );
									reserva->setSALAID ( m_row.SALAID );
									reserva->setHORAIN ( m_row.HORAIN );
									reserva->setHORAFIM ( m_row.HORAFIM );
									reserva->setUSUARIOID ( m_row.USUARIOID );
									reserva->setUSUARIO ( m_row.USUARIO );
									reserva->setASSUNTO ( m_row.ASSUNTO );
									reserva->setDEPTO ( m_row.DEPTO );
									reserva->setNOTAS ( m_row.NOTAS );
									reserva->m_colorScheme = m_owner->getColorScheme ( m_row.SCHEMEID );

									reserva->relocate();
									reserva->refreshData();
								}

								fetchRow();
							}
						}

						m_parent->checkRowHeight ( iRow - 1, iSalaID );
					}
					m_stmt->Close();
					break;
				}
			case CNotification::FBEDelete:
				{
					m_stmt->Prepare ( "Select \
					                  RESERVAID, GEN_ID(SEQRESERVAS, 0) \
					                  From \
					                  DEL$RESERVAS \
					                  Where \
					                  SEQ > ?" );

					m_stmt->Set ( 1, m_lastUpdate );
					m_stmt->Execute();

					Timestamp ts;
					int iReservaID;

					CReserva* reserva;
					TListaReserva::iterator it;

					while ( m_stmt->Fetch() )
					{
						m_stmt->Get ( 1, iReservaID );
						m_stmt->Get ( 2, m_lastUpdate );

						reserva = m_reservaItems[iReservaID];
						if ( reserva )
						{
							delete reserva;
						}
					}
					break;
				}
		}
	}
	catch ( Exception &e )
	{
		std::cerr << e.ErrorMessage() << std::endl;
		QMessageBox ( tr ( "Erro" ), e.ErrorMessage(), QMessageBox::Warning, QMessageBox::Cancel, 0, 0 ).exec();
		return;
	}

	if ( needRefresh )
		m_parent->refreshData ( m_parent->getDate() );
}
