#include "CUsuarios.h"

CUsuarios::CUsuarios(CData* _data):
	m_data(_data),
	m_model(0),
	m_delegate(0)
{
	setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	
	connect(this, SIGNAL(accepted()), this, SLOT(onAccept()));
		
	m_model = new CUsuariosModel(m_data);
	tableView->setModel(m_model);

	m_delegate = new CComboBoxDelegate(this);
	tableView->setItemDelegate(m_delegate);
}

CUsuarios::~CUsuarios()
{
	if (m_model)
		delete m_model;

	if (m_delegate)
		delete m_delegate;
}

void CUsuarios::onAccept()
{
	m_model->CommitData();
}
