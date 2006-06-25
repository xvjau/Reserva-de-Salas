#include "CSalasItem.h"
#include "CSalas.h"

CSalasItem::CSalasItem(PSala _sala, CSalas *_form):
	m_sala(_sala),
	m_validAndar(0, 4, this),
	m_validSalaID(0, 999, this)
{
	setupUi(this);
	
	connect(this, SIGNAL(rejected()), this, SLOT(cancel()));
	connect(this, SIGNAL(rejected()), this, SLOT(onClose()));
	
	connect(okButton, SIGNAL(clicked()), this, SLOT(ok()));
	connect(this, SIGNAL(accepted()), _form, SLOT(refreshData()));
	connect(this, SIGNAL(accepted()), this, SLOT(onClose()));

	connect(leNome, SIGNAL(textChanged(QString)), this, SLOT(onValidate()));
	connect(leSalaID, SIGNAL(textChanged(QString)), this, SLOT(onValidate()));
	
	leAndar->setValidator(&m_validAndar);
	leSalaID->setValidator(&m_validSalaID);
	
	leAndar->setText(QString::number(m_sala->getAndar()));
	leNome->setText(m_sala->getNome());
	leSalaID->setText(QString::number(m_sala->getSalaID()));
	
	onValidate();
}

CSalasItem::~CSalasItem()
{
}

void CSalasItem::onClose()
{
	delete this;
}

void CSalasItem::ok()
{
	m_sala->setAndar(leAndar->text().toInt());
	m_sala->setNome(leNome->text());
	m_sala->setSalaID(leSalaID->text().toInt());
	
	try
	{
		m_sala->save();
	    emit accepted();
	}
	catch (int SQLCode)
	{
		if (SQLCode == -803)
		{
		    QMessageBox msg("Erro", "Esse numero de sala já existe!", QMessageBox::Warning, QMessageBox::Cancel, 0, 0);
		    msg.setWindowIcon(QIcon(":/png/buttons/16/agt_update_critical.png"));
			msg.setIconPixmap(QPixmap(":/png/buttons/16/kopete016.png"));
			msg.exec();
		}
	}
}

void CSalasItem::cancel()
{
}

void CSalasItem::onValidate()
{
    okButton->setEnabled(leNome->text().length() &&
                        leSalaID->text().length());
}
