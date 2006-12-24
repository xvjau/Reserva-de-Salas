//
// C++ Interface: CSalaList
//
// Description: 
//
//
// Author: Gianni Rossi <gianni.rossi@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
class CSala;

class CSalaList
{
	private:
		CData   		*m_owner;
		Transaction 	*m_tr;
		int				m_areaId;
		
	public:
		CSalaList(CData *_owner, const int _areaId = -1);
		~CSalaList();
		
		void save();
		
		Transaction* getTransaction(){return m_tr;};
		
		typedef QMap<int, CSala*> TSalaList;
		bool loadList();

		TSalaList   m_salas;
		CSala* addSala();
		
		friend class CSala;
};