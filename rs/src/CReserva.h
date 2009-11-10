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
#ifndef __INCLUDE_CRESERVA_H
#define __INCLUDE_CRESERVA_H

#include "CData.h"

#include <QList>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QDate>
#include <QPalette>
#include <QLabel>
#include <QLayout>

class CReserva;
class CReservaList;

typedef QList<CReserva*> TListaReserva;

class CReservaLabel;

class CReserva: public QFrame
{
		Q_OBJECT

		friend class CReservaList;
		friend class CReservaLabel;

	private:
		CReservaList   *m_owner;
		CReserva ( CReservaList *_owner );

	public:
		~CReserva();
		TListaReserva m_children;

	protected:
		virtual void mouseDoubleClickEvent ( QMouseEvent * event );
		virtual void mousePressEvent ( QMouseEvent * event );
		virtual void paintEvent ( QPaintEvent * event );

	private:
		int		oldRESERVAID;
		int		RESERVAID;
		int		SALAID;
		int		oldSALAID;
		QTime	HORAIN;
		QTime	HORAFIM;
		QDate	DATA;
		int		USUARIOID;
		QString	USUARIO;
		QString	ASSUNTO;
		QString	DEPTO;
		QString	NOTAS;
		char	TIPO;

		bool	m_selected;
		
		// Semanal
		QDate	DATAIN;
		QDate	DATAFIM;
		bool	DIAS[7];
		bool	REVESA;
		bool	m_readSemanal;
		void	readSemanal();

		// Mensal
		int		ORDEM;
		int		DIA_S;
		bool	m_readMensal;
		void	readMensal();

		QVBoxLayout *vboxLayout;
		QHBoxLayout *hboxLayout;
		QVBoxLayout *vboxLayout1;

		CReservaLabel	*lblHoraIn;
		CReservaLabel	*lblHoraFim;
		CReservaLabel	*lblTitulo;

		int		m_height;

		bool m_relocate;
		bool m_deleting;

		CReserva*	m_parent;
	public:
		bool save();
		bool del();
		void refreshData();
		void relocate();

		void addChild ( CReserva* _reserva ) {m_children.append ( _reserva ); _reserva->m_parent = this;}
		CReserva* getParent() {return m_parent;}

		void setSelected( bool _value );
		bool getSelected() { return m_selected; };
		
		int		getoldRESERVAID() {return oldRESERVAID;};
		int		getRESERVAID() {return RESERVAID;};
		void	setRESERVAID ( int _value ) { RESERVAID = _value;};
		int		getSALAID() {return SALAID;};
		void	setSALAID ( int _value ) {SALAID = _value; m_relocate = true;};
		QTime	getHORAIN() {return HORAIN;};
		void    setHORAIN ( QTime _value ) { HORAIN = _value; m_relocate = true;};
		QTime	getHORAFIM() {return HORAFIM;};
		void	setHORAFIM ( QTime _value ) { HORAFIM = _value; m_relocate = true;};
		QDate	getDATA() {return DATA;};
		void	setDATA ( QDate _value ) {DATA = _value; m_relocate = true;};
		int		getUSUARIOID() {return USUARIOID;};
		void	setUSUARIOID ( int _value ) { USUARIOID = _value;};
		QString	getUSUARIO() {return USUARIO;};
		void	setUSUARIO ( QString _value ) { USUARIO = _value;};
		QString	getASSUNTO() {return ASSUNTO;};
		void	setASSUNTO ( QString _value ) { ASSUNTO = _value;};
		QString	getDEPTO() {return DEPTO;};
		void	setDEPTO ( QString _value ) { DEPTO = _value;};
		QString	getNOTAS() {return NOTAS;};
		void    setNOTAS ( QString _value ) { NOTAS = _value;};
		char	getTIPO() {return TIPO;};
		void	setTIPO ( char _value ) { TIPO = _value;};

		// Semanal ou Mensal
		QDate	getDATAIN();
		void	getDATAIN ( QDate _value ) {DATAIN = _value;};
		QDate	getDATAFIM();
		void	setDATAFIM ( QDate _value ) {DATAFIM = _value;};

		// Semanal
		bool	getDOW ( int _day );
		void	setDOW ( int _day, bool _value );
		bool    getREVESA();
		void    setREVESA ( bool _value ) {REVESA = _value;};

		QPalette*	m_colorScheme;
		CReservaList* 	getOwner() {return m_owner;};
		int		getHeight() {return m_height;};

	signals:
		void needRefresh();
};

#endif
