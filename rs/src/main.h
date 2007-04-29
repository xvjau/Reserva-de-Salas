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

#ifndef __INCLUDE_MAIN_H
#define __INCLUDE_MAIN_H

#include "CData.h"

#include <QtCore/Qt>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QList>
#include <QtGui/QApplication>
#include <QtGui/QMessageBox>
#include <QtCore/QAbstractTableModel>
#include <QtCore/QFile>
#include <QtCore/QSettings>
#include <iostream>

class CSalas;
class CMainWindow;

enum IntervalKind { ikWeekly, ikMonthly, ikCustom };

inline QApplication* app()
{
	return static_cast<QApplication*> ( QApplication::instance() );
}

namespace Qt
{
	static const int LookUpRole = UserRole + 1;
}

typedef QList<int> TIntList;

class CUpdateLock
{
	public:
		CUpdateLock ( QWidget *widget ) :
				m_widget ( widget )
		{
			m_widget->setUpdatesEnabled ( false );
		};

		~CUpdateLock()
		{
			m_widget->setUpdatesEnabled ( true );
			m_widget->update();
		};

	private:
		QWidget     *m_widget;
};

class CWaitCursor
{
	public:
		CWaitCursor()
		{
			app()->setOverrideCursor ( Qt::WaitCursor );
		};

		~CWaitCursor()
		{
			app()->restoreOverrideCursor();
		};
};

QSettings* getConfigFile();
void deleteConfigFile();

#endif // __INCLUDE_MAIN_H
