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
#ifndef __INCLUDE_CSALALIST_H
#define __INCLUDE_CSALALIST_H

#include "CData.h"

#include <ibpp.h>

#include <QtCore/QMap>

class CSala;

class CSalaList
{
	private:
		CData   		*m_owner;
		Transaction 	*m_tr;
		int				m_areaId;

	public:
		CSalaList ( CData *_owner, const int _areaId = -1 );
		~CSalaList();

		void save();

		Transaction* getTransaction() {return m_tr;};

		typedef QMap<int, CSala*> TSalaList;
		bool loadList();

		TSalaList   m_salas;
		CSala* addSala();

		friend class CSala;
};

#endif
