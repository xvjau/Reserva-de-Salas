#include "CConfig.h"
#include <string>

#ifdef __unix__
#include <unistd.h>
#else
#undef UNICODE
#include <windows.h>
#endif

static CConfig* s_instance;

CConfig* CConfig::getConfig(CData *_data)
{
	if (!s_instance)
		s_instance = new CConfig(_data);
	
	return s_instance;
}

CConfig* CConfig::getConfig()
{
	return s_instance;
}


CConfig::CConfig(CData *_data):
	m_data(_data),
	m_loaded(false)
{
}

CConfig::~CConfig()
{
}

void CConfig::loadConfig()
{
	if (! m_loaded)
	{
		try
		{
	#ifdef __unix__
			const int BUFF_SIZE = L_cuserid;
			char    buff[BUFF_SIZE + 1];

			cuserid(buff);
	#else
			const int BUFF_SIZE = 512;
			char    buff[BUFF_SIZE + 1];

			DWORD ibuffSize = BUFF_SIZE;
			if (!GetUserName(buff, &ibuffSize))
			{
			    char lpMsgBuf[2048];
			    LPVOID lpDisplayBuf;
			    DWORD dw = GetLastError();

			    FormatMessage(
			        FORMAT_MESSAGE_FROM_SYSTEM,
			        NULL,
			        dw,
			        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			        (LPTSTR) &lpMsgBuf,
			        2048, NULL );

				QMessageBox("Windows SUCKS!", lpMsgBuf, QMessageBox::Warning,
				QMessageBox::Cancel, 0, 0).exec();
				throw -1;

			    LocalFree(lpMsgBuf);
			}
	#endif

			Transaction tr = TransactionFactory(m_data->m_db, amWrite);
			tr->Start();

		    Statement stmt = StatementFactory(m_data->m_db, tr);
			
			stmt->Prepare("Select USUARIOID, NOME, STYLE, SCHEMEID, NIVEL \
							From USUARIOS Where LOGIN = ?");

			stmt->Set(1, buff);
			stmt->Execute();

			if (stmt->Fetch())
			{
				std::string s;
	
				stmt->Get(1, m_userID);
				stmt->Get(2, s);
				m_userName = s.c_str();
				stmt->Get(3, s);
				m_style = s.c_str();
				stmt->Get(4, m_colorScheme);
				stmt->Get(5, m_userNivel);

				tr->Rollback();
			}
			else
			{
				m_userName = buff;
	
				m_style = "plastique";
			
				stmt->Close();
	
				int iCount = 0;
				stmt->Execute("Select GEN_ID(GENUSUARIOS, 1), COUNT(*) From USUARIOS");
				stmt->Fetch();
				stmt->Get(1, m_userID);
				stmt->Get(2, iCount);
				stmt->Close();
	
				if (iCount == 0)
					m_userNivel = 3;
				else
					m_userNivel = 0;
				/*
				stmt->Execute("Select First 1 \
									C.SCHEMEID, COUNT(*) \
								From COLOR_SCHEME C left outer join USUARIOS U on \
									U.SCHEMEID = C.SCHEMEID \
								Group By \
									C.SCHEMEID \
								Order By \
									COUNT(*)");
				*/
				stmt->Execute("Select First 1 \
									C.SCHEMEID, \
									(SELECT COUNT(*) FROM USUARIOS U WHERE U.SCHEMEID = C.SCHEMEID) \
								From \
									COLOR_SCHEME C \
								Order By \
									2");
	
				if (stmt->Fetch())
					stmt->Get(1, m_colorScheme);
				else
					m_colorScheme = 0;
	
				stmt->Close();
	
				stmt->Prepare("Insert Into USUARIOS (USUARIOID, LOGIN, NOME, STYLE, SCHEMEID, NIVEL) \
								Values (?, ?, ?, ?, ?, ?)");
	
				stmt->Set(1, m_userID);
				stmt->Set(2, buff);
				stmt->Set(3, buff);
				stmt->Set(4, m_style.toStdString());
				stmt->Set(5, m_colorScheme);
				stmt->Set(6, m_userNivel);
				stmt->Execute();
	
				tr->Commit();
			}
		}
		catch (Exception &e)
		{
			std::cerr << e.ErrorMessage() << std::endl;
			QMessageBox("Erro", e.ErrorMessage(), QMessageBox::Warning, QMessageBox::Cancel, 0, 0).exec();
		}
	
		m_loaded = true;
	}
}

int CConfig::getUsuarioID()
{
	loadConfig();
	return m_userID;
}

int CConfig::getNivel()
{
	loadConfig();
	return m_userNivel;
}

QString CConfig::getStyle()
{
	loadConfig();
	return m_style;
}

void CConfig::setStyle(QString _value)
{
	m_style = _value;
	
	Transaction tr = TransactionFactory(m_data->m_db, amWrite);
	tr->Start();
	
	Statement stmt = StatementFactory(m_data->m_db, tr);
	
	stmt->Prepare("Update USUARIOS Set STYLE = ? Where USUARIOID = ?");
	stmt->Set(1, m_style.toStdString());
	stmt->Set(2, m_userID);
	stmt->Execute();
	
	stmt->Close();
tr->Commit();
}

int CConfig::getColorScheme()
{
	loadConfig();
	return m_colorScheme;
}

