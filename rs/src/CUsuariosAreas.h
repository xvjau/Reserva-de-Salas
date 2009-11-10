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
#ifndef CUSUARIOSAREAS_H_
#define CUSUARIOSAREAS_H_

#include <QWidget>
#include <QDialog>
#include <QStringList>
#include "../obj/ui_frusersareas.h"
#include "CData.h"

class CUsuariosAreas: public QDialog, public Ui::frUsersAreas
{
		Q_OBJECT

	public:
		CUsuariosAreas ( CData * _data, QWidget * _parent = 0 );
		virtual ~CUsuariosAreas();

	private:
		QStringList		m_areas;

	public:
		void setUserAreas ( QString _areas );
		QString getUserAreas();

	public slots:
		void on_tbAdd_clicked();
		void on_tbAddAll_clicked();
		void on_tbRemove_clicked();
		void on_tbRemoveAll_clicked();

		void on_lsAreas_doubleClicked ( const QModelIndex & index );
		void on_lsUserAreas_doubleClicked ( const QModelIndex & index );
};

#endif /*CUSUARIOSAREAS_H_*/
