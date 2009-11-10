/*
	Reserva de Salas
	Copyright 2006 Gianfranco Rossi.

	Este programa é software livre; você pode redistribuí-lo e/ou
	modificá-lo sob os termos da Licença Pública Geral GNU, conforme
	publicada pela Free Software Foundation; tanto a versão 2 da
	Licença.

	Este programa é distribuído na expectativa de ser útil, mas SEM
	QUALQUER GARANTIA; sem mesmo a garantia implícita de
	COMERCIALIZAÇÃO ou de ADEQUAÇÃO A QUALQUER PROPÓSITO EM
	PARTICULAR. Consulte a Licença Pública Geral GNU para obter mais
	detalhes.

	Você deve ter recebido uma cópia da Licença Pública Geral GNU
	junto com este programa; se não, escreva para a Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
	02111-1307, USA.
 */

#ifndef __INCLUDE_MAINWINDOW_H
#define __INCLUDE_MAINWINDOW_H

#include "main.h"
#include "../obj/ui_frmain.h"
#include "CData.h"
#include "CConfig.h"
#include "CSemana.h"
#include "CReservaList.h"

#include <QPrinter>
#include <QResizeEvent>

class CMainWindow: public QMainWindow, public Ui::MainWindow
{
		Q_OBJECT

	public:
		CMainWindow ( QWidget * _parent = 0 );
		~CMainWindow();
	private:
		QActionGroup	m_stylesGroup;
		QActionGroup	m_intervalGroup;

		CSemana			*m_semana;
		CSalaList		*m_salaList;
		CReserva		*m_activeReserva;

		CData		m_data;
		QDate		m_date;
		CConfig		*m_config;

		IntervalKind m_intervalKind;
		int			m_dayInterval;

		QDate		m_activeDate;
		int			m_activeSalaID;

		void clearData();
		void updateButtons();
		void changeLocale ( const QString &locale );

		QMenu		m_mnPopupReserva;
		QMenu		m_mnPopupHoje;
		QPrinter	m_printer;

		bool		m_needRefresh;
		bool		m_canRefresh;
		bool		m_initialized;

	private slots:
		void on_actionSalas_triggered();
		void on_actionUsuarios_triggered();
		void on_actionCores_triggered();
		void on_actionAreas_triggered();
		void on_actionAdicionar_triggered();
		void on_actionRemover_triggered();
		void on_actionAlterar_triggered();
		void on_actionImprimirLista_triggered();
		void on_actionImprimirReserva_triggered();
		void on_actionCopiar_triggered();
		void on_actionHoje_triggered();
		void on_actionPortugu_s_triggered();
		void on_actionEnglish_triggered();
		void on_actionSemanal_triggered();
		void on_actionMensal_triggered();
		void on_actionOutro_triggered();
		void on_actionSobreRS_triggered();
		void on_actionSobreQt_triggered();
		void on_btAnte_clicked();
		void on_btProx_clicked();
		void onSetStyle();
		void cbAreaChanged ( int index );

	protected:
		virtual void resizeEvent ( QResizeEvent * event );
		virtual void showEvent ( QShowEvent * event );
		virtual void mousePressEvent ( QMouseEvent * event );

	public:
		void checkRowHeight ( int _row, int _salaID );
		QDate getDate() {return m_date;}

		void setCanRefresh ( bool _value ) { m_canRefresh = _value; };
		void checkRefresh() {if ( m_needRefresh ) refreshData ( m_date );};

		bool initialize();

		void setIntervalKind ( const IntervalKind& theValue );
		IntervalKind getIntervalKind() const { return m_intervalKind; }

		void setDayInterval ( int theValue );
		int getDayInterval() const;

	public slots:
		void refreshSalas();
		void refreshAreas();
		void refreshData ( const QDate &_date );
		void needRefresh();

		void setActiveReserva ( CReserva *_reserva );
		void checkActiveReservaDeleted ( CReserva *_reserva );
		void setActiveDate ( QDate _date ) {m_activeDate = _date;};
		void setActiveSalaID ( int _salaID ) {m_activeSalaID = _salaID;};

		void showReservaMenu ( const QPoint _pos );

};

#endif // __INCLUDE_MAINWINDOW_H
