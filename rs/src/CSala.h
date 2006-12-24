//
// C++ Interface: CSala
//
// Description: 
//
//
// Author: Gianni Rossi <gianni.rossi@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "CSalaList.h"
#include "CData.h"
	
class CSala
{
	friend class CSalaList;

	private:
		CSalaList   *m_owner;
		CSala(CSalaList *_owner);

	public:
		~CSala();

	private:
		int		SALAID;
		int		oldSALAID;
	
		int		ANDAR;
		QString	NOME;
		QString	AREA;
	public:
		bool	save();
		bool	del();

        int getOldSalaID() {return oldSALAID;};
		int getSalaID() {return SALAID;};
		void setSalaID(const int _salaID);
		int getAndar() {return ANDAR;};
		void setAndar(const int _andar) {ANDAR = _andar;};
		QString getNome() {return NOME;};
		void setNome(const QString &_nome){NOME = _nome;};
		QString getArea() {return AREA;};
		void setArea(const QString &_area){AREA = _area;};

		CSalaList* getOwner() {return m_owner;};

		int m_column;
};
typedef CSala* PSala;