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

#include "main.h"
#include "CData.h"
#include "../obj/ui_frschemas.h"
#include "CSchemasModel.h"

class CSchemas: public QDialog, public Ui::frSchemas
{
	Q_OBJECT
	
	public:
		CSchemas(CData* _data);
		~CSchemas();
		
	private:
		CData			*m_data;
		CSchemasModel	*m_model;
		QModelIndex 	m_activeIndex;

	private slots:
		void onAccept();
		void onsliderMoved();
		void on_tableView_pressed( const QModelIndex & index );
		void on_pbAlterar_clicked();
		void on_pbAdcionar_clicked();
		void on_pbRemover_clicked();
};
