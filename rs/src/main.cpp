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
#include "CMainWindow.h"
#include <QSettings>
#include <QLocale>
#include <QTranslator>

int main (int argc, char *argv[])
{
	QApplication app(argc, argv);
	g_application = &app;

	QString locale;

	{
		QSettings settings("RolTram", "RS");
		settings.beginGroup("Locale");
		
		if ( settings.contains("Locale") )
			locale = settings.value("Locale").toString();
		else
			locale = QLocale::system().name();
	}
	
	QTranslator translator;

	translator.load(QString("rs_") + locale);
	app.installTranslator(&translator);
	
	int exitCode = -1;
	
	CMainWindow mainwindow;
	
	if ( mainwindow.initialize() )
	{
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
	
		exitCode = app.exec();	
		
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
	}
		
	return exitCode;
}
