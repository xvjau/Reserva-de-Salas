#ifndef __INCLUDE_CDATA_H
#define __INCLUDE_CDATA_H

#include <ibpp.h>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QHash>
#include <QtCore/QString>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtGui/QFrame>
#include <QtGui/QLabel>
#include <QtGui/QScrollArea>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <QtGui/QPalette>

#include "CNotification.h"

using namespace IBPP;

Timestamp DateTimeToTimestamp(QDate _date, QTime _time);
QDate TimestampToDate(Timestamp _timestamp);

typedef QHash<int, QPalette*> TColorSchemeList;

class CMainWindow;
class CData: public QObject
{
	Q_OBJECT
	
	friend class CSalaList;
	friend class CSemana;

	public:
		CData();
		~CData();
	
	private:
		TColorSchemeList	m_colorSchemes;
		QStringList*		m_areas;
		QList<int>*			m_areasId;
		
		void loadColorSchemes();

	public:
		Database		m_db;
		CNotification*	m_notify;
		Events			m_event;

		bool connect();
		void disconnect();
		
		QPalette*		getColorScheme(int _id) { return m_colorSchemes[_id]; };
		QStringList*	getAreas();
		int				getAreaId(const int _areaListIndex);
		int				getAreaId(const QString &_area) {return getAreaId(getAreas()->indexOf(_area)); };
};

//====================  SALAS ===================================

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
	
	//====================  RESERVAS ===================================
	
class rowRESERVA;
class CSemana;
class CReservaList: public QFrame
{
	Q_OBJECT
	
	private:
		CSemana		*m_owner;
		QDate		m_date;
		int			m_salaID;
		
		QGridLayout *gridLayout;
		QVBoxLayout *vboxLayout;
		
	protected:
		virtual void mouseDoubleClickEvent(QMouseEvent * event);
		virtual void mousePressEvent(QMouseEvent * event);
		
	public:
		CReservaList(CSemana *_owner, QDate _date, int _salaID);
		~CReservaList();
		
		class CReserva;
		typedef QList<CReserva*> TListaReserva;

		class CReserva: public QFrame
		{
			friend class CReservaList;
	
			private:
				CReservaList   *m_owner;
				CReserva(CReservaList *_owner);
	
			public:
				~CReserva();
				TListaReserva m_children;
	
			protected:
				virtual void mouseDoubleClickEvent(QMouseEvent * event);
				virtual void mousePressEvent(QMouseEvent * event);
				virtual void paintEvent(QPaintEvent * event);
	
			private:
				int		oldRESERVAID;
				int		RESERVAID;
				int		SALAID;
				int		oldSALAID;
				QTime	HORAIN;
				QTime	HORAFIM;
				QDate	DATA;
				int		USUARIOID;
				QString	USUARIO;
				QString	ASSUNTO;
				QString	DEPTO;
				QString	NOTAS;
				char	TIPO;

				// Semanal
				QDate	DATAIN;
				QDate	DATAFIM;
				bool	DIAS[7];
				bool	REVESA;
				bool	m_readSemanal;
				void	readSemanal();
				
				// Mensal
				int		ORDEM;
				int		DIA_S;
				bool	m_readMensal;
				void	readMensal();
	
				QVBoxLayout *vboxLayout;
				QHBoxLayout *hboxLayout;
				QVBoxLayout *vboxLayout1;
	
				QLabel	*lblHoraIn;
				QLabel	*lblHoraFim;
				QLabel	*lblTitulo;
	
				int	m_height;
	
				bool m_relocate;
				bool m_deleting;
				
				CReserva*	m_parent;
			public:
				bool save();
				bool del();
				void refreshData();
				void relocate();
				
				void addChild(CReserva* _reserva) {m_children.append(_reserva); _reserva->m_parent = this;}
				CReserva* getParent() {return m_parent;}
	
				int		getoldRESERVAID() {return oldRESERVAID;};
				int		getRESERVAID() {return RESERVAID;};
				void	setRESERVAID(int _value) { RESERVAID = _value;};
				int		getSALAID() {return SALAID;};
				void	setSALAID(int _value) {SALAID = _value; m_relocate = true;};
				QTime	getHORAIN() {return HORAIN;};
				void    setHORAIN(QTime _value) { HORAIN = _value; m_relocate = true;};
				QTime	getHORAFIM() {return HORAFIM;};
				void	setHORAFIM(QTime _value) { HORAFIM = _value; m_relocate = true;};
				QDate	getDATA() {return DATA;};
				void	setDATA(QDate _value) {DATA = _value; m_relocate = true;};
				int		getUSUARIOID() {return USUARIOID;};
				void	setUSUARIOID(int _value) { USUARIOID = _value;};
				QString	getUSUARIO() {return USUARIO;};
				void	setUSUARIO(QString _value) { USUARIO = _value;};
				QString	getASSUNTO() {return ASSUNTO;};
				void	setASSUNTO(QString _value) { ASSUNTO = _value;};
				QString	getDEPTO() {return DEPTO;};
				void	setDEPTO(QString _value) { DEPTO = _value;};
				QString	getNOTAS() {return NOTAS;};
				void    setNOTAS(QString _value) { NOTAS = _value;};
				char	getTIPO() {return TIPO;};
				void	setTIPO(char _value) { TIPO = _value;};
				
