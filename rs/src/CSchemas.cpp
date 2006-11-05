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

#include "CSchemas.h"

CSchemas::CSchemas(CData* _data):
	m_data(_data),
	m_model(0)
{
	setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	
	connect(this, SIGNAL(accepted()), this, SLOT(onAccept()));

	connect(slRed, SIGNAL(valueChanged(int)), this, SLOT(onsliderMoved()));
	connect(slGreen, SIGNAL(valueChanged(int)), this, SLOT(onsliderMoved()));
	connect(slBlue, SIGNAL(valueChanged(int)), this, SLOT(onsliderMoved()));
	
	connect(tableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(cellDoubleClicked(QModelIndex)));
	
	m_model = new CSchemasModel(m_data);
	tableView->setModel(m_model);
	
	lblCor->setBackgroundRole(QPalette::Window);
	onsliderMoved();
}

CSchemas::~CSchemas()
{
	if (m_model)
	    delete m_model;
}

void CSchemas::onAccept()
{
	m_model->CommitData();
}

void CSchemas::onsliderMoved()
{
	QPalette palette;
	palette.setColor(QPalette::Window, QColor(slRed->value(),slGreen->value(),slBlue->value()));
	lblCor->setPalette(palette);
}

void CSchemas::cellDoubleClicked(const QModelIndex &index)
{
	int icolor	= (slRed->value() * 65536) +
					(slGreen->value() * 256) +
					(slBlue->value());
	
	std::cout << "Cor " << icolor << std::endl;
	m_model->setData(index, icolor, Qt::DisplayRole);
	tableView->repaint();
}
