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
#ifndef __INCLUDE_ROWRESERVA_H
#define __INCLUDE_ROWRESERVA_H


#include "main.h"

#include <ibpp.h>

class CReservaList;
class rowRESERVA
{
	public:
		int RESERVAID;
		int SALAID;
		QDate DATA;
		QTime HORAIN;
		QTime HORAFIM;
		int USUARIOID;
		QString USUARIO;
		QString ASSUNTO;
		QString DEPTO;
		QString NOTAS;
		int SCHEMEID;
		char TIPO;

		rowRESERVA ( Statement *stmt );

		bool checkRow ( CReservaList* _list );
};

#endif

