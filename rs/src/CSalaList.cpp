#include "CSalaList.h"

CSalaList::CSalaList(CData *_owner, const int _areaId):
    m_owner(_owner),
    m_areaId(_areaId)
{
	m_tr = new Transaction;
	*m_tr = TransactionFactory(m_owner->m_db, amWrite);
	(*m_tr)->Start();
}

CSalaList::~CSalaList()
{
	if((*m_tr)->Started())
		(*m_tr)->Rollback();

	delete m_tr;
}

bool CSalaList::loadList()
{
	try
	{
		Statement stmt = StatementFactory(m_owner->m_db, *m_tr);

		stmt->Prepare
				("Select Distinct\
					SL.SALAID, \
					SL.ANDAR, \
					SL.NOME, \
					(Select First 1 \
						AR.AREA \
					From \
						AREAS AR \
							Join SALAS_AREAS SA on \
								SA.AREAID = AR.AREAID \
					Where \
						SA.SALAID = SL.SALAID) AREA \
				From \
					SALAS SL \
						Join SALAS_AREAS SA on \
							SA.SALAID = SL.SALAID \
				Where \
					(? = -1) OR \
					(SA.AREAID = ?) \
				Order By \
					SL.ANDAR, SL.SALAID");
		
		stmt->Set(1, m_areaId);
		stmt->Set(2, m_areaId);
		
		stmt->Execute();

		CSala* sala;

		std::string s;

		int i = 0;
		while (stmt->Fetch())
		{
			sala = addSala();

			stmt->Get(1, i);
			sala->setSalaID(i);
			sala->oldSALAID = sala->SALAID;

			stmt->Get(2, sala->ANDAR);
			
			stmt->Get(3, s);
			sala->NOME = s.c_str();

			if (! stmt->IsNull(4))
			{
				stmt->Get(4, s);
				sala->AREA = s.c_str();
			}
		}

		return true;
	}
	catch (Exception &e)
	{
		std::cerr << e.ErrorMessage() << std::endl;
		QMessageBox("Erro", e.ErrorMessage(), QMessageBox::Warning, QMessageBox::Cancel, 0, 0).exec();
		return false;
	}
}

PSala CSalaList::addSala()
{
	CSala *result = new CSala(this);
	return result;
}

void CSalaList::save()
{
	(*m_tr)->Commit();
}