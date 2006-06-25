#ifndef __INCLUDE_CCONFIG_H
#define __INCLUDE_CCONFIG_H

#include "main.h"
#include "CData.h"

class CConfig
{
	public:
		~CConfig();

		int getUsuarioID();
		int getNivel();
		
		QString getStyle();
		void setStyle(QString _value);
		
		int getColorScheme();
		
		QString getUserName() {return m_userName;};

		static CConfig* getConfig(CData *_data);
		static CConfig* getConfig();
	private:
        CConfig(CData *_data);

		CData	*m_data;
		
		bool    m_loaded;
		void	loadConfig();
		
		QString m_userName;
		int     m_userID;
		int     m_userNivel;
		QString m_style;
		int     m_colorScheme;
};

#endif // __INCLUDE_CCONFIG_H
