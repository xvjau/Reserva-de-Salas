#ifndef __INCLUDE_MAIN_H
#define __INCLUDE_MAIN_H

#include "CData.h"

#include <QtCore/Qt>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QDate>
#include <QtGui/QApplication>
#include <QtGui/QMessageBox>
#include <QtCore/QAbstractTableModel>
#include <iostream>

class CSalas;
class CMainWindow;

static QApplication *g_application;

class CUpdateLock
{
	public:
		CUpdateLock(QWidget *widget):
                m_widget(widget)
		    {
				m_widget->setUpdatesEnabled(false);
			};
			
		~CUpdateLock()
			{
				m_widget->setUpdatesEnabled(true);
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
				g_application->setOverrideCursor(Qt::WaitCursor);
			};
			
		~CWaitCursor()
		{
			g_application->restoreOverrideCursor();
		};
};

#endif // __INCLUDE_MAIN_H
