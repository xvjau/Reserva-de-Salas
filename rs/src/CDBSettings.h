#ifndef CDBSETTINGS_H_
#define CDBSETTINGS_H_

#include <QDialog>
#include "../obj/ui_frdbsettings.h"
#include "CMainWindow.h"

class CDBSettings: public QDialog, public Ui::frdbsettings
{
	Q_OBJECT
	
	public:
		CDBSettings(CMainWindow *_mainWindow);
		virtual ~CDBSettings();
		
	private:
		CMainWindow	*m_mainWindow;
				
	public slots:
		void onClose();
		void onCancel();
};

#endif /*CDBSETTINGS_H_*/
