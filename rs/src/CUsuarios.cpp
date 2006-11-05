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
