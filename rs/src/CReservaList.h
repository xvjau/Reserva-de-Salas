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
#ifndef __INCLUDE_CRESERVALIST_H
#define __INCLUDE_CRESERVALIST_H

#include "CData.h"
#include "CSalaList.h"
#include "CMainWindow.h"
#include "CReserva.h"

#include <QFrame>
#include <QLayout>
#include <QLabel>
#include <QPainter>

class rowRESERVA;
class CReserva;
class CSemana;
class CReservaList: public QFrame
{
		Q_OBJECT

	private:
		CSemana		*m_owner;
		QDate		m_date;
		int			m_salaID;
		QPoint		m_position;

		QGridLayout *gridLayout;
		QVBoxLayout *vboxLayout;

	protected:
		virtual void mouseDoubleClickEvent ( QMouseEvent * event );
		virtual void mousePressEvent ( QMouseEvent * event );

	public:
		CReservaList ( CSemana *_owner, QDate _date, int _salaID );
		~CReservaList();

		bool loadList ( rowRESERVA* _row );

		TListaReserva	m_reservas;
		CReserva*	addReserva();
		void insertReserva ( CReserva* _reserva );

		int	getSALAID() {return m_salaID;};
		QDate	getDATA() {return m_date;};

		friend class CReserva;
		friend class rowRESERVA;

		void setPosition ( const QPoint _value ) { m_position = _value; };
		QPoint getPosition() const { return m_position; };

	signals:
		void showReservaMenu ( const QPoint _pos );
};

#endif
