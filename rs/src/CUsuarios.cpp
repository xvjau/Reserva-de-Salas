#include "CUsuarios.h"

CUsuarios::CUsuarios(CData* _data):
	m_data(_data),
	m_model(0)
{
	setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	
	connect(this, SIGNAL(accepted()), this, SLOT(onAccept()));
		
	m_model = new CUsuariosModel(m_data);
	tableView->setModel(m_model);
}

CUsuarios::~CUsuarios()
{
	if (m_model)
		delete m_model;
}

void CUsuarios::onAccept()
{
	m_model->CommitData();
}
