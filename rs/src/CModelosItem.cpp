#include "CModelosItem.h"
#include "main.h"
#include "CModelos.h"

CModelosItem::CModelosItem(Database _db, CModelos *_owner):
	m_ModeloID(-1),
	m_owner(_owner),
	m_db(_db)
{
	setupUi(this);
	if (! connect(this, SIGNAL(rejected()), this, SLOT(onClose())))
		std::cerr << "Error in signal this(rejected) this(onClose)";
	
	if (! connect(this, SIGNAL(accepted()), this, SLOT(onClose())))
		std::cerr << "Error in signal this(accepted) to this(onClose)";

	if (! connect(okButton, SIGNAL(clicked()), this, SLOT(onAccept())))
		std::cerr << "Error in signal okButton(clicked) to this(onAccept)";
}

CModelosItem::~CModelosItem()
{
}

void CModelosItem::onAccept()
{
	try
	{
		bool isInserting = (m_ModeloID == -1);
		Transaction tr = TransactionFactory(m_db, IBPP::amWrite);
		tr->Start();
		Statement stmt = StatementFactory(m_db, tr);
		
		if (isInserting)
		{
			stmt->Execute("Select GEN_ID(GENMODELOS, 1) From RDB$DATABASE");
			stmt->Fetch();
			stmt->Get(1, m_ModeloID);
			stmt->Close();

			stmt->Prepare("INSERT INTO MODELOS (NOME, BODY, ITEM, FUNDO, FUNDOALT, MODELOID)\
							VALUES (?,?,?,?,?,?)");
		}
		else
			stmt->Prepare("UPDATE MODELOS SET NOME = ?, BODY = ?, ITEM = ?, FUNDO = ?, FUNDOALT = ? WHERE MODELOID = ?");

		stmt->Set(1, leModelo->text().toStdString());
		stmt->Set(2, teBody->toPlainText().toStdString());
		stmt->Set(3, teItems->toPlainText().toStdString());
		stmt->Set(4, leFundo->text().toStdString());
		stmt->Set(5, leFundoAlt->text().toStdString());
		stmt->Set(6, m_ModeloID);

		stmt->Execute();

		if (stmt->AffectedRows() == 0)
			std::cerr << "WARN! 0 Rows Affected" << std::endl;
		
		stmt->Close();

		tr->Commit();

		CModelos::ROW_MODELOS *row;
		
		if (isInserting)
		{
			row = new CModelos::ROW_MODELOS;
			m_owner->m_modelos.push_back(row);
		}
		else
			row = m_owner->m_modelos[m_owner->comboBox->currentIndex()];

		row->MODELOID = m_ModeloID;
		row->NOME = leModelo->text();
		row->BODY = teBody->toPlainText();
		row->ITEM = teItems->toPlainText();
		row->FUNDO = leFundo->text();
		row->FUNDOALT = leFundoAlt->text();

		if (isInserting)
		{
			m_owner->comboBox->addItem(row->NOME);
			m_owner->comboBox->setCurrentIndex(m_owner->m_modelos.size()-1);
		}
		
		emit accepted();
		m_owner->render(m_owner->comboBox->currentIndex());
	}
	catch (SQLException &e)
	{
		if (e.SqlCode() == -803)
		{
			QMessageBox msg("Erro", "Já existe um modelo<br>com esse nome.", QMessageBox::Warning, QMessageBox::Cancel, 0, 0);
			msg.setWindowIcon(QIcon(":/png/buttons/16/agt_update_critical.png"));
			msg.setIconPixmap(QPixmap(":/png/buttons/16/kopete016.png"));
			msg.exec();
		}
	}
	catch (Exception &e)
	{
		{
			std::cerr << e.ErrorMessage() << std::endl;
			QMessageBox("Erro", e.ErrorMessage(), QMessageBox::Warning, QMessageBox::Cancel, 0, 0).exec();
		}
	}
}

void CModelosItem::onClose()
{
	delete this;
}
