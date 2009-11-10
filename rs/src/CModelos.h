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

#ifndef __INCLUDE_CMODELOS_H
#define __INCLUDE_CMODELOS_H

#include "CData.h"
#include "CReservaList.h"
#include "CReserva.h"

#include "../obj/ui_frmodelos.h"

#include <QTextDocument>
#include <QPrinter>

class CModelos: public QDialog, public Ui::Modelos
{
		Q_OBJECT

		friend class CModelosItem;

	public:
		CModelos ( Database _db, CReservaList *_lista, const QString _sala, QPrinter *_printer );
		CModelos ( Database _db, CReserva *_reserva, const QString _sala, QPrinter *_printer );
		~CModelos();

	private:
		struct ROW_MODELOS
		{
			int MODELOID;
			QString NOME;
			QString BODY;
			QString ITEM;
			QString FUNDO;
			QString FUNDOALT;
		};

		typedef QList<ROW_MODELOS*>	TModelosList;
		TModelosList m_modelos;

		Database	m_db;
		CReservaList	*m_lista;
		CReserva	*m_reserva;
		QString		m_sala;
		QPrinter	*m_printer;
		QString		m_doc;

		inline void init();
	public:
		int	getCount() const {return m_modelos.size();};
		QString getNomeModelo ( const int _id ) const {return m_modelos[_id]->NOME;};

	public slots:
		void render ( const int _id );
		void onAccept();
		void onClose();

		void on_btAdd_clicked();
		void on_btEdit_clicked();
		void on_btDelete_clicked();
};

#endif // __INCLUDE_CMODELOS_H
