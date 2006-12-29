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

#ifndef __INCLUDE_CSALAS_H
#define __INCLUDE_CSALAS_H

#include "main.h"
#include "../obj/ui_frsalas.h"
#include "CData.h"
#include "CSalaList.h"

class CSalas: public QDialog, public Ui::Salas
{
	Q_OBJECT
	
	public:
		CSalas(CMainWindow *_form, CData *_data);
		~CSalas();
		
	private:
		CSalaList	m_salaList;
		CData		*m_data;
		
		void setItemText(const int _row, const int _col, const QString &_text);

	public:
		CData*		getData(){ return m_data; };
		
	private slots:
		void saveSalas();
		void on_btAdicionar_clicked();
		void on_btAlterar_clicked();
		void on_btExcluir_clicked();
		void on_tbSalas_currentCellChanged ( int currentRow, int currentColumn, int previousRow, int previousColumn );
		
	public slots:
		void refreshData();
};

#endif // __INCLUDE_CSALAS_H
