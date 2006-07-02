#ifndef __INCLUDE_MAINWINDOW_H
#define __INCLUDE_MAINWINDOW_H

#include "main.h"
#include "../obj/ui_frmain.h"
#include "CData.h"
#include "CConfig.h"

#include <QPrinter>
#include <QResizeEvent>

class CMainWindow: public QMainWindow, public Ui::MainWindow
{
	Q_OBJECT
	
	public:
		CMainWindow();
		~CMainWindow();
	private:
		QActionGroup        m_stylesgroup;

        CSemana      *m_semana;
		CSalaList   *m_salaList;
		CReservaList::CReserva *m_activeReserva;

		CData	m_data;
		QDate   m_date;
		CConfig *m_config;
		
		QDate   m_activeDate;
		int     m_activeSalaID;
		
		void clearData();

		QMenu		m_mnPopupReserva;
		QMenu		m_mnPopupHoje;
		QPrinter	m_printer;
		
	private slots:
		void on_actionSalas_activated();
 		void on_actionUsuarios_activated();
 		void on_actionCores_activated();
		void on_actionAdicionar_activated();
		void on_actionRemover_activated();
		void on_actionAlterar_activated();
		void on_actionImprimirLista_activated();
		void on_actionImprimirReserva_activated();
		void on_actionHoje_activated();
		void on_btAnte_clicked();
		void on_btProx_clicked();
		void onSetStyle();
		
	protected:
		virtual void resizeEvent(QResizeEvent * event);
		virtual void mousePressEvent ( QMouseEvent * event );
		
	public:
		void checkRowHeight(int _row, int _salaID);
		QDate getDate() {return m_date;}
		
	public slots:
		void refreshSalas();
		void refreshData(const QDate &_date);
		
		void setActiveReserva(CReservaList::CReserva *_reserva);
		void setActiveDate(QDate _date) {m_activeDate = _date;};
		void setActiveSalaID(int _salaID) {m_activeSalaID = _salaID;};

        void showReservaMenu(const QPoint _pos);
};

#endif // __INCLUDE_MAINWINDOW_H
