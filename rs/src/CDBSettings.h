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

#ifndef CDBSETTINGS_H_
#define CDBSETTINGS_H_

#include <QDialog>
#include "../obj/ui_frdbsettings.h"
#include "CMainWindow.h"

class CDBSettings: public QDialog, public Ui::frdbsettings
{
		Q_OBJECT

	public:
		CDBSettings ( CMainWindow *_mainWindow );
		virtual ~CDBSettings();

	private:
		CMainWindow		*m_mainWindow;
		int				m_result;

	public slots:
		void onClose();
		void onCancel();
		void on_helpButton_pressed();
};

#endif /*CDBSETTINGS_H_*/
