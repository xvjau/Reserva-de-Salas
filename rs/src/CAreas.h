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

#ifndef CAREAS_H_
#define CAREAS_H_

#include "../obj/ui_frareas.h"
#include "CAreasModel.h"
#include "CData.h"
#include "CMainWindow.h"

class CAreas: public QDialog, public Ui::areas
{
		Q_OBJECT

	public:
		CAreas ( CData* _data, CMainWindow* _form );
		~CAreas();

	private:
		CAreasModel*	m_model;

	protected slots:
		void onAccept();
		void on_pbAdcionar_pressed();
		void on_pbRemover_pressed();
};

#endif /*CAREAS_H_*/