				// Semanal ou Mensal
				QDate	getDATAIN();
				void	getDATAIN(QDate _value) {DATAIN = _value;};
				QDate	getDATAFIM();
				void	setDATAFIM(QDate _value) {DATAFIM = _value;};
				
				// Semanal
				bool	getDOW(int _day);
				void	setDOW(int _day, bool _value);
				bool    getREVESA();
				void    setREVESA(bool _value) {REVESA = _value;};
				
				QPalette*	m_colorScheme;
				CReservaList* 	getOwner() {return m_owner;};
				int		getHeight() {return m_height;};
		};
	
		typedef QList<CReserva*> TReservaList;
		bool loadList(rowRESERVA* _row);
	
		TReservaList	m_reservas;
		CReserva*	addReserva();
		void insertReserva(CReserva* _reserva);
	
		int	getSALAID() {return m_salaID;};
		QDate	getDATA() {return m_date;};
	
		friend class CReserva;
		friend class rowRESERVA;

	signals:
		void showReservaMenu(const QPoint _pos);
};

class rowRESERVA
{
	public:
		int RESERVAID;
		int SALAID;
		QDate DATA;
		QTime HORAIN;
		QTime HORAFIM;
		int USUARIOID;
		QString USUARIO;
		QString ASSUNTO;
		QString DEPTO;
		QString NOTAS;
		int SCHEMEID;
		char TIPO;

		rowRESERVA(Statement *stmt)
		{
			if (stmt)
			{
				Time ts;
				Date dt;
				int h, m, s, y, mo, d;
				std::string str;

				(*stmt)->Get(1, RESERVAID);
				(*stmt)->Get(2, SALAID),

				(*stmt)->Get(3, dt);
				dt.GetDate(y, mo, d);
				DATA = QDate(y, mo, d);

				(*stmt)->Get(4, ts);
				ts.GetTime(h, m, s);
				HORAIN = QTime(h, m, s);

				(*stmt)->Get(5, ts);
				ts.GetTime(h, m, s);
				HORAFIM = QTime(h, m, s);

				(*stmt)->Get(6, USUARIOID);

				(*stmt)->Get(7, str);
				USUARIO = str.c_str();
				
				(*stmt)->Get(8, str);
				ASSUNTO = str.c_str();

				(*stmt)->Get(9, str);
				DEPTO = str.c_str();

				(*stmt)->Get(10, str);
				NOTAS = str.c_str();

				(*stmt)->Get(11, SCHEMEID);
				
				(*stmt)->Get(12, str);
				TIPO = *str.c_str();
			}
			else
			{
				RESERVAID = -1;
				SALAID = -1;
				USUARIOID = -1;
				SCHEMEID = -1;
			}
		}

		inline bool checkRow(CReservaList* _list)
		{
			return (_list->m_salaID == SALAID) &&
					(_list->m_date == DATA);
		}
};

class CSemana: public QObject
{
	Q_OBJECT
	
	friend class CReservaList::CReserva;
	friend class CReservaList;
	friend class rowRESERVA;

	public:
		CSemana(CMainWindow *_parent, QDate &_segunda, CData *_owner,
				CSalaList *_salas, const int _areaId);
		~CSemana();
		
	private:
		CData			*m_owner;
		CSalaList		*m_salas;
		QDate			m_date;
		CMainWindow		*m_parent;
		int				m_areaId;
		
		typedef	QMap<int, CReservaList::CReserva*> TMapReservas;
		TMapReservas    m_reservaItems;
		
		Transaction		m_tr;
		Statement		m_stmt;
		rowRESERVA		m_row;
		void fetchRow();
		
		int				m_lastUpdate;
		
		typedef QMap<int, CReservaList*> TSemanaList;
		TSemanaList m_reservas[7];
		
		CReservaList* m_firstReservaList;
		
	public:
		bool			loadData();
		void			clear();
		CReservaList*	getReservaList(int _dow, int _salaID);
		CReservaList*	getFirstReservaList() {return m_firstReservaList;};

		int				getAreaId() {return m_areaId;};
		
	public slots:
		void onFBEvent(int event, int count);
};


#endif // __INCLUDE_CDATA_H

