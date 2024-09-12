#include "stdafx.h"
#include "NewVarEvent.h"


std::recursive_mutex				CNewVarEvent::m_lock;
std::list<CNewVarEvent *> 	CNewVarEvent::m_listeners;

CNewVarEvent::CNewVarEvent()
{
	std::unique_lock<std::recursive_mutex> lock(m_lock);
	m_listeners.push_back(this);
}


CNewVarEvent::~CNewVarEvent()
{
	std::unique_lock<std::recursive_mutex> lock(m_lock);
	m_listeners.remove(this);
}

void  CNewVarEvent::OnNewVar(int id, int subID, int core) 
{ 
	int ic = getCore();
	if (core == ic || ic == -1)
		OnNewVar(id, subID);
}

void CNewVarEvent::PushVar(int id, int subID, int core)
{
	for (std::list<CNewVarEvent *>::iterator i = m_listeners.begin(), ie = m_listeners.end(); i != ie; ++i) {
		(*i)->OnNewVar(id, subID, core);
	}
}