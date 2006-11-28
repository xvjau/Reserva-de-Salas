#include "CDBSettings.h"
#include <QtCore/QSettings>

CDBSettings::CDBSettings(CMainWindow *_mainWindow):
	m_mainWindow(_mainWindow)
{
	setupUi(this);
	
	cbCharset->addItem("NONE");
	cbCharset->addItem("OCTETS");
	cbCharset->addItem("ASCII");
	cbCharset->addItem("UNICODE_FSS");
	cbCharset->addItem("UTF8");
	cbCharset->addItem("SJIS_0208");
	cbCharset->addItem("EUCJ_0208");
	cbCharset->addItem("DOS437");
	cbCharset->addItem("DOS850");
	cbCharset->addItem("DOS865");
	cbCharset->addItem("ISO8859_1");
	cbCharset->addItem("ISO8859_2");
	cbCharset->addItem("ISO8859_3");
	cbCharset->addItem("ISO8859_4");
	cbCharset->addItem("ISO8859_5");
	cbCharset->addItem("ISO8859_6");
	cbCharset->addItem("ISO8859_7");
	cbCharset->addItem("ISO8859_8");
	cbCharset->addItem("ISO8859_9");
	cbCharset->addItem("ISO8859_13");
	cbCharset->addItem("DOS852");
	cbCharset->addItem("DOS857");
	cbCharset->addItem("DOS860");
	cbCharset->addItem("DOS861");
	cbCharset->addItem("DOS863");
	cbCharset->addItem("CYRL");
	cbCharset->addItem("DOS737");
	cbCharset->addItem("DOS775");
	cbCharset->addItem("DOS858");
	cbCharset->addItem("DOS862");
	cbCharset->addItem("DOS864");
	cbCharset->addItem("DOS866");
	cbCharset->addItem("DOS869");
	cbCharset->addItem("WIN1250");
	cbCharset->addItem("WIN1251");
	cbCharset->addItem("WIN1252");
	cbCharset->addItem("WIN1253");
	cbCharset->addItem("WIN1254");
	cbCharset->addItem("NEXT");
	cbCharset->addItem("WIN1255");
	cbCharset->addItem("WIN1256");
	cbCharset->addItem("WIN1257");
	cbCharset->addItem("KSC_5601");
	cbCharset->addItem("BIG_5");
	cbCharset->addItem("GB_2312");
	cbCharset->addItem("KOI8R");
	cbCharset->addItem("KOI8U");
	cbCharset->addItem("WIN1258");
	
	connect(this, SIGNAL(accepted()), this, SLOT(onClose()));
	connect(this, SIGNAL(rejected()), this, SLOT(onCancel()));
	connect(this, SIGNAL(accepted()), m_mainWindow, SLOT(initialize()));
	
	QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope, ".");
	QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "Conf","RS");
	
	settings.beginGroup("DB");
	
	leDatabase->setText(settings.value("DB", "RS").toString());
	leServer->setText(settings.value("Server", "localhost").toString());
	leUser->setText(settings.value("User", "RS").toString());
	lePassword->setText(settings.value("Password", "rs").toString());
	
	int index = cbCharset->findText(settings.value("Characterset", "ISO8859_1").toString());
	cbCharset->setCurrentIndex(index);
}

CDBSettings::~CDBSettings()
{
}

void CDBSettings::onClose()
{
	QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope, ".");
	QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "Conf","RS");
	
	settings.beginGroup("DB");
	
	settings.setValue("DB", leDatabase->text());
	settings.setValue("Server", leServer->text());
	settings.setValue("User", leUser->text());
	settings.setValue("Password", lePassword->text());
	settings.setValue("Characterset", cbCharset->currentText());
}

void CDBSettings::onCancel()
{
	g_application->exit(-1);
}
