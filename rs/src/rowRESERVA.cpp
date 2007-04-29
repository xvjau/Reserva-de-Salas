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

#include "rowRESERVA.h"
#include "CReservaList.h"

rowRESERVA::rowRESERVA ( Statement *stmt )
{
	if ( stmt )
	{
		Time ts;
		Date dt;
		int h, m, s, y, mo, d;
		std::string str;

		( *stmt )->Get ( 1, RESERVAID );
		( *stmt )->Get ( 2, SALAID ),

		( *stmt )->Get ( 3, dt );
		dt.GetDate ( y, mo, d );
		DATA = QDate ( y, mo, d );

		( *stmt )->Get ( 4, ts );
		ts.GetTime ( h, m, s );
		HORAIN = QTime ( h, m, s );

		( *stmt )->Get ( 5, ts );
		ts.GetTime ( h, m, s );
		HORAFIM = QTime ( h, m, s );

		( *stmt )->Get ( 6, USUARIOID );

		( *stmt )->Get ( 7, str );
		USUARIO = str.c_str();

		( *stmt )->Get ( 8, str );
		ASSUNTO = str.c_str();

		( *stmt )->Get ( 9, str );
		DEPTO = str.c_str();

		( *stmt )->Get ( 10, str );
		NOTAS = str.c_str();

		( *stmt )->Get ( 11, SCHEMEID );

		( *stmt )->Get ( 12, str );
		TIPO = *str.c_str();
	}
	else
	{
		RESERVAID = -1;
		SALAID = -1;
		USUARIOID = -1;
		SCHEMEID = -1;
	}
}

bool rowRESERVA::checkRow ( CReservaList* _list )
{
	return ( _list->m_salaID == SALAID ) &&
	       ( _list->m_date == DATA );
}
