#include "main.h"
#include "CData.h"
#include "../obj/ui_frschemas.h"
#include "CSchemasModel.h"

class CSchemas: public QDialog, public Ui::frSchemas
{
	Q_OBJECT
	
	public:
		CSchemas(CData* _data);
		~CSchemas();
		
	private:
		CData			*m_data;
		CSchemasModel	*m_model;

	private slots:
		void onAccept();
		
		void onsliderMoved();
		
		void cellDoubleClicked(const QModelIndex &index);
};
