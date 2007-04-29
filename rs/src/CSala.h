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
#ifndef __INCLUDE_CSALA_H
#define __INCLUDE_CSALA_H

#include "CSalaList.h"
#include "CData.h"

class CSala
{
		friend class CSalaList;

	private:
		CSalaList   *m_owner;
		CSala ( CSalaList *_owner );

	public:
		~CSala();

	private:
		int		SALAID;
		int		oldSALAID;

		int		ANDAR;
		QString	NOME;
		QString	AREA;
	public:
		bool	save();
		bool	del();

		int getOldSalaID() {return oldSALAID;};
		int getSalaID() {return SALAID;};
		void setSalaID ( const int _salaID );
		int getAndar() {return ANDAR;};
		void setAndar ( const int _andar ) {ANDAR = _andar;};
		QString getNome() {return NOME;};
		void setNome ( const QString &_nome ) {NOME = _nome;};
		QString getArea() {return AREA;};
		void setArea ( const QString &_area ) {AREA = _area;};

		CSalaList* getOwner() {return m_owner;};

		int m_column;
};
typedef CSala* PSala;

#endif
