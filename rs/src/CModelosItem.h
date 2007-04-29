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

#ifndef __INCLUDE_CMODELOSITEM_H
#define __INCLUDE_CMODELOSITEM_H

#include "CData.h"
#include "../obj/ui_frmodelositem.h"

class CModelos;

class CModelosItem: public QDialog, public Ui::ModeloItem
{
		Q_OBJECT

	public:
		CModelosItem ( Database _db, CModelos *_owner );
		~CModelosItem();

	private:
		Database	m_db;
		int			m_ModeloID;
		CModelos	*m_owner;

	public:
		void setModeloID ( const int _value ) {m_ModeloID = _value;};

	public slots:
		void onAccept();
};

#endif //__INCLUDE_CMODELOSITEM_H
