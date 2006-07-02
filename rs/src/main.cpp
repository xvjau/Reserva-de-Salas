#include "main.h"
#include "CMainWindow.h"
#include <QSettings>

int main (int argc, char *argv[])
{
	QApplication app(argc, argv);
	g_application = &app;
	
	CMainWindow mainwindow;
	
	{
		QSettings settings("RolTram", "RS");
		settings.beginGroup("mainwindow");
		
		if (settings.contains("maxed") && (settings.value("maxed") == true))
			mainwindow.setWindowState(Qt::WindowMaximized);
		else
		{
			if (settings.contains("size"))
				mainwindow.resize(settings.value("size").toSize());
		}
	}
	
	mainwindow.show();

	int exitCode = app.exec();	
	
	{
		QSettings settings("RolTram", "RS");
		settings.beginGroup("mainwindow");
		
		if (mainwindow.windowState() == Qt::WindowMaximized)
			settings.setValue("maxed", true);
		else
		{
			settings.setValue("maxed", false);
			settings.setValue("size", mainwindow.size());
		}
	}
	
	return exitCode;
}
