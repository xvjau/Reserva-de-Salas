#ifndef __INCLUDE_CSALAS_H
#define __INCLUDE_CSALAS_H

#include "main.h"
#include "../obj/ui_frsalas.h"
#include "CData.h"

class CSalas: public QDialog, public Ui::Salas
{
	Q_OBJECT
	
	public:
		CSalas(CMainWindow *_form, CData *_data);
		~CSalas();
		
	private:
		CSalaList	m_salaList;
		CData		*m_data;
		
		void setItemText(const int _row, const int _col, const QString &_text);

	public:
		CData*		getData(){ return m_data; };
		
	private slots:
		void saveSalas();
		void on_btAdicionar_clicked();
		void on_btAlterar_clicked();
		void on_btExcluir_clicked();
		void on_tbSalas_currentCellChanged ( int currentRow, int currentColumn, int previousRow, int previousColumn );
		
	public slots:
		void refreshData();
};

#endif // __INCLUDE_CSALAS_H
