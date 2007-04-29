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

#ifndef __INCLUDE_CCONFIG_H
#define __INCLUDE_CCONFIG_H

#include "main.h"
#include "CData.h"

class CConfig
{
	public:
		~CConfig();

		int getUsuarioID();
		int getNivel();

		QString getStyle();
		void setStyle ( QString _value );

		QString	getLastArea();
		void	setLastArea ( const QString &_area );

		int getColorScheme();

		QString getUserName() {return m_userName;};

		static CConfig* getConfig ( CData *_data );
		static CConfig* getConfig();

		const TIntList* getUserSalaList() {return &m_userSalaList;};
		int getUserAreaID() {return m_userArea;};

		void setIntervalKind ( const IntervalKind& theValue );
		IntervalKind getIntervalKind() const;

		void setDayInterval ( int theValue );
		int getDayInterval() const;

	private:
		CConfig ( CData *_data );

		CData		*m_data;

		bool		m_loaded;
		void		loadConfig();

		QString		m_userName;
		int			m_userID;
		int			m_userNivel;
		QString		m_style;
		int			m_colorScheme;
		TIntList	m_userSalaList;
		int			m_userArea;
		IntervalKind m_intervalKind;
		int			m_dayInterval;
};

#endif // __INCLUDE_CCONFIG_H
