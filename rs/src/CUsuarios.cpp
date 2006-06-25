#include "CUsuarios.h"

CUsuarios::CUsuarios(CData* _data):
	m_data(_data),
	m_model(0)
{
	setupUi(this);

    connect(this, SIGNAL(accepted()), this, SLOT(onAccept()));

    connect(this, SIGNAL(rejected()), this, SLOT(onClose()));
	connect(this, SIGNAL(accepted()), this, SLOT(onClose()));

	m_model = new CUsuariosModel(m_data);
	tableView->setModel(m_model);
}

CUsuarios::~CUsuarios()
{
	if (m_model)
	    delete m_model;
}

void CUsuarios::onClose()
{
	delete this;
}

void CUsuarios::onAccept()
{
	m_model->CommitData();
}
